using NUnit.Framework.Constraints;
using System.Collections;
using UnityEditor.Experimental.GraphView;
using UnityEditor.Rendering;
using UnityEngine;
using static UnityEditor.PlayerSettings;

public class PlayerController : ObjectController
{
    Coroutine _coSkill;

    protected override void Init()
    {
        base.Init();
    }

    protected override void UpdateController()
    {
        switch(State)
        {
            case Define.ObjectState.Idle:
                InputDirection();
                break;

            case Define.ObjectState.Moving:
                InputDirection();
                break;

            default:
                break;
        }
        
        base.UpdateController();
    }

    private void LateUpdate()
    {
        //Camera.main.transform.position = new Vector3(transform.position.x, transform.position.y, -10);
    }

    protected override void UpdateIdle()
    {
        // �̵� ����
        if(MoveDir != Define.MoveDir.Idle)
        {
            State = Define.ObjectState.Moving;
            return;
        }

        // ��ų ����
        if (Input.GetKey(KeyCode.Space))
        {
            State = Define.ObjectState.Skill;
            _coSkill = StartCoroutine("CoStartAttack");
        }
    }

    // Ű���� �Է� ���� ����
    void InputDirection()
    {
        if (Input.GetKey(KeyCode.UpArrow))
        {
            MoveDir = Define.MoveDir.Up;
        }
        else if (Input.GetKey(KeyCode.DownArrow))
        {
            MoveDir = Define.MoveDir.Down;
        }
        else if (Input.GetKey(KeyCode.LeftArrow))
        {
            MoveDir = Define.MoveDir.Left;
        }
        else if (Input.GetKey(KeyCode.RightArrow))
        {
            MoveDir = Define.MoveDir.Right;
        }
        else
        {
            MoveDir = Define.MoveDir.Idle;
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
