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
}
