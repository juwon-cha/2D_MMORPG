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
}
