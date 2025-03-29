using Google.FlatBuffers;
using UnityEngine;
using System.Collections;
using Data;
using UnityEngine.UIElements;
using System.Net.Sockets;

#if UNITY_EDITOR
using UnityEditor;
#endif

public class MyPlayerController : PlayerController
{
    bool _moveKeyPressed = false;
    bool _changeMap = false;

    protected override void Init()
    {
        base.Init();
    }

    protected override void UpdateController()
    {
        GetUIKeyInput();

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
            Data.Skill skillData = null;
            if (DataManager.SkillDict.TryGetValue(SkillId, out skillData) == false)
            {
                return;
            }

            FlatBufferBuilder builder = new FlatBufferBuilder(1024);

            var skillInfo = SkillInfo.CreateSkillInfo(builder, SkillId);
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

    void GetUIKeyInput()
    {
        if (Input.GetKeyDown(KeyCode.I))
        {
            InventoryController inven = Manager.UI.InvenUI;

            if (inven.gameObject.activeSelf)
            {
                var uiDoc = inven.gameObject.GetComponent<UIDocument>();
                // 게임씬 시작 후 처음 인벤토리를 열었을 때 가려져 있는 UI 활성화를 위함
                if (uiDoc.rootVisualElement.style.visibility == Visibility.Hidden)
                {
                    uiDoc.rootVisualElement.style.visibility = Visibility.Visible;
                    inven.gameObject.SetActive(true);
                    inven.RefreshUI();
                }
                else
                {
                    // 게임 시작 후 처음 인벤토리 창을 열었을 때를 제외하고
                    // 인벤토리 게임오브젝트 활성화/비활성화 함
                    inven.gameObject.SetActive(false);
                }
            }
            else
            {
                inven.gameObject.SetActive(true);
                inven.RefreshUI();
            }
        }
        else if (Input.GetKey(KeyCode.Escape))
        {
            ExitPlayMode();
        }
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

            ChangeMap();
        }
    }

    void ChangeMap()
    {
        GameScene curScene = Manager.Scene.CurScene as GameScene;
        if (curScene.MapId == 1)
        {
            // 현재 플에이어의 좌표가 맵을 전환해야하는 좌표라면
            // 1 -> 5
            if ((CellPos.x == -18 || CellPos.x == -17 || CellPos.x == -16) && CellPos.y == 15)
            {
                _changeMap = true;
            }
            // 1 -> 4
            else if (CellPos.x == 18 && (CellPos.y == 4 || CellPos.y == 5 || CellPos.y == 6))
            {
                _changeMap = true;
            }
            // 1 -> 2
            else if ((CellPos.x == -3 || CellPos.x == -2 || CellPos.x == -1) && CellPos.y == -17)
            {
                _changeMap = true;
            }
            else
            {
                _changeMap = false;
                return;
            }
        }
        else if (curScene.MapId == 2)
        {
            // 2 -> 1
            if ((CellPos.x == -3 || CellPos.x == -2 || CellPos.x == -1) && CellPos.y == 24)
            {
                _changeMap = true;
            }
            // 2 -> 3
            else if (CellPos.x == 29 && (CellPos.y == 18 || CellPos.y == 19 || CellPos.y == 20))
            {
                _changeMap = true;
            }
            else
            {
                _changeMap = false;
                return;
            }
        }
        else if (curScene.MapId == 3)
        {
            // 3 -> 2
            if (CellPos.x == -32 && (CellPos.y == 18 || CellPos.y == 19 || CellPos.y == 20))
            {
                _changeMap = true;
            }
            // 3 -> 4
            else if ((CellPos.x == -9 || CellPos.x == -10 || CellPos.x == -11) && CellPos.y == 24)
            {
                _changeMap = true;
            }
            else
            {
                _changeMap = false;
                return;
            }
        }
        else if (curScene.MapId == 4)
        {
            // 4 -> 1
            if (CellPos.x == -32 && (CellPos.y == 11 || CellPos.y == 12 || CellPos.y == 13))
            {
                _changeMap = true;
            }
            // 4 -> 3
            else if ((CellPos.x == -9 || CellPos.x == -10 || CellPos.x == -11) && CellPos.y == -10)
            {
                _changeMap = true;
            }
            else
            {
                _changeMap = false;
                return;
            }
        }
        else if (curScene.MapId == 5)
        {
            // 5 -> 1
            if ((CellPos.x == -2 || CellPos.x == -3 || CellPos.x == -4) && CellPos.y == -21)
            {
                _changeMap = true;
            }
            else
            {
                _changeMap = false;
                return;
            }
        }

        if (_changeMap)
        {
            // 플레이어 좌표와 현재 게임 씬의 mapId를 서버로 전송
            FlatBufferBuilder builder = new FlatBufferBuilder(1024);
            var changeMap = C_CHANGE_MAP.CreateC_CHANGE_MAP(builder, curScene.MapId);
            var changeMapPkt = Manager.Packet.CreatePacket(changeMap, builder, PacketType.C_CHANGE_MAP);
            Manager.Network.Send(changeMapPkt);
        }
    }
}
