using System.Collections.Generic;
using System;
using UnityEngine;

public class InventoryManager
{
    public Dictionary<int, Item> Items { get; } = new Dictionary<int, Item>();

    public void Add(Item item)
    {
        Items.Add(item.ItemObjectId, item);
    }

    public Item Get(int itemObjectId)
    {
        Item item = null;
        Items.TryGetValue(itemObjectId, out item);
        return item;
    }

    public Item Find(Func<Item, bool> condition)
    {
        foreach (Item item in Items.Values)
        {
            if (condition.Invoke(item))
                return item;
        }

        return null;
    }

    public void Clear()
    {
        Items.Clear();
    }
}
