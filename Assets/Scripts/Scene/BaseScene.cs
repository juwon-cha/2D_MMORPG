using UnityEngine;
using Define;

public abstract class BaseScene : BaseMonobehaviour
{
    public SceneType SceneType { get; set; }
    public BaseUIController[] UIControllers;
    void Start()
    {
        Init();
    }
    protected abstract void Init();
    protected virtual void Clear()
    {
    }
}
