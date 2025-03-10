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
        AddHpBar();

        State = Define.ObjectState.Idle;
        MoveDir = Define.MoveDir.Down;
    }

    protected override void UpdateAnim()
    {
        if (_animator == null && _sprite == null)
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
                    _sprite.flipX = false;
                    break;

                case Define.MoveDir.Down:
                    _animator.Play("SLIME01_IDLE");
                    _sprite.flipX = false;
                    break;

                case Define.MoveDir.Left:
                    _animator.Play("SLIME01_IDLE");
                    _sprite.flipX = true;
                    break;

                case Define.MoveDir.Right:
                    _animator.Play("SLIME01_IDLE");
                    _sprite.flipX = false;
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
                    _sprite.flipX = false;
                    break;

                case Define.MoveDir.Down:
                    _animator.Play("SLIME01_IDLE");
                    _sprite.flipX = false;
                    break;

                // 오른쪽 애니메이션 반전
                case Define.MoveDir.Left:
                    _animator.Play("SLIME01_WALK_RIGHT");
                    _sprite.flipX = true;
                    break;

                case Define.MoveDir.Right:
                    _animator.Play("SLIME01_WALK_RIGHT");
                    _sprite.flipX = false;
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
            GameObject effectOriginal = Resources.Load<GameObject>("Prefabs/Effect/MonsterDeathEffect");
            GameObject deathEffect = Object.Instantiate(effectOriginal);
            deathEffect.transform.position = transform.position;
            deathEffect.name = "DeathEffect";

            deathEffect.GetComponent<Animator>().Play("MonsterDeathEffectAnim");
            Destroy(deathEffect, 0.5f);
        }
    }

    protected override void UpdateIdle()
    {
    }

    public override void UseSkill(int skillId)
    {
        if (skillId == 1)
        {
            State = Define.ObjectState.Skill;
        }
        else if (skillId == 2)
        {
            // TODO: projectile skill
        }
    }

    protected override void UpdateCoordinates()
    {
    }

    protected override void UpdateSkill()
    {
    }

    protected override void UpdateDead()
    {
    }

    public override void OnDead()
    {
        base.OnDead();

    }
}
