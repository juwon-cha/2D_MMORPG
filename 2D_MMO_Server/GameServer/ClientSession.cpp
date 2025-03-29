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

void ClientSession::OnConnected()
{
	GSessionManager.Add(static_pointer_cast<ClientSession>(shared_from_this()));

	// Temp Player Settings
	shared_ptr<Player> myPlayer = ObjectManager::Instance().Add<Player>();
	SetPlayer(myPlayer);

	Stat statData;
	auto iter = DataManager::Stats.find(1); // 레벨 1 시작
	if (iter != DataManager::Stats.end())
	{
		statData = iter->second;
	}

	_player->SetObjectInfo(myPlayer->GetObjectId(), "Player " + to_string(myPlayer->GetObjectId()));
	_player->SetPosInfo(0, 0, ObjectState_IDLE, MoveDir_DOWN);
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
	RoomManager::Instance().Find(1)->EnterGame(_player);

	cout << "Connected: " << _player->GetObjectName() << endl;
}

void ClientSession::OnDisconnected()
{
	cout << "Disconnected: " << _player->GetObjectName() << endl;

	// 현재 맵 아이디에 해당하는 게임 룸에서 떠남
	RoomManager::Instance().Find(_player->GetMapId())->LeaveGame(_player->GetObjectId());

	GSessionManager.Remove(static_pointer_cast<ClientSession>(shared_from_this()));
}

int32 ClientSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketManager::Instance().OnRecvPacket(this, buffer);

	return len;
}

void ClientSession::OnSend(int32 len)
{
	//cout << "OnSend Len = " << len << endl;
}

shared_ptr<Player> ClientSession::GetPlayer() const
{
	return _player;
}

void ClientSession::SetPlayer(shared_ptr<Player> player)
{
	_player = player;
}
