using Google.FlatBuffers;
using UnityEngine;
using System.Collections;

#if UNITY_EDITOR
using UnityEditor;
#endif

public class MyPlayerController : PlayerController
{
    protected override void Init()
    {
        base.Init();
    }

    protected override void UpdateController()
    {
        switch (State)
        {
            case Define.ObjectState.Idle:
                GetDirection();
                break;

            case Define.ObjectState.Moving:
                GetDirection();
                break;
        }

        base.UpdateController();
    }

    protected override void UpdateIdle()
    {
        // �̵� ����
        if (MoveDir != Define.MoveDir.None)
        {
            State = Define.ObjectState.Moving;
            return;
        }

        // ��ų ����
        if (_coSkillCooltime == null && Input.GetKey(KeyCode.Space))
        {
            Debug.Log("Skill!");

            FlatBufferBuilder builder = new FlatBufferBuilder(1024);
            var skillInfo = SkillInfo.CreateSkillInfo(builder, 1);
            var skill = C_SKILL.CreateC_SKILL(builder, skillInfo);
            var skillPkt = Manager.Packet.CreatePacket(skill, builder, PacketType.C_SKILL);
            Manager.Network.Send(skillPkt);

            _coSkillCooltime = StartCoroutine("CoInputCooltime", 0.3f);
        }
    }

    Coroutine _coSkillCooltime;
    IEnumerator CoInputCooltime(float time)
    {
        yield return new WaitForSeconds(time);
        _coSkillCooltime = null;
    }

    // Ű���� �Է� ���� ����
    void GetDirection()
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
        else if(Input.GetKey(KeyCode.Escape))
        {
            ExitPlayMode();
        }
        else
        {
            MoveDir = Define.MoveDir.None;
        }
    }

    // This script runs only in the Unity editor
    private void ExitPlayMode()
    {
#if UNITY_EDITOR
        EditorApplication.ExitPlaymode();
        Manager.Network.Disconnect();
#endif
    }

    protected override void UpdateCoordinates()
    {
        if (_dir == Define.MoveDir.None)
        {
            State = Define.ObjectState.Idle;
            CheckUpdatedFlag();
            return;
        }

        // Idle ���°� �ƴϸ� Moving ����
        Vector3Int destPos = CellPos;

        switch (_dir)
        {
            case Define.MoveDir.Up:
                destPos += Vector3Int.up;
                break;

            case Define.MoveDir.Down:
                destPos += Vector3Int.down;
                break;

            case Define.MoveDir.Left:
                destPos += Vector3Int.left;
                break;

            case Define.MoveDir.Right:
                destPos += Vector3Int.right;
                break;

            default:
                break;
        }

        if (Manager.Map.CanMove(destPos))
        {
            // ��ü�� ������ �̵� ����
            if (Manager.Object.Find(destPos) == null)
            {
                CellPos = destPos;
            }
        }

        CheckUpdatedFlag();
    }

    protected override void CheckUpdatedFlag()
    {
        if (_updated)
        {
            FlatBufferBuilder builder = new FlatBufferBuilder(1024);

            var posInfo = PositionInfo.CreatePositionInfo(builder, (ObjectState)State, (MoveDir)MoveDir, CellPos.x, CellPos.y);
            var move = C_MOVE.CreateC_MOVE(builder, posInfo);
            var movePkt = Manager.Packet.CreatePacket(move, builder, PacketType.C_MOVE);
            Manager.Network.Send(movePkt);
            _updated = false;
        }
    }
}
