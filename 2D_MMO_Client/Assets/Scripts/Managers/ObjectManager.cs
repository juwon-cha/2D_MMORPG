using System;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

public class ObjectManager
{
    public MyPlayerController MyPlayer { get; set; }
    Dictionary<int, GameObject> _objects = new Dictionary<int, GameObject>();

    public static GameObjectType GetObjectTypeById(int id)
    {
        int type = (id >> 24) & 0x7F;
        return (GameObjectType)type;
    }

    public void Add(ObjectInfo info, bool myPlayer = false)
    {
        GameObjectType objType = GetObjectTypeById(info.ObjectId);
        if (objType == GameObjectType.PLAYER)
        {
            if (myPlayer) // 내가 조종할 플레이어
            {
                GameObject playerOriginal = Resources.Load<GameObject>("Prefabs/Creature/Player/MyPlayer");
                GameObject player = UnityEngine.Object.Instantiate(playerOriginal);
                player.name = info.Name;
                _objects.Add(info.ObjectId, player);

                MyPlayer = player.GetComponent<MyPlayerController>();
                MyPlayer.Id = info.ObjectId;
                MyPlayer.CellPos = new Vector3Int(info.PosInfo.Value.PosX, info.PosInfo.Value.PosY, 0);
                MyPlayer.MoveDir = (Define.MoveDir)info.PosInfo.Value.MoveDir;
                MyPlayer.State = (Define.ObjectState)info.PosInfo.Value.State;
                MyPlayer.HP = info.StatInfo.Value.Hp;
                MyPlayer.MaxHP = info.StatInfo.Value.MaxHp;
                MyPlayer.Speed = info.StatInfo.Value.Speed;

                MyPlayer.SyncPos();
            }
            else // 내가 아닌 다른 플레이어
            {
                GameObject playerOriginal = Resources.Load<GameObject>("Prefabs/Creature/Player/Player");
                GameObject player = UnityEngine.Object.Instantiate(playerOriginal);
                player.name = info.Name;
                _objects.Add(info.ObjectId, player);

                PlayerController pc = player.GetComponent<PlayerController>();
                pc.Id = info.ObjectId;
                pc.CellPos = new Vector3Int(info.PosInfo.Value.PosX, info.PosInfo.Value.PosY, 0);
                pc.MoveDir = (Define.MoveDir)info.PosInfo.Value.MoveDir;
                pc.State = (Define.ObjectState)info.PosInfo.Value.State;
                pc.HP = info.StatInfo.Value.Hp;
                pc.MaxHP = info.StatInfo.Value.MaxHp;
                pc.Speed = info.StatInfo.Value.Speed;

                pc.SyncPos();
            }
        }
        else if (objType == GameObjectType.MONSTER)
        {
            GameObject monsterOriginal = Resources.Load<GameObject>("Prefabs/Creature/Monster/SLIME");
            GameObject monster = UnityEngine.Object.Instantiate(monsterOriginal);
            monster.name = info.Name;
            _objects.Add(info.ObjectId, monster);

            MonsterController mc = monster.GetComponent<MonsterController>();
            mc.Id = info.ObjectId;
            mc.CellPos = new Vector3Int(info.PosInfo.Value.PosX, info.PosInfo.Value.PosY, 0);
            mc.MoveDir = (Define.MoveDir)info.PosInfo.Value.MoveDir;
            mc.State = (Define.ObjectState)info.PosInfo.Value.State;
            mc.HP = info.StatInfo.Value.Hp;
            mc.MaxHP = info.StatInfo.Value.MaxHp;
            mc.Speed = info.StatInfo.Value.Speed;

            mc.SyncPos();
        }
        else if (objType == GameObjectType.PROJECTILE)
        {

        }
    }

    public void Add(int id, GameObject obj)
    {
        _objects.Add(id, obj);
    }

    public GameObject FindById(int id)
    {
        GameObject go = null;

        _objects.TryGetValue(id, out go);

        return go;
    }

    // 해당 좌표에 게임오브젝트가 있는지 찾고 반환
    public GameObject Find(Vector3Int cellPos)
    {
        foreach (GameObject obj in _objects.Values)
        {
            ObjectController controller = obj.GetComponent<ObjectController>();
            if (controller == null)
            {
                continue;
            }

            if (controller.CellPos == cellPos)
            {
                return obj;
            }
        }

        return null;
    }

    public GameObject Find(Func<GameObject, bool> condition)
    {
        foreach (GameObject obj in _objects.Values)
        {
            ObjectController controller = obj.GetComponent<ObjectController>();
            if (controller == null)
            {
                continue;
            }

            if (condition.Invoke(obj))
            {
                return obj;
            }
        }

        return null;
    }

    public void Remove(int id)
    {
        GameObject go = FindById(id);
        if (go == null)
        {
            return;
        }

        _objects.Remove(id);
        UnityEngine.Object.Destroy(go);
    }

    public void RemoveMyPlayer()
    {
        if (MyPlayer == null)
        {
            return;
        }

        Remove(MyPlayer.Id);
        MyPlayer = null;
    }

    public void Clear(GameObject obj)
    {
        foreach (GameObject go in _objects.Values)
        {
            UnityEngine.Object.Destroy(go);
        }

        _objects.Clear();
    }
}
