#include "pch.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"
#include "Player.h"
#include "PlayerManager.h"
#include "GameRoom.h"
#include "RoomManager.h"
#include "PacketManager.h"

void ClientSession::OnConnected()
{
	GSessionManager.Add(static_pointer_cast<ClientSession>(shared_from_this()));

	// Temp Player Settings
	shared_ptr<Player> myPlayer = PlayerManager::Instance().Add();
	SetPlayer(myPlayer);

	_player->SetPlayerInfo(myPlayer->GetPlayerId(), "Player " + to_string(myPlayer->GetPlayerId()), 0, 0, ObjectState_IDLE, MoveDir_NONE);
	_player->SetClientSession(static_pointer_cast<ClientSession>(shared_from_this()));

	RoomManager::Instance().Find(1)->EnterGame(_player);

	cout << "Connected: " << _player->GetPlayerName() << endl;
}

void ClientSession::OnDisconnected()
{
	cout << "Disconnected: " << _player->GetPlayerName() << endl;

	RoomManager::Instance().Find(1)->LeaveGame(_player->GetPlayerId());

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
