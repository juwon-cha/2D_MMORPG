using NUnit.Framework.Interfaces;
using UnityEngine;
using Data;

public class Item
{
    public int ItemObjectId { get; set; }
    public int TemplateId { get; set; }
    public int Count { get; set; }
    public int Slot { get; set; }

    public ItemType ItemType { get; private set; }

    public Item(ItemType itemType)
    {
        ItemType = itemType;
    }

    public static Item MakeItem(ItemInfo itemInfo)
    {
        Item item = null;

        Data.ItemData itemData = null;
        DataManager.ItemDict.TryGetValue(itemInfo.TemplateId, out itemData);

        if (itemData == null)
            return null;

        switch (itemData.ItemType)
        {
            case ItemType.ITEM_WEAPON:
                item = new Weapon(itemInfo.TemplateId);
                break;
            case ItemType.ITEM_ARMOR:
                item = new Armor(itemInfo.TemplateId);
                break;
            case ItemType.ITEM_NONE:
                break;
        }

        if (item != null)
        {
            item.ItemObjectId = itemInfo.ItemId;
            item.TemplateId = itemInfo.TemplateId;
            item.Count = itemInfo.Count;
            item.Slot = itemInfo.Slot;
        }

        return item;
    }
}

public class Weapon : Item
{
    public WeaponType WeaponType { get; private set; }
    public int Damage { get; private set; }

    public Weapon(int templateId) : base(ItemType.ITEM_WEAPON)
    {
        Init(templateId);
    }

    void Init(int templateId)
    {
        ItemData itemData = null;
        DataManager.ItemDict.TryGetValue(templateId, out itemData);
        if (itemData.ItemType != ItemType.ITEM_WEAPON)
            return;

        WeaponData data = (WeaponData)itemData;
        {
            TemplateId = data.ID;
            Count = 1;
            WeaponType = data.WeaponType;
            Damage = data.Damage;
        }
    }
}

public class Armor : Item
{
    public int Defence { get; private set; }

    public Armor(int templateId) : base(ItemType.ITEM_ARMOR)
    {
        Init(templateId);
    }

    void Init(int templateId)
    {
        ItemData itemData = null;
        DataManager.ItemDict.TryGetValue(templateId, out itemData);
        if (itemData.ItemType != ItemType.ITEM_ARMOR)
            return;

        ArmorData data = (ArmorData)itemData;
        {
            TemplateId = data.ID;
            Count = 1;
            Defence = data.Defence;
        }
    }
}
