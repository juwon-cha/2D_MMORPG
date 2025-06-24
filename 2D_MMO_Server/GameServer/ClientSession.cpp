#include "pch.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"
#include "Player.h"
#include "ObjectManager.h"
#include "GameRoom.h"
#include "RoomManager.h"
#include "PacketManager.h"
#include "ContentsData.h"
#include "Item.h"
#include "ItemManager.h"

ClientSession::ClientSession()
{
	// 생성자에서 _lastPacketReceiveTime 초기화
	_lastPacketReceiveTime = std::chrono::steady_clock::now();
}

ClientSession::~ClientSession()
{
	cout << "~ClientSession" << endl;
}

void ClientSession::OnConnected()
{
	GSessionManager.Add(static_pointer_cast<ClientSession>(shared_from_this()));

	shared_ptr<Player> myPlayer = ObjectManager::Instance().Add<Player>();
	SetPlayer(myPlayer);

	Stat statData;
	auto iter = DataManager::Stats.find(1); // 레벨 1 시작
	if (iter != DataManager::Stats.end())
	{
		statData = iter->second;
	}

	_player->SetObjectInfo(myPlayer->GetObjectId(), "Player " + to_string(myPlayer->GetObjectId()));
	_player->SetPosInfo(1, 10, ObjectState_IDLE, MoveDir_DOWN);
	_player->SetStatInfo(statData.Level, statData.Speed, statData.Hp, statData.MaxHp, statData.Attack, statData.TotalExp);
	_player->SetClientSession(static_pointer_cast<ClientSession>(shared_from_this()));

#pragma region Item Setting
	// 접속할 때 검, 활 아이템 소유
	for (int32 i = 1; i <= 2; ++i)
	{
		// 플레이어가 가지고 있는 아이템 리스트(인벤토리)에 아이템 추가
		shared_ptr<Item> item = Item::MakeItem(i); // templateId에 따라 아이템을 만들고 메모리에 저장

		item->SetObjectId(ObjectManager::Instance().GenerateId(item->GetObjectType()));
		item->SetOwner(_player);
		item->SetOwnerId(_player->GetObjectId());
		item->SetTemplateId(i);
		item->SetCount(1);
		item->SetSlot(i - 1);
		item->SetEquipped(false);

		if (item != nullptr)
		{
			_player->GetInventory()->AddItem(item);
		}

		//RoomManager::Instance().Find(1)->EnterGame(item); 아이템 스폰
	}

	// TEMP Helmet 생성
	shared_ptr<Item> item = Item::MakeItem(100); // templateId에 따라 아이템을 만들고 메모리에 저장

	item->SetObjectId(ObjectManager::Instance().GenerateId(item->GetObjectType()));
	item->SetOwner(_player);
	item->SetOwnerId(_player->GetObjectId());
	item->SetTemplateId(100);
	item->SetCount(1);
	item->SetSlot(2);
	item->SetEquipped(false);

	if (item != nullptr)
	{
		_player->GetInventory()->AddItem(item);
	}

	// 플레이어의 아이템 목록 가져옴
	shared_ptr<ItemManager> inven = _player->GetInventory();

	// 클라이언트한테 아이템 목록 전달
	flatbuffers::FlatBufferBuilder builder;
	vector<flatbuffers::Offset<ItemInfo>> infoArray;
	for (const auto& pair : inven->GetItemList())
	{
		auto itemInfo = CreateItemInfo(builder, pair.second->GetObjectId(), pair.second->GetTemplateId(), pair.second->GetCount(), pair.second->GetSlot());
		infoArray.push_back(itemInfo);
	}
	auto itemArray = builder.CreateVector(infoArray);
	auto data = CreateSC_ITEM_LIST(builder, itemArray);
	auto itemListPkt = PacketManager::Instance().CreatePacket(data, builder, PacketType_SC_ITEM_LIST);
	Send(itemListPkt);
#pragma endregion

	// DB 없이 테스트
	//RoomManager::Instance().Find(1)->EnterGame(_player); // 1번 룸으로 입장(1번 맵)

	cout << "Connected: " << _player->GetObjectName() << endl;

	_lastPacketReceiveTime = std::chrono::steady_clock::now();
	cout << "[ClientSession] ID " << SessionId << ": Connected. Initialized lastPacketReceiveTime." << endl;
}

void ClientSession::OnDisconnected()
{
	cout << "Disconnected: " << _player->GetObjectName() << endl;

	// 현재 맵 아이디에 해당하는 게임 룸에서 떠남
	//RoomManager::Instance().Find(_player->GetMapId())->LeaveGame(_player->GetObjectId());

	//RoomManager::Instance().Find(_player->GetMapId())->PushJob(&GameRoom::LeaveGame, _player->GetObjectId());
	//GSessionManager.Remove(static_pointer_cast<ClientSession>(shared_from_this()));

	// 1. 플레이어 객체가 유효한지 먼저 확인
	shared_ptr<Player> player = GetPlayer();
	if (player == nullptr)
	{
		// 플레이어가 할당되기도 전에 접속이 끊긴 경우
		GSessionManager.Remove(static_pointer_cast<ClientSession>(shared_from_this()));
		return;
	}

	cout << "Disconnected: " << player->GetObjectName() << endl;

	// 2. 플레이어가 속한 룸 정보를 안전하게 가져온다
	shared_ptr<GameRoom> room = player->GetGameRoom();
	if (room == nullptr)
	{
		// 룸에 입장하기 전에 접속이 끊긴 경우
		GSessionManager.Remove(static_pointer_cast<ClientSession>(shared_from_this()));
		return;
	}

	// 3. 람다를 사용하여 '안전한 작업'을 예약
	// [=] 또는 [room, player] 캡처를 통해 room과 player의 shared_ptr 복사본을 만든다
	// 이 복사본 덕분에 람다가 실행되는 시점까지 두 객체의 생명이 보장
	room->PushJob([room, player]()
		{
			// 이 코드는 나중에 GameRoom의 스레드에서 안전하게 실행
			// room과 player는 여기서 유효함이 보장
			room->LeaveGame(player->GetObjectId());
		});

	// 4. 세션 매니저에서 현재 세션을 제거
	GSessionManager.Remove(static_pointer_cast<ClientSession>(shared_from_this()));
}

// 모든 패킷 수신 시 호출되는 함수 (PacketSession에서 오버라이드)
int32 ClientSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	UpdateLastPacketReceiveTime(); // 어떤 패킷이든 받으면 마지막 수신 시간 갱신
	// cout << "[ClientSession] ID " << SessionId() << ": Packet received. Updated lastPacketReceiveTime." << endl; // 너무 자주 찍힐 수 있으므로 주의

	PacketManager::Instance().OnRecvPacket(this, buffer);

	return len;
}

void ClientSession::OnSend(int32 len)
{
	//cout << "OnSend Len = " << len << endl;
}

void ClientSession::UpdateLastPacketReceiveTime()
{
	_lastPacketReceiveTime = std::chrono::steady_clock::now();
}

bool ClientSession::IsConnectionTimedOut(std::chrono::seconds timeoutDuration) const
{
	return std::chrono::steady_clock::now() > (_lastPacketReceiveTime.load() + timeoutDuration);
}

shared_ptr<Player> ClientSession::GetPlayer() const
{
	return _player;
}

void ClientSession::SetPlayer(shared_ptr<Player> player)
{
	_player = player;
}
