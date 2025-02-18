#pragma once

class GameRoom;
class ClientSession;

class Player
{
public:
	shared_ptr<GameRoom> GetGameRoom() const;
	void SetGameRoom(shared_ptr<GameRoom> room);

	shared_ptr<ClientSession> GetClientSession() const;
	void SetClientSession(shared_ptr<ClientSession> session);

	int32 GetPlayerId() const;
	std::string GetPlayerName() const;
	int32 GetPlayerPosX() const;
	int32 GetPlayerPosY() const;
	ObjectState GetPlayerState() const;
	MoveDir GetPlayerMoveDir() const;
	void SetPlayerId(int32 id);
	void SetPlayerInfo(int32 id, std::string name, int32 posX, int32 posY, ObjectState state, MoveDir moveDir);

private:
	shared_ptr<GameRoom> _room;
	shared_ptr<ClientSession> _session;
	int32 _id = 0;
	std::string _name;
	int32 _posX = 0;
	int32 _posY = 0;
	ObjectState _state = ObjectState_IDLE;
	MoveDir _moveDir = MoveDir_NONE;
};

