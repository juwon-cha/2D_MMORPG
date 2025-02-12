using UnityEngine;
using Define;
using UnityEngine.SceneManagement;

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
