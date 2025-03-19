using System;
using System.Collections.Generic;
using System.IO;
using UnityEngine;
using UnityEngine.Tilemaps;

public class GameScene : BaseScene
{
    InventoryController invenController;

    protected override void Init()
    {
        base.Init();

        SceneType = Define.SceneType.INGAME;

        Manager.Map.LoadMap(1);

        Screen.SetResolution(640, 480, false);

        invenController = Manager.UI.ShowInventoryUI();
        GameObject inven = invenController.gameObject;
        inven.SetActive(false); // ó���� �κ��丮 â ��Ȱ��ȭ
    }
}
