using Google.FlatBuffers;
using ServerCore;
using System;
using UnityEngine;

public class PacketHandler
{
    public static void SC_ENTER_GAMEHandler(PacketSession session, ByteBuffer buffer)
    {
        var enter = SC_ENTER_GAME.GetRootAsSC_ENTER_GAME(buffer);

        // 오브젝트 매니저에서 캐릭터 게임 오브젝트 생성
        Manager.Object.Add(enter.Player.Value, myPlayer: true);
        Debug.Log("SC_ENTER_GAMEHandler");
        Debug.Log($"ID: {enter.Player.Value.PlayerId}, NAME: {enter.Player.Value.Name.ToString()}");
    }

    public static void SC_LEAVE_GAMEHandler(PacketSession session, ByteBuffer buffer)
    {
        var leave = SC_LEAVE_GAME.GetRootAsSC_LEAVE_GAME(buffer);

        Manager.Object.RemoveMyPlayer();
        Debug.Log("SC_LEAVE_GAMEHandler");
    }

    public static void SC_SPAWNHandler(PacketSession session, ByteBuffer buffer)
    {
        var spawn = SC_SPAWN.GetRootAsSC_SPAWN(buffer);

        for (int i = 0; i < spawn.PlayersLength; ++i)
        {
            Manager.Object.Add(spawn.Players(i).Value, myPlayer: false);
        }

        Debug.Log("SC_SPAWNHandler");
        for (int i = 0; i < spawn.PlayersLength; ++i)
        {
            Debug.Log($"ID: {spawn.Players(i).Value.PlayerId}, NAME: {spawn.Players(i).Value.Name.ToString()}");
        }
    }

    public static void SC_DESPAWNHandler(PacketSession session, ByteBuffer buffer)
    {
        var despawn = SC_DESPAWN.GetRootAsSC_DESPAWN(buffer);

        for (int i = 0; i < despawn.PlayerIdsLength; ++i)
        {
            Manager.Object.Remove(despawn.PlayerIds(i));
        }

        Debug.Log("SC_DESPAWNHandler");
    }

    public static void SC_MOVEHandler(PacketSession session, ByteBuffer buffer)
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

    public static void SC_ChatHandler(PacketSession session, ByteBuffer buffer)
    {
        var pkt = SC_Chat.GetRootAsSC_Chat(buffer);

        var playerInfo = pkt.Player.Value;

        var id = playerInfo.PlayerId;
        var player = Manager.Object.FindById(id);
        if (player == null)
            return;
        var arr = GameObject.FindObjectsByType<ChatUIController>(FindObjectsSortMode.None);
        arr[0].PushChat(id, pkt.Text);
    }
}
