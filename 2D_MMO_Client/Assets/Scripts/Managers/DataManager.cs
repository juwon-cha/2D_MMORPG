using Newtonsoft.Json;
using System.Collections.Generic;
using System.IO;

public interface ILoader<Key, Value>
{
    Dictionary<Key, Value> MakeData();
}

public class DataManager
{
    public static Dictionary<int, Data.Skill> SkillData { get; private set; } = new Dictionary<int, Data.Skill>();

    public static void LoadData()
    {
        SkillData = LoadJson<Data.SkillData, int, Data.Skill>("SkillData").MakeData();
    }

    static Loader LoadJson<Loader, Key, Value>(string path) where Loader : ILoader<Key, Value>
    {
        string text = File.ReadAllText($"../Common/Data/ContentsData/{path}.json");
        return Newtonsoft.Json.JsonConvert.DeserializeObject<Loader>(text);
    }
}
