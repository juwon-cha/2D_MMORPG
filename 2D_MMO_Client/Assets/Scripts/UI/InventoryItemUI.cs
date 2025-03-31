using UnityEngine;
using UnityEngine.UIElements;

public class InventoryItemUI : InventoryController
{
    public void SetItem(VisualElement root, int templateId, int count, int slot)
    {
        Data.ItemData itemData = null;
        DataManager.ItemDict.TryGetValue(templateId, out itemData);

        var icon = Resources.Load<Texture2D>(itemData.IconPath);

        var groupBox = root.Q<GroupBox>("GroupBox-Item");
        for (int i = slot; i <= slot; i++)
        {
            VisualElement edge = new VisualElement();
            edge.AddToClassList("Item-Edge");
            edge.name = $"Item{i.ToString()}";
            VisualElement bg = new VisualElement();
            bg.name = $"Item-BG";
            edge.Add(bg);
            groupBox.Add(edge);

            bg.style.backgroundImage = icon; // 아이템 이미지
            bg.style.flexGrow = 0;
            bg.style.position = Position.Absolute;
            bg.style.width = new Length(100, LengthUnit.Percent);
            bg.style.height = new Length(100, LengthUnit.Percent);
            edge.Add(bg);

            int n;
            if (i <= 5)
                n = 0;
            else if (i >= slot - 5)
                n = 6;
            else
                n = 3;

            switch (i % 5)
            {
                case 0:
                    edge.style.backgroundImage = new StyleBackground(ItemBG[0 + n]);
                    break;
                case 1:
                    edge.style.backgroundImage = new StyleBackground(ItemBG[1 + n]);
                    break;
                default:
                    edge.style.backgroundImage = new StyleBackground(ItemBG[2 + n]);
                    break;
            }

            float top = (i / 5) * 14f;
            float left = (i % 5) * 20f;
            edge.style.left = new Length(left, LengthUnit.Percent);
            edge.style.top = new Length(top, LengthUnit.Percent);

        }

        var edges = root.Query<VisualElement>(className: "Item-Edge");
        foreach (var edge in edges.ToList())
        {
            VisualElement bg = edge.Q<VisualElement>("Item-BG");
            DragAndDropManipulator mainpulator = new(bg, root);
        }
    }
}
