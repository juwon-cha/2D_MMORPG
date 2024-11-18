using UnityEngine;
using UnityEngine.UIElements;

public class SettingUIController : BaseUIController
{
    VisualElement root;
    protected override void Init()
    {
        base.Init();
        root = document.rootVisualElement;
        print(root.name);
    }
}
