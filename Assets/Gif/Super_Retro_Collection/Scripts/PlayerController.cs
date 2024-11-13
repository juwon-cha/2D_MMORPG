using NUnit.Framework.Constraints;
using System.Collections;
using UnityEditor.Experimental.GraphView;
using UnityEditor.Rendering;
using UnityEngine;

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
            case ObjectState.Idle:
                InputDirection();
                break;

            case ObjectState.Moving:
                InputDirection();
                break;

            default:
                break;
        }
        
        base.UpdateController();
    }

    private void LateUpdate()
    {
        Camera.main.transform.position = new Vector3(transform.position.x, transform.position.y, -10);
    }

    protected override void UpdateIdle()
    {
        // �̵� ����
        if(MoveDir != MoveDir.Idle)
        {
            State = ObjectState.Moving;
            return;
        }

        // ��ų ����
        if (Input.GetKey(KeyCode.Space))
        {
            State = ObjectState.Skill;
            _coSkill = StartCoroutine("CoStartAttack");
        }
    }

    // Ű���� �Է� ���� ����
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

    IEnumerator CoStartAttack()
    {
        // �ǰ� ����
        GameObject gameObj = ObjManager.Find(GetFacingCellPostition());
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
        State = ObjectState.Idle;
        _coSkill = null;
    }
}
