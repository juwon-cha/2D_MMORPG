#pragma once

class Item;

class ItemManager
{
public:
	ItemManager() = default;
	~ItemManager() = default;

	void AddItem(shared_ptr<Item> item);
	shared_ptr<Item> GetItem(int32 itemId);
	shared_ptr<Item> FindItem(function<bool(shared_ptr<Item>)> condition);

	map<int32, shared_ptr<Item>> GetItemList() const { return _items; }

private:
	map<int32, shared_ptr<Item>> _items;
};
