using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MonsterController : ObjectController
{
    public override Define.ObjectState State
    {
        get
        {
            return _state;
        }
        set
        {
            if (_state == value)
            {
                return;
            }

            base.State = value;
        }
    }

    protected override void Init()
    {
        base.Init();
        State = Define.ObjectState.Idle;
        MoveDir = Define.MoveDir.Down;
    }

    protected override void UpdateAnim()
    {
        if(_animator == null)
        {
            return;
        }

        if (State == Define.ObjectState.Idle)
        {
            // 마지막으로 바라보는 방향 Idle
            switch (MoveDir)
            {
                case Define.MoveDir.Up:
                    _animator.Play("SLIME01_IDLE");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;

                case Define.MoveDir.Down:
                    _animator.Play("SLIME01_IDLE");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;

                case Define.MoveDir.Left:
                    _animator.Play("SLIME01_IDLE");
                    transform.localScale = new Vector3(-1.0f, 1.0f, 1.0f);
                    break;

                case Define.MoveDir.Right:
                    _animator.Play("SLIME01_IDLE");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;

                default:
                    break;
            }
        }
        else if (State == Define.ObjectState.Moving)
        {
            switch (MoveDir)
            {
                case Define.MoveDir.Up:
                    _animator.Play("SLIME01_IDLE");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;

                case Define.MoveDir.Down:
                    _animator.Play("SLIME01_IDLE");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;

                // 오른쪽 애니메이션 반전
                case Define.MoveDir.Left:
                    _animator.Play("SLIME01_WALK_RIGHT");
                    transform.localScale = new Vector3(-1.0f, 1.0f, 1.0f);
                    break;

                case Define.MoveDir.Right:
                    _animator.Play("SLIME01_WALK_RIGHT");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;

                default:
                    break;
            }
        }
        else if (State == Define.ObjectState.Skill)
        {
            // TODO: Skill anim
        }
        else if (State == Define.ObjectState.Dead)
        {
            // TODO: Death anim
        }
    }

    protected override void UpdateIdle()
    {
        base.UpdateIdle();

    }

    public override void OnDamaged()
    {

    }
}
