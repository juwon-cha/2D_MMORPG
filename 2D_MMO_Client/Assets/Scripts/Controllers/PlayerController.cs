using System.Collections;
using UnityEngine;

public class PlayerController : ObjectController
{
    protected Coroutine _coSkill;

    protected override void Init()
    {
        base.Init();
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
    }

    IEnumerator CoStartSwordAttack()
    {
        // 대기 시간
        State = Define.ObjectState.Skill;
        yield return new WaitForSeconds(0.5f);
        State = Define.ObjectState.Idle;
        _coSkill = null;
        CheckUpdatedFlag(); // 플레이어의 상태를 Idle로 변경하고 서버로 전송
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

    public override void OnDamaged()
    {
    }
}
