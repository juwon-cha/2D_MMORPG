#include "pch.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"
#include "Player.h"
#include "ObjectManager.h"
#include "GameRoom.h"
#include "RoomManager.h"
#include "PacketManager.h"
#include "ContentsData.h"

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


	cout << "Connected: " << _player->GetObjectName() << endl;
}

void ClientSession::OnDisconnected()
{
	cout << "Disconnected: " << _player->GetObjectName() << endl;

	RoomManager::Instance().Find(1)->LeaveGame(_player->GetObjectId());

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
