using System;
using System.Collections.Generic;
using System.IO;
using UnityEngine;
using UnityEngine.Tilemaps;
using UnityEngine.UIElements;

public class GameScene : BaseScene
{
    public int MapId { get; set; }
    InventoryController _inventory;

    protected override void Init()
    {
        base.Init();

        SceneType = Define.SceneType.INGAME;

        MapId = 1;
        Manager.Map.LoadMap(MapId);

        Screen.SetResolution(640, 480, false);

        _inventory = Manager.UI.ShowInventoryUI();
        var uiDoc = _inventory.gameObject.GetComponent<UIDocument>();
        // 게임 시작 시 인벤토리 숨기기
        uiDoc.rootVisualElement.style.visibility = Visibility.Hidden;
    }
}
