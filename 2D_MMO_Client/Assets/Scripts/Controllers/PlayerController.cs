using System.Collections;
using UnityEngine;

public class PlayerController : ObjectController
{
    protected Coroutine _coSkill;

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
            // ���������� �ٶ󺸴� ���� Idle
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

                // ������ �ִϸ��̼� ����
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
            // ���������� �ٶ� ���� �������� ��ų ����
            switch (MoveDir)
            {
                case Define.MoveDir.Up:
                    _animator.Play("ATTACK_UP");
                    _sprite.flipX = false;
                    break;

                case Define.MoveDir.Down:
                    _animator.Play("ATTACK_DOWN");
                    _sprite.flipX = false;
                    break;

                // ������ �ִϸ��̼� ����
                case Define.MoveDir.Left:
                    _animator.Play("ATTACK_RIGHT");
                    _sprite.flipX = true;
                    break;

                case Define.MoveDir.Right:
                    _animator.Play("ATTACK_RIGHT");
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
        else if(skillId == 2)
        {
            // TODO: projectile skill
        }
    }

    IEnumerator CoStartSwordAttack()
    {
        // ��� �ð�
        State = Define.ObjectState.Skill;
        yield return new WaitForSeconds(0.5f);
        State = Define.ObjectState.Idle;
        _coSkill = null;
        CheckUpdatedFlag(); // �÷��̾��� ���¸� Idle�� �����ϰ� ������ ����
    }

    protected virtual void CheckUpdatedFlag()
    {
    }

    protected override void UpdateIdle()
    {
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
