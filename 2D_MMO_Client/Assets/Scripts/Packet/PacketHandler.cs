using Google.FlatBuffers;
using ServerCore;
using System;
using UnityEngine;

public class PacketHandler
{
    public static void SC_ENTER_GAME_Handler(PacketSession session, ByteBuffer buffer)
    {
        var enter = SC_ENTER_GAME.GetRootAsSC_ENTER_GAME(buffer);

        // 오브젝트 매니저에서 캐릭터 게임 오브젝트 생성
        Manager.Object.Add(enter.Player.Value, myPlayer: true);
        Debug.Log("SC_ENTER_GAME_Handler");
        Debug.Log($"ID: {enter.Player.Value.PlayerId}, NAME: { enter.Player.Value.Name.ToString()}");
    }

    public static void SC_LEAVE_GAME_Handler(PacketSession session, ByteBuffer buffer)
    {
        var leave = SC_LEAVE_GAME.GetRootAsSC_LEAVE_GAME(buffer);

        Manager.Object.RemoveMyPlayer();
        Debug.Log("SC_LEAVE_GAME_Handler");
    }

    public static void SC_SPAWN_Handler(PacketSession session, ByteBuffer buffer)
    {
        var spawn = SC_SPAWN.GetRootAsSC_SPAWN(buffer);

        for (int i = 0; i < spawn.PlayersLength; ++i)
        {
            Manager.Object.Add(spawn.Players(i).Value, myPlayer: false);
        }

        Debug.Log("SC_SPAWN_Handler");
        for(int i = 0; i < spawn.PlayersLength; ++i)
        {
            Debug.Log($"ID: {spawn.Players(i).Value.PlayerId}, NAME: {spawn.Players(i).Value.Name.ToString()}");
        }
    }

    public static void SC_DESPAWN_Handler(PacketSession session, ByteBuffer buffer)
    {
        var despawn = SC_DESPAWN.GetRootAsSC_DESPAWN(buffer);

        for (int i = 0; i < despawn.PlayerIdsLength; ++i)
        {
            Manager.Object.Remove(despawn.PlayerIds(i));
        }

        Debug.Log("SC_DESPAWN_Handler");
    }

    public static void SC_MOVE_Handler(PacketSession session, ByteBuffer buffer)
    {
        var move = SC_MOVE.GetRootAsSC_MOVE(buffer);

        GameObject go = Manager.Object.FindById(move.PlayerId);
        if (go == null)
        {
            return;
        }

        ObjectController objController = go.GetComponent<ObjectController>();
        if (objController == null)
        {
            return;
        }

        objController.State = (Define.ObjectState)move.PosInfo.Value.State;
        objController.MoveDir = (Define.MoveDir)move.PosInfo.Value.MoveDir;
        objController.CellPos = new Vector3Int(move.PosInfo.Value.PosX, move.PosInfo.Value.PosY, 0);
    }
}
