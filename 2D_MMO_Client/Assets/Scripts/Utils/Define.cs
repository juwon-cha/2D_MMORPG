using UnityEngine;

namespace Define
{
    public enum SceneType
    {
        NONE,
        LOGIN,
        CHARSELECTION,
        INGAME,
    }

    public enum ObjectState
    {
        Idle,
        Moving,
        Skill,
        Dead,
    }

    public enum MoveDir
    {
        None,
        Up,
        Down,
        Left,
        Right,
    }
}
