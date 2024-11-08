using UnityEngine;

public class MonsterController : ObjectController
{
    protected override void Init()
    {
        base.Init();
        State = ObjectState.Idle;
        MoveDir = MoveDir.Idle;
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

    public override void OnDamaged()
    {
        // 몬스터 죽음 이펙트 오브젝트
        GameObject effectOriginal = Resources.Load<GameObject>("Prefabs/Effects/MonsterDeathEffect");
        GameObject deathEffect = Object.Instantiate(effectOriginal);
        deathEffect.transform.position = transform.position;
        deathEffect.name = "DeathEffect";

        deathEffect.GetComponent<Animator>().Play("MonsterDeathEffectAnim");
        GameObject.Destroy(deathEffect, 0.5f);

        // 몬스터 삭제
        ObjManager.Remove(gameObject);
        GameObject.Destroy(gameObject);
    }
}
