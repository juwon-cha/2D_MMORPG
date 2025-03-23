#pragma once
#include "GameObject.h"
#include "Map.h"

class ClientSession;
class GameRoom;
class ItemManager;

class Player : public GameObject
{
public:
	Player();
	~Player() = default;

public:
	void OnDamaged(shared_ptr<GameObject> attacker, int32 damage) override;
	void OnDead(shared_ptr<GameObject> attacker) override;

public:
	shared_ptr<ClientSession> GetClientSession() const { return _session; };
	void SetClientSession(shared_ptr<ClientSession> session) { _session = session; }
	shared_ptr<ItemManager> GetInventory() const { return _inventory; }

private:
	shared_ptr<ClientSession> _session;
	shared_ptr<ItemManager> _inventory;
};
