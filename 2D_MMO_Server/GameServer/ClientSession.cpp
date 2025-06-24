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
	// �����ڿ��� _lastPacketReceiveTime �ʱ�ȭ
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
	auto iter = DataManager::Stats.find(1); // ���� 1 ����
	if (iter != DataManager::Stats.end())
	{
		statData = iter->second;
	}

	_player->SetObjectInfo(myPlayer->GetObjectId(), "Player " + to_string(myPlayer->GetObjectId()));
	_player->SetPosInfo(1, 10, ObjectState_IDLE, MoveDir_DOWN);
	_player->SetStatInfo(statData.Level, statData.Speed, statData.Hp, statData.MaxHp, statData.Attack, statData.TotalExp);
	_player->SetClientSession(static_pointer_cast<ClientSession>(shared_from_this()));

#pragma region Item Setting
	// ������ �� ��, Ȱ ������ ����
	for (int32 i = 1; i <= 2; ++i)
	{
		// �÷��̾ ������ �ִ� ������ ����Ʈ(�κ��丮)�� ������ �߰�
		shared_ptr<Item> item = Item::MakeItem(i); // templateId�� ���� �������� ����� �޸𸮿� ����

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

		//RoomManager::Instance().Find(1)->EnterGame(item); ������ ����
	}

	// TEMP Helmet ����
	shared_ptr<Item> item = Item::MakeItem(100); // templateId�� ���� �������� ����� �޸𸮿� ����

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

	// �÷��̾��� ������ ��� ������
	shared_ptr<ItemManager> inven = _player->GetInventory();

	// Ŭ���̾�Ʈ���� ������ ��� ����
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

	// DB ���� �׽�Ʈ
	//RoomManager::Instance().Find(1)->EnterGame(_player); // 1�� ������ ����(1�� ��)

	cout << "Connected: " << _player->GetObjectName() << endl;

	_lastPacketReceiveTime = std::chrono::steady_clock::now();
	cout << "[ClientSession] ID " << SessionId << ": Connected. Initialized lastPacketReceiveTime." << endl;
}

void ClientSession::OnDisconnected()
{
	cout << "Disconnected: " << _player->GetObjectName() << endl;

	// ���� �� ���̵� �ش��ϴ� ���� �뿡�� ����
	//RoomManager::Instance().Find(_player->GetMapId())->LeaveGame(_player->GetObjectId());

	//RoomManager::Instance().Find(_player->GetMapId())->PushJob(&GameRoom::LeaveGame, _player->GetObjectId());
	//GSessionManager.Remove(static_pointer_cast<ClientSession>(shared_from_this()));

	// 1. �÷��̾� ��ü�� ��ȿ���� ���� Ȯ��
	shared_ptr<Player> player = GetPlayer();
	if (player == nullptr)
	{
		// �÷��̾ �Ҵ�Ǳ⵵ ���� ������ ���� ���
		GSessionManager.Remove(static_pointer_cast<ClientSession>(shared_from_this()));
		return;
	}

	cout << "Disconnected: " << player->GetObjectName() << endl;

	// 2. �÷��̾ ���� �� ������ �����ϰ� �����´�
	shared_ptr<GameRoom> room = player->GetGameRoom();
	if (room == nullptr)
	{
		// �뿡 �����ϱ� ���� ������ ���� ���
		GSessionManager.Remove(static_pointer_cast<ClientSession>(shared_from_this()));
		return;
	}

	// 3. ���ٸ� ����Ͽ� '������ �۾�'�� ����
	// [=] �Ǵ� [room, player] ĸó�� ���� room�� player�� shared_ptr ���纻�� �����
	// �� ���纻 ���п� ���ٰ� ����Ǵ� �������� �� ��ü�� ������ ����
	room->PushJob([room, player]()
		{
			// �� �ڵ�� ���߿� GameRoom�� �����忡�� �����ϰ� ����
			// room�� player�� ���⼭ ��ȿ���� ����
			room->LeaveGame(player->GetObjectId());
		});

	// 4. ���� �Ŵ������� ���� ������ ����
	GSessionManager.Remove(static_pointer_cast<ClientSession>(shared_from_this()));
}

// ��� ��Ŷ ���� �� ȣ��Ǵ� �Լ� (PacketSession���� �������̵�)
int32 ClientSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	UpdateLastPacketReceiveTime(); // � ��Ŷ�̵� ������ ������ ���� �ð� ����
	// cout << "[ClientSession] ID " << SessionId() << ": Packet received. Updated lastPacketReceiveTime." << endl; // �ʹ� ���� ���� �� �����Ƿ� ����

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
