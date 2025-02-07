using UnityEngine;
using Define;

public class BaseScene : BaseMonobehaviour
{
    public SceneType SceneType { get; set; } = Define.SceneType.NONE;
    public BaseUIController[] UIControllers;

    void Awake()
    {
        Init();
    }

    protected virtual void Init()
    {
        
    }

    public virtual void Clear()
    {
    }
}
