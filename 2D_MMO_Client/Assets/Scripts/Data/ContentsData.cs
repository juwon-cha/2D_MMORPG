using Newtonsoft.Json;
using Newtonsoft.Json.Converters;
using System;
using System.Collections.Generic;

namespace Data
{
    #region Skill
    [Serializable]
    public class Skill
    {
        public int ID;
        public string Name;
        public float CoolTime;
        public int Damage;
        [JsonConverter(typeof(StringEnumConverter))] // string to enum parsing
        public SkillType skillType;
        public ProjectileInfo Projectile;
    }

    [Serializable]
    public class ProjectileInfo
    {
        public string Name;
        public float Speed;
        public int Range;
        public string Prefab;
    }

    [Serializable]
    public class SkillData : ILoader<int, Skill>
    {
        public List<Skill> skills = new List<Skill>();

        public Dictionary<int, Skill> MakeData()
        {
            Dictionary<int, Skill> dict = new Dictionary<int, Skill>();
            foreach (Skill skill in skills)
            {
                dict.Add(skill.ID, skill);
            }
            return dict;
        }
    }
    #endregion

    #region Item
    [Serializable]
    public class ItemData
    {
        public int ID;
        public string Name;
        [JsonConverter(typeof(StringEnumConverter))]
        public ItemType ItemType;
        public string IconPath;
    }

    [Serializable]
    public class WeaponData : ItemData
    {
        [JsonConverter(typeof(StringEnumConverter))]
        public WeaponType WeaponType;
        public int Damage;
    }

    [Serializable]
    public class ArmorData : ItemData
    {
        public int Defence;
    }

    [Serializable]
    public class ItemLoader : ILoader<int, ItemData>
    {
        public List<WeaponData> weapons = new List<WeaponData>();
        public List<ArmorData> armors = new List<ArmorData>();

        public Dictionary<int, ItemData> MakeData()
        {
            Dictionary<int, ItemData> dict = new Dictionary<int, ItemData>();
            foreach (ItemData item in weapons)
            {
                item.ItemType = ItemType.ITEM_WEAPON;
                dict.Add(item.ID, item);
            }
            foreach (ItemData item in armors)
            {
                item.ItemType = ItemType.ITEM_ARMOR;
                dict.Add(item.ID, item);
            }
            return dict;
        }
    }
    #endregion
}
