using System;
using System.Collections.Generic;
using System.IO;
using UnityEditor.Experimental.GraphView;
using UnityEngine;
using UnityEngine.Experimental.Playables;
using UnityEngine.Tilemaps;

// 우선 샘플 게임 신에서 테스트(맵, 캐릭터 이동, 몬스터 이동, 전투)
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

        //    // 랜덤한 위치에 몬스터 생성
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
