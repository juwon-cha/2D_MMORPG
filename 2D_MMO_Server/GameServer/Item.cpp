#include "pch.h"
#include "Item.h"
#include "ContentsData.h"
#include "ObjectManager.h"

Item::Item()
	: _templateId(0)
	, _count(0)
	, _slot(0)
	, _itemType(ItemType_ITEM_NONE)
	, _ownerId(0)
	, _owner(nullptr)
{
	_type = GameObjectType_ITEM;
}

Item::Item(ItemType itemType)
	: _templateId(0)
	, _count(0)
	, _slot(0)
	, _itemType(itemType)
	, _ownerId(0)
	, _owner(nullptr)
{
	_type = GameObjectType_ITEM;
}

shared_ptr<Item> Item::MakeItem(int32 templateId)
{
	shared_ptr<Item> item = nullptr;
	shared_ptr<ItemData> itemData = nullptr;

	auto iter = DataManager::Items.find(templateId);
	if (iter != DataManager::Items.end())
	{
		itemData = iter->second;
	}

	if (itemData == nullptr)
	{
		return nullptr;
	}

	switch (itemData->ItemType)
	{
	case ItemType_ITEM_WEAPON:
		item = make_shared<Weapon>(templateId);
		break;

	case ItemType_ITEM_ARMOR:
		item = make_shared<Armor>(templateId);
		break;

	case ItemType_ITEM_NONE:
		break;
	}

	return item;
}

Weapon::Weapon(int32 templateId)
	: Item(ItemType_ITEM_WEAPON)
	, _weaponType(WeaponType_WEAPON_NONE)
	, _damage(0)
{
	Init(templateId);
}

void Weapon::Init(int32 templateId)
{
	shared_ptr<ItemData> itemData = nullptr;

	auto iter = DataManager::Items.find(templateId);
	if (iter != DataManager::Items.end())
	{
		itemData = iter->second;
	}

	if (itemData->ItemType != ItemType_ITEM_WEAPON)
	{
		CRASH();
		return;
	}

	shared_ptr<WeaponData> weaponData = static_pointer_cast<WeaponData>(itemData);
	SetTemplateId(weaponData->ID);
	SetCount(1);
	_weaponType = weaponData->WeaponType;
	_damage = weaponData->Damage;
}

Armor::Armor(int32 templateId)
	: Item(ItemType_ITEM_ARMOR)
	, _defence(0)
{
	Init(templateId);
}

void Armor::Init(int32 templateId)
{
	shared_ptr<ItemData> itemData = nullptr;

	auto iter = DataManager::Items.find(templateId);
	if (iter != DataManager::Items.end())
	{
		itemData = iter->second;
	}

	if (itemData->ItemType != ItemType_ITEM_ARMOR)
	{
		CRASH();
		return;
	}

	shared_ptr<ArmorData> armorData = static_pointer_cast<ArmorData>(itemData);
	SetTemplateId(armorData->ID);
	SetCount(1);
	_defence = armorData->Defence;
}
