#include "pch.h"
#include "ItemManager.h"
#include "Item.h"

void ItemManager::AddItem(shared_ptr<Item> item)
{
	_items.insert(make_pair(item->GetObjectId(), item));
}

shared_ptr<Item> ItemManager::GetItem(int32 itemId)
{
	shared_ptr<Item> item = nullptr;

	auto iter = _items.find(itemId);
	if (iter != _items.end())
	{
		item = iter->second;
	}

	return item;
}

shared_ptr<Item> ItemManager::FindItem(function<bool(shared_ptr<Item>)> condition)
{
	for (const auto& pair : _items)
	{
		if (condition(pair.second))
		{
			return pair.second;
		}
	}

	return nullptr;
}
