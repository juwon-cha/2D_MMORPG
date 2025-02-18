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

    IEnumerator CoStartAttack()
    {
        // �ǰ� ����
        GameObject gameObj = Manager.Object.Find(GetFacingCellPostition());
        if(gameObj != null)
        {
            Debug.Log(gameObj.name);
            Debug.Log(gameObj.transform.position);

            ObjectController objController = gameObj.GetComponent<ObjectController>();
            if (objController != null)
            {
                objController.OnDamaged();
            }
        }

        // ��� �ð�
        yield return new WaitForSeconds(0.5f);
        State = Define.ObjectState.Idle;
        _coSkill = null;
    }
}
