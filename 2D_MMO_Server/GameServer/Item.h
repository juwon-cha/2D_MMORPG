#pragma once
#include "GameObject.h"

class Player;

class Item : public GameObject
{
public:
	Item();
	Item(ItemType itemType);
	virtual ~Item() = default;

public:
	int32 GetTemplateId() const { return _templateId; }
	int32 GetCount() const { return _count; }
	int32 GetSlot() const { return _slot; }
	ItemType GetItemType() const { return _itemType; }

	void SetTemplateId(int32 templateId) { _templateId = templateId; }
	void SetCount(int32 count) { _count = count; }
	void SetSlot(int32 slot) { _slot = slot; }

	int32 GetOwnerId() const { return _ownerId; }
	shared_ptr<Player> GetOwner() const { return _owner; }

	void SetOwnerId(int32 ownerId) { _ownerId = ownerId; }
	void SetOwner(shared_ptr<Player> owner) { _owner = owner; }

public:
	static shared_ptr<Item> MakeItem(int32 templateId);

private:
	int32 _templateId;
	int32 _count;
	int32 _slot;
	ItemType _itemType;

	int32 _ownerId;
	shared_ptr<Player> _owner;
};

class Weapon : public Item
{
public:
	Weapon(int32 templateId);
	~Weapon() = default;

	void Init(int32 templateId);

	WeaponType GetWeaponType() const { return _weaponType; }
	int32 GetDamage() const { return _damage; }

private:
	WeaponType _weaponType;
	int32 _damage;
};

class Armor : public Item
{
public:
	Armor(int32 templateId);
	~Armor() = default;

	void Init(int32 templateId);

private:
	int32 _defence;
};
