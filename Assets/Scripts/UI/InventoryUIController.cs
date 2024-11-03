using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UIElements;

public class InventoryController : BaseUIController
{
    struct Item
    {
        public VisualElement Edge;
        public VisualElement BG;
    }
    VisualElement root;
    [SerializeField]
    Sprite[] ItemBG;
    Dictionary<int, Item> items = new Dictionary<int, Item>();
    [SerializeField]
    int itemCount;
    protected override void Init()
    {
        base.Init();
        root = document.rootVisualElement;

        for (int i = 1; i < itemCount + 1; i++)
        {
            VisualElement edge = root.Q<VisualElement>($"Item{i.ToString()}");
            VisualElement bg = root.Q<VisualElement>($"Item{i.ToString()}BG");
            Item item = new Item();
            item.Edge = edge;
            item.BG = bg;
            items.Add(i, item);
            if (i <= 5)
            {
                switch (i % 5)
                {
                    case 0:
                        edge.style.backgroundImage = new StyleBackground(ItemBG[2]);
                        break;
                    case 1:
                        edge.style.backgroundImage = new StyleBackground(ItemBG[0]);
                        break;
                    default:
                        edge.style.backgroundImage = new StyleBackground(ItemBG[1]);
                        break;
                }
            }
            else if (i >= itemCount - 5)
            {
                switch (i % 5)
                {
                    case 0:
                        edge.style.backgroundImage = new StyleBackground(ItemBG[5]);
                        break;
                    case 1:
                        edge.style.backgroundImage = new StyleBackground(ItemBG[3]);
                        break;
                    default:
                        edge.style.backgroundImage = new StyleBackground(ItemBG[4]);
                        break;
                }
            }
            else
            {
                switch (i % 5)
                {
                    case 0:
                        edge.style.backgroundImage = new StyleBackground(ItemBG[8]);
                        break;
                    case 1:
                        edge.style.backgroundImage = new StyleBackground(ItemBG[6]);
                        break;
                    default:
                        edge.style.backgroundImage = new StyleBackground(ItemBG[7]);
                        break;
                }
            }
        }
    }
}
