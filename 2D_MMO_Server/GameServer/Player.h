#pragma once
#include "GameObject.h"
#include "Map.h"

class GameRoom;
class ClientSession;

class Player : public GameObject
{
public:
	Player();
	~Player() = default;

public:
	void OnDamaged(shared_ptr<GameObject> attacker, int32 damage) override;

public:
	shared_ptr<ClientSession> GetClientSession() const { return _session; };
	void SetClientSession(shared_ptr<ClientSession> session) { _session = session; }

private:
	shared_ptr<ClientSession> _session;
};
