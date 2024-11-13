using System.Collections;
using UnityEngine;

public class MonsterController : ObjectController
{
    Coroutine _coPatrol;
    Vector3Int _destPos;
    public override ObjectState State
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

            // �ڷ�ƾ �ݺ�
            if (_coPatrol != null)
            {
                StopCoroutine(_coPatrol);
                _coPatrol = null;
            }
        }
    }

    protected override void Init()
    {
        base.Init();
        State = ObjectState.Idle;
        MoveDir = MoveDir.Idle;
    }

    protected override void UpdateAnim()
    {
        if (State == ObjectState.Idle)
        {
            // ���������� �ٶ󺸴� ���� Idle
            switch (_lastFacingDir)
            {
                case MoveDir.Up:
                    _animator.Play("SLIME01_IDLE");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;

                case MoveDir.Down:
                    _animator.Play("SLIME01_IDLE");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;

                case MoveDir.Left:
                    _animator.Play("SLIME01_IDLE");
                    transform.localScale = new Vector3(-1.0f, 1.0f, 1.0f);
                    break;

                case MoveDir.Right:
                    _animator.Play("SLIME01_IDLE");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;

                default:
                    break;
            }
        }
        else if (State == ObjectState.Moving)
        {
            switch (_dir)
            {
                case MoveDir.Up:
                    _animator.Play("SLIME01_IDLE");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;

                case MoveDir.Down:
                    _animator.Play("SLIME01_IDLE");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;

                // ������ �ִϸ��̼� ����
                case MoveDir.Left:
                    _animator.Play("SLIME01_WALK_RIGHT");
                    transform.localScale = new Vector3(-1.0f, 1.0f, 1.0f);
                    break;

                case MoveDir.Right:
                    _animator.Play("SLIME01_WALK_RIGHT");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;

                default:
                    break;
            }
        }
        else if (State == ObjectState.Skill)
        {
            // TODO: Skill anim
        }
        else if (State == ObjectState.Dead)
        {
            // TODO: Death anim
        }
    }

    protected override void UpdateIdle()
    {
        base.UpdateIdle();

        // Patrol AI
        if(_coPatrol == null)
        {
            _coPatrol = StartCoroutine("CoPatrol");
        }
    }

    protected override void UpdateCoordinates()
    {
        // TODO: A*
        Vector3Int moveDir = _destPos - CellPos;
        if(moveDir.x > 0)
        {
            MoveDir = MoveDir.Right;
        }
        else if(moveDir.x < 0)
        {
            MoveDir = MoveDir.Left;
        }
        else if(moveDir.y > 0)
        {
            MoveDir = MoveDir.Up;
        }
        else if (moveDir.y < 0)
        {
            MoveDir = MoveDir.Down;
        }
        else
        {
            MoveDir = MoveDir.Idle;
        }

        Vector3Int destPos = CellPos;

        switch (_dir)
        {
            case MoveDir.Up:
                destPos += Vector3Int.up;
                break;

            case MoveDir.Down:
                destPos += Vector3Int.down;
                break;

            case MoveDir.Left:
                destPos += Vector3Int.left;
                break;

            case MoveDir.Right:
                destPos += Vector3Int.right;
                break;

            default:
                break;
        }

        if (GameScene.CanMove(destPos) && ObjManager.Find(destPos) == null)
        {
            CellPos = destPos;

        }
        else
        {
            State = ObjectState.Idle;
        }
    }

    public override void OnDamaged()
    {
        // ���� ���� ����Ʈ ������Ʈ
        GameObject effectOriginal = Resources.Load<GameObject>("Prefabs/Effects/MonsterDeathEffect");
        GameObject deathEffect = Object.Instantiate(effectOriginal);
        deathEffect.transform.position = transform.position;
        deathEffect.name = "DeathEffect";

        deathEffect.GetComponent<Animator>().Play("MonsterDeathEffectAnim");
        GameObject.Destroy(deathEffect, 0.5f);

        // ���� ����
        ObjManager.Remove(gameObject);
        GameObject.Destroy(gameObject);
    }

    IEnumerator CoPatrol()
    {
        int wait = Random.Range(1, 4);
        yield return new WaitForSeconds(wait);

        // ���� ��ǥ�� �� �� �ִ��� ������ �Ǻ�
        for(int i = 0; i < 7; ++i)
        {
            int x = Random.Range(-5, 5);
            int y = Random.Range(-5, 5);
            Vector3Int randomPos = CellPos + new Vector3Int(x, y, 0);

            if (GameScene.CanMove(randomPos) && ObjManager.Find(randomPos) == null)
            {
                _destPos = randomPos;
                State = ObjectState.Moving;
                yield break;
            }
        }

        // �� �� ���� ��ǥ�� Idle ����
        State = ObjectState.Idle;
    }
}
