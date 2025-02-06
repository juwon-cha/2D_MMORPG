using UnityEngine;

public class Manager : MonoBehaviour
{
    #region Singleton
    static Manager _instance;
    SceneManagerEx _scene = new SceneManagerEx();
    public static Manager Instance {  get { return _instance; } }
    public static SceneManagerEx Scene { get { return _instance._scene; } }
    #endregion
    void Start()
    {
        _instance = this;
        DontDestroyOnLoad(gameObject);
    }
}
