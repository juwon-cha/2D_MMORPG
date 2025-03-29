using Google.FlatBuffers;
using ServerCore;
using UnityEngine;

public partial class PacketHandler
{
    public static void SC_ENTER_GAMEHandler(PacketSession session, ByteBuffer buffer)
    {
        var enter = SC_ENTER_GAME.GetRootAsSC_ENTER_GAME(buffer);

        // 오브젝트 매니저에서 캐릭터 게임 오브젝트 생성
        Manager.Object.Add(enter.Player.Value, myPlayer: true);
        Debug.Log("SC_ENTER_GAMEHandler");
        Debug.Log($"ID: {enter.Player.Value.ObjectId}, NAME: {enter.Player.Value.Name.ToString()}");
    }

    public static void SC_LEAVE_GAMEHandler(PacketSession session, ByteBuffer buffer)
    {
        var leave = SC_LEAVE_GAME.GetRootAsSC_LEAVE_GAME(buffer);

        Manager.Object.Clear();
        Debug.Log("SC_LEAVE_GAMEHandler");
    }

    public static void SC_SPAWNHandler(PacketSession session, ByteBuffer buffer)
    {
        var spawn = SC_SPAWN.GetRootAsSC_SPAWN(buffer);

        for (int i = 0; i < spawn.ObjectsLength; ++i)
        {
            Manager.Object.Add(spawn.Objects(i).Value, myPlayer: false);
        }

        Debug.Log("SC_SPAWNHandler");
        for (int i = 0; i < spawn.ObjectsLength; ++i)
        {
            Debug.Log($"ID: {spawn.Objects(i).Value.ObjectId}, NAME: {spawn.Objects(i).Value.Name.ToString()}");
        }
    }

    public static void SC_DESPAWNHandler(PacketSession session, ByteBuffer buffer)
    {
        var despawn = SC_DESPAWN.GetRootAsSC_DESPAWN(buffer);

        for (int i = 0; i < despawn.ObjectIdsLength; ++i)
        {
            Manager.Object.Remove(despawn.ObjectIds(i));
        }

        Debug.Log("SC_DESPAWNHandler");
    }

    public static void SC_MOVEHandler(PacketSession session, ByteBuffer buffer)
    {
        var move = SC_MOVE.GetRootAsSC_MOVE(buffer);

        GameObject go = Manager.Object.FindById(move.ObjectId);
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

    public static void SC_CHATHandler(PacketSession session, ByteBuffer buffer)
    {
        var pkt = SC_CHAT.GetRootAsSC_CHAT(buffer);

        var playerInfo = pkt.Player.Value;

        var id = playerInfo.ObjectId;
        var player = Manager.Object.FindById(id);
        if (player == null)
            return;
        var arr = GameObject.FindObjectsByType<ChatUIController>(FindObjectsSortMode.None);
        arr[0].PushChat(id, pkt.Text);
    }

    public static void SC_SKILLHandler(PacketSession session, ByteBuffer buffer)
    {
        var skill = SC_SKILL.GetRootAsSC_SKILL(buffer);

        GameObject go = Manager.Object.FindById(skill.ObjectId);
        if (go == null)
        {
            return;
        }

        ObjectController objController = go.GetComponent<ObjectController>();
        if (objController == null)
        {
            return;
        }

        objController.UseSkill(skill.SkillInfo.Value.SkillId);
    }

    public static void SC_CHANGE_HPHandler(PacketSession session, ByteBuffer buffer)
    {
        var changeHpPkt = SC_CHANGE_HP.GetRootAsSC_CHANGE_HP(buffer);

        GameObject go = Manager.Object.FindById(changeHpPkt.ObjectId);
        if (go == null)
        {
            return;
        }

        ObjectController objController = go.GetComponent<ObjectController>();
        if (objController == null)
        {
            return;
        }

        objController.HP = changeHpPkt.Hp;
    }

    public static void SC_DIEHandler(PacketSession session, ByteBuffer buffer)
    {
        var diePkt = SC_DIE.GetRootAsSC_DIE(buffer);

        GameObject go = Manager.Object.FindById(diePkt.ObjectId);
        if (go == null)
        {
            return;
        }

        ObjectController objController = go.GetComponent<ObjectController>();
        if (objController == null)
        {
            return;
        }

        objController.HP = 0;
        objController.OnDead();
    }

    public static void SC_ITEM_LISTHandler(PacketSession session, ByteBuffer buffer)
    {
        var itemListPkt = SC_ITEM_LIST.GetRootAsSC_ITEM_LIST(buffer);

        for (int i = 0; i < itemListPkt.ItemsLength; ++i)
        {
            Debug.Log($"Item template ID {itemListPkt.Items(i).Value.TemplateId} : Count {itemListPkt.Items(i).Value.Count}");
        }

        InventoryController inven = Manager.UI.InvenUI;

        // 메모리에 아이템 정보 적용
        for (int i = 0; i < itemListPkt.ItemsLength; ++i)
        {
            Item item = Item.MakeItem(itemListPkt.Items(i).Value);
            Manager.Inven.Add(item);
        }

        // UI 갱신
        if (inven == null)
        {
            return;
        }

        inven.RefreshUI();
    }

    public static void SC_EQUIP_ITEMHandler(PacketSession session, ByteBuffer buffer)
    {
        var equipItemPkt = SC_EQUIP_ITEM.GetRootAsSC_EQUIP_ITEM(buffer);

        // 메모리에 아이템 정보 적용
        Item item = Manager.Inven.Get(equipItemPkt.ItemId);
        if (item == null)
        {
            return;
        }

        item.Equipped = equipItemPkt.Equipped;
        Debug.Log($"아이템 착용 변경!");

        // 아이템 착용에 따라 플레이어 스킬 변경
        if (Manager.Object.MyPlayer != null)
        {
            Manager.Object.MyPlayer.SkillId = item.TemplateId;
        }
    }

    public static void SC_CHANGE_MAPHandler(PacketSession session, ByteBuffer buffer)
    {
        var changeMapPkt = SC_CHANGE_MAP.GetRootAsSC_CHANGE_MAP(buffer);

        GameScene curScene = Manager.Scene.CurScene as GameScene;
        curScene.MapId = changeMapPkt.MapId;

        // 맵 전환
        Manager.Map.LoadMap(curScene.MapId);
    }
}
