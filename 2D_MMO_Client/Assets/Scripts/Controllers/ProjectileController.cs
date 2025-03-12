using UnityEngine;

public class ProjectileController : ObjectController
{
    protected override void Init()
    {
        switch (MoveDir)
        {
            case Define.MoveDir.Up:
                transform.rotation = Quaternion.Euler(0, 0, 0);
                break;
            case Define.MoveDir.Down:
                transform.rotation = Quaternion.Euler(0, 0, -180);
                break;
            case Define.MoveDir.Left:
                transform.rotation = Quaternion.Euler(0, 0, 90);
                break;
            case Define.MoveDir.Right:
                transform.rotation = Quaternion.Euler(0, 0, -90);
                break;
        }

        State = Define.ObjectState.Moving;

        base.Init();
    }
}
