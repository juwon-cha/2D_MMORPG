using System.Collections;
using UnityEngine;

public class PlayerController : ObjectController
{
    protected Coroutine _coSkill;
    protected bool _rangedSkill = false;

    protected override void Init()
    {
        base.Init();
        AddHpBar();
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
                    _animator.Play("IDLE_UP");
                    _sprite.flipX = false;
                    break;

                case Define.MoveDir.Down:
                    _animator.Play("IDLE_DOWN");
                    _sprite.flipX = false;
                    break;

                case Define.MoveDir.Left:
                    _animator.Play("IDLE_RIGHT");
                    _sprite.flipX = true;
                    break;

                case Define.MoveDir.Right:
                    _animator.Play("IDLE_RIGHT");
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
                    _animator.Play("WALK_UP");
                    _sprite.flipX = false;
                    break;

                case Define.MoveDir.Down:
                    _animator.Play("WALK_DOWN");
                    _sprite.flipX = false;
                    break;

                // 오른쪽 애니메이션 반전
                case Define.MoveDir.Left:
                    _animator.Play("WALK_RIGHT");
                    _sprite.flipX = true;
                    break;

                case Define.MoveDir.Right:
                    _animator.Play("WALK_RIGHT");
                    _sprite.flipX = false;
                    break;

                default:
                    break;
            }
        }
        else if (State == Define.ObjectState.Skill)
        {
            // 마지막으로 바라본 방향 기준으로 스킬 시전
            switch (MoveDir)
            {
                case Define.MoveDir.Up:
                    _animator.Play(_rangedSkill ? "BOW_UP" : "ATTACK_UP");
                    _sprite.flipX = false;
                    break;

                case Define.MoveDir.Down:
                    _animator.Play(_rangedSkill ? "BOW_DOWN" : "ATTACK_DOWN");
                    _sprite.flipX = false;
                    break;

                // 오른쪽 애니메이션 반전
                case Define.MoveDir.Left:
                    _animator.Play(_rangedSkill ? "BOW_RIGHT" : "ATTACK_RIGHT");
                    _sprite.flipX = true;
                    break;

                case Define.MoveDir.Right:
                    _animator.Play(_rangedSkill ? "BOW_RIGHT" : "ATTACK_RIGHT");
                    _sprite.flipX = false;
                    break;

                default:
                    break;
            }
        }
        else if (State == Define.ObjectState.Dead)
        {
            _animator.Play("PLAYER_DEATH");
        }
    }

    protected override void UpdateController()
    {
        base.UpdateController();
    }

    public override void UseSkill(int skillId)
    {
        if (skillId == 1)
        {
            _coSkill = StartCoroutine("CoStartSwordAttack");
        }
        else if (skillId == 2)
        {
            _coSkill = StartCoroutine("CoStartBowAttack");
        }
    }

    IEnumerator CoStartSwordAttack()
    {
        // 대기 시간
        _rangedSkill = false;
        State = Define.ObjectState.Skill;
        yield return new WaitForSeconds(0.5f);
        State = Define.ObjectState.Idle;
        _coSkill = null;

        // 스킬 사용 후플레이어의 상태를 Idle로 변경하고 서버로 전송
        CheckUpdatedFlag();
    }

    IEnumerator CoStartBowAttack()
    {
        // 대기 시간
        _rangedSkill = true;
        State = Define.ObjectState.Skill;
        yield return new WaitForSeconds(0.3f);
        State = Define.ObjectState.Idle;
        _coSkill = null;

        CheckUpdatedFlag();
    }

    protected virtual void CheckUpdatedFlag()
    {

    }

    public override void OnDead()
    {
        base.OnDead();

    }
}
