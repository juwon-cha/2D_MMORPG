using System;
using System.Collections.Generic;
using System.IO;
using UnityEditor.Experimental.GraphView;
using UnityEngine;
using UnityEngine.Experimental.Playables;
using UnityEngine.Tilemaps;

// �켱 ���� ���� �ſ��� �׽�Ʈ(��, ĳ���� �̵�, ���� �̵�, ����)
public class GameScene : BaseScene
{
    protected override void Init()
    {
        base.Init();

        SceneType = Define.SceneType.INGAME;

        Manager.Map.LoadMap(1);

        //GameObject playerOriginal = Resources.Load<GameObject>("Prefabs/Character/Player/TestPlayer");
        //GameObject player = Instantiate(playerOriginal);
        //PlayerController playerController = player.GetComponent<PlayerController>();

        //player.name = "Player";
        //Manager.Object.Add(player);

        //for (int i = 0; i < 3; ++i)
        //{
        //    GameObject monsterOriginal = Resources.Load<GameObject>("Prefabs/Character/Player/SampleMonster");
        //    GameObject monster = Instantiate(monsterOriginal);
        //    monster.name = $"Monster{i + 1}";

        //    // ������ ��ġ�� ���� ����
        //    Vector3Int tempPos = new Vector3Int()
        //    {
        //        x = UnityEngine.Random.Range(-5, 5),
        //        y = UnityEngine.Random.Range(-5, 5),
        //    };

        //    MonsterController monController = monster.GetComponent<MonsterController>();
        //    monController.CellPos = tempPos;

        //    Manager.Object.Add(monster);
        //}
    }
}
