using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MonsterController : ObjectController
{
    Coroutine _coPatrol;
    Coroutine _coSearch;
    Vector3Int _destPos;

    GameObject _target;
    float _searchRange = 5.0f;

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

            // �ڷ�ƾ �ݺ�
            if (_coPatrol != null)
            {
                StopCoroutine(_coPatrol);
                _coPatrol = null;
            }

            // �ڷ�ƾ �ݺ�
            if (_coSearch != null)
            {
                StopCoroutine(_coSearch);
                _coSearch = null;
            }
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
        if (State == Define.ObjectState.Idle)
        {
            // ���������� �ٶ󺸴� ���� Idle
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

                // ������ �ִϸ��̼� ����
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

        // Patrol AI
        if(_coPatrol == null)
        {
            _coPatrol = StartCoroutine("CoPatrol");
        }

        // SearchAI
        if (_coSearch == null)
        {
            _coSearch = StartCoroutine("CoSearch");
        }
    }

    protected override void UpdateCoordinates()
    {
        Vector3Int targetPos = _destPos;
        if(_target != null)
        {
            targetPos = _target.GetComponent<ObjectController>().CellPos;
        }

        // TODO: A*
        List<Vector3Int> path = Manager.Map.FindPathBFS(CellPos, targetPos, true);
        // ���� �� ã�ų� Ÿ���� �ʹ� �־����� ����
        if(path.Count < 2 || (_target != null && path.Count > 10))
        {
            _target = null;
            State = Define.ObjectState.Idle;
            return;
        }

        Vector3Int nextPos = path[1];
        Vector3Int moveDir = nextPos - CellPos;
        if(moveDir.x > 0)
        {
            MoveDir = Define.MoveDir.Right;
        }
        else if(moveDir.x < 0)
        {
            MoveDir = Define.MoveDir.Left;
        }
        else if(moveDir.y > 0)
        {
            MoveDir = Define.MoveDir.Up;
        }
        else if (moveDir.y < 0)
        {
            MoveDir = Define.MoveDir.Down;
        }

        if (Manager.Map.CanMove(nextPos) && Manager.Object.Find(nextPos) == null)
        {
            CellPos = nextPos;

        }
        else
        {
            State = Define.ObjectState.Idle;
        }
    }

    public override void OnDamaged()
    {
        // ���� ���� ����Ʈ ������Ʈ
        GameObject effectOriginal = Resources.Load<GameObject>("Prefabs/Effect/MonsterDeathEffect");
        GameObject deathEffect = Object.Instantiate(effectOriginal);
        deathEffect.transform.position = transform.position;
        deathEffect.name = "DeathEffect";

        deathEffect.GetComponent<Animator>().Play("MonsterDeathEffectAnim");
        GameObject.Destroy(deathEffect, 0.5f);

        // ���� ����
        Manager.Object.Remove(Id);
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

            if (Manager.Map.CanMove(randomPos) && Manager.Object.Find(randomPos) == null)
            {
                _destPos = randomPos;
                State = Define.ObjectState.Moving;
                yield break;
            }
        }

        // �� �� ���� ��ǥ�� Idle ����
        State = Define.ObjectState.Idle;
    }

    IEnumerator CoSearch()
    {
        while (true)
        {
            yield return new WaitForSeconds(1);

            if(_target != null)
            {
                continue;
            }

            _target = Manager.Object.Find((gameObj) =>
            {
                // ���ٽ��� ���ǿ� ���� ������Ʈ ��ȯ
                PlayerController playerController = gameObj.GetComponent<PlayerController>();
                if(playerController == null)
                {
                    return false;
                }

                Vector3Int dir = playerController.CellPos - CellPos;
                if(dir.magnitude > _searchRange)
                {
                    return false;
                }

                return true;
            });
        }
    }
}
