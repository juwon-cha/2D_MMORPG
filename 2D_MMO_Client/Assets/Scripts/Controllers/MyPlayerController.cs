using Google.FlatBuffers;
using UnityEngine;
using System.Collections;
using Data;


#if UNITY_EDITOR
using UnityEditor;
#endif

public class MyPlayerController : PlayerController
{
    bool _moveKeyPressed = false;

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
        // 이동 상태
        if (_moveKeyPressed)
        {
            State = Define.ObjectState.Moving;
            return;
        }

        // 스킬 상태
        if (_coSkillCooltime == null && Input.GetKey(KeyCode.Space))
        {
            int skillId = 2; // TEMP

            Data.Skill skillData = null;
            if (DataManager.SkillData.TryGetValue(skillId, out skillData) == false)
            {
                return;
            }

            FlatBufferBuilder builder = new FlatBufferBuilder(1024);

            var skillInfo = SkillInfo.CreateSkillInfo(builder, skillId);
            var skill = C_SKILL.CreateC_SKILL(builder, skillInfo);
            var skillPkt = Manager.Packet.CreatePacket(skill, builder, PacketType.C_SKILL);
            Manager.Network.Send(skillPkt);

            _coSkillCooltime = StartCoroutine("CoInputCooltime", skillData.CoolTime);
        }
    }

    Coroutine _coSkillCooltime;
    IEnumerator CoInputCooltime(float time)
    {
        yield return new WaitForSeconds(time);
        _coSkillCooltime = null;
    }

    // 키보드 입력 방향 설정
    void GetDirection()
    {
        _moveKeyPressed = true;

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
        else if (Input.GetKey(KeyCode.Escape))
        {
            ExitPlayMode();
        }
        else
        {
            _moveKeyPressed = false;
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
        if (_moveKeyPressed == false)
        {
            State = Define.ObjectState.Idle;
            CheckUpdatedFlag();
            return;
        }

        // Idle 상태가 아니면 Moving 상태
        Vector3Int destPos = CellPos;

        switch (MoveDir)
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
            // 객체가 없으면 이동 가능
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
