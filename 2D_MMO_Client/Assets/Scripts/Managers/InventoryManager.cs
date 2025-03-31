using System.Collections.Generic;
using System;
using UnityEngine;
using System.Globalization;

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

    public Item Find(Func<string, bool> condition)
    {
        foreach (Item item in Items.Values)
        {
            Weapon wepon = item as Weapon;
            string typeName = wepon.WeaponType.ToString();
            string[] splitTypeName = typeName.Split("_", System.StringSplitOptions.RemoveEmptyEntries);
            string weapon = splitTypeName[1].ToLower();
            TextInfo ti = new CultureInfo("en-US", false).TextInfo;

            if (condition.Invoke(ti.ToTitleCase(weapon)))
                return item;
        }

        return null;
    }

    public void Clear()
    {
        Items.Clear();
    }
}
