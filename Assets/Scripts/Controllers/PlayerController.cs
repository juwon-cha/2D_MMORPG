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
        // 이동 상태
        if(MoveDir != Define.MoveDir.Idle)
        {
            State = Define.ObjectState.Moving;
            return;
        }

        // 스킬 상태
        if (Input.GetKey(KeyCode.Space))
        {
            State = Define.ObjectState.Skill;
            _coSkill = StartCoroutine("CoStartAttack");
        }
    }

    // 키보드 입력 방향 설정
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
        // 피격 판정
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

        // 대기 시간
        yield return new WaitForSeconds(0.5f);
        State = Define.ObjectState.Idle;
        _coSkill = null;
    }
}
