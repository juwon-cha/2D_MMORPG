using UnityEngine;

public class MonsterController : ObjectController
{
    protected override void Init()
    {
        base.Init();
    }

    protected override void UpdateController()
    {
        base.UpdateController();
    }

    void InputDirection()
    {
        if (Input.GetKey(KeyCode.UpArrow))
        {
            MoveDir = MoveDir.Up;
        }
        else if (Input.GetKey(KeyCode.DownArrow))
        {
            MoveDir = MoveDir.Down;
        }
        else if (Input.GetKey(KeyCode.LeftArrow))
        {
            MoveDir = MoveDir.Left;
        }
        else if (Input.GetKey(KeyCode.RightArrow))
        {
            MoveDir = MoveDir.Right;
        }
        else
        {
            MoveDir = MoveDir.Idle;
        }
    }
}
