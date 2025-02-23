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

    protected override void UpdateIdle()
    {
        // �̵� ����
        if(MoveDir != Define.MoveDir.None)
        {
            State = Define.ObjectState.Moving;
            return;
        }
    }

    public void UseSkill(int skillId)
    {
        if (skillId == 1)
        {
            _coSkill = StartCoroutine("CoStartSwordAttack");
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
}
