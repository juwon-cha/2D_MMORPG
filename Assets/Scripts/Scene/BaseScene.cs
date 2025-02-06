using UnityEngine;
using Define;

public class BaseScene : BaseMonobehaviour
{
    public SceneType SceneType { get; set; }
    public BaseUIController[] UIControllers;
    void Start()
    {
        Init();
    }
    protected virtual void Init()
    {
        Manager.Scene.CurScene = this;
    }
    protected virtual void Clear()
    {
    }
}
