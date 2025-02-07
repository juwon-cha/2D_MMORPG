using UnityEngine;

public class Manager : MonoBehaviour
{
    #region Singleton
    static Manager _instance;
    public static Manager Instance {  get { Init(); return _instance; } }
    #endregion

    #region Contents
    MapManager _map = new MapManager();
    ObjectManager _object = new ObjectManager();

    public static MapManager Map { get { return Instance._map; } }
    public static ObjectManager Object { get { return Instance._object; } }
    #endregion

    SceneManagerEx _scene = new SceneManagerEx();
    public static SceneManagerEx Scene { get { return Instance._scene; } }

    void Start()
    {
        Init();
    }

    static void Init()
    {
        if (_instance == null)
        {
            GameObject go = GameObject.Find("Manager");
            if (go == null)
            {
                go = new GameObject { name = "Manager" };
                go.AddComponent<Manager>();
            }

            DontDestroyOnLoad(go);
            _instance = go.GetComponent<Manager>();
        }
    }

    public static void Clear()
    {
        Scene.Clear();
    }
}
