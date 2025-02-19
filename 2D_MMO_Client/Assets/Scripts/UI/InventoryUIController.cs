using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.UIElements;

public class InventoryController : BaseUIController
{
    VisualElement root;
    [SerializeField]
    Sprite[] ItemBG;
    [SerializeField]
    int itemCount;
    protected override void Init()
    {
        base.Init();
        root = document.rootVisualElement;

        var groupBox = root.Q<GroupBox>("GroupBox-Item");
        var bow = Resources.Load<Texture2D>("BattleBow");
        for (int i = 0; i < itemCount; i++)
        {
            VisualElement edge = new VisualElement();
            edge.AddToClassList("Item-Edge");
            edge.name = $"Item{i.ToString()}";
            VisualElement bg = new VisualElement();
            bg.name = $"Item-BG";
            edge.Add(bg);
            groupBox.Add(edge);

            bg.style.backgroundImage = bow;
            bg.style.flexGrow = 0;
            bg.style.position = Position.Absolute;
            bg.style.width = new Length(100, LengthUnit.Percent);
            bg.style.height = new Length(100, LengthUnit.Percent);
            edge.Add(bg);
            int n;
            if (i <= 5)
                n = 0;
            else if (i >= itemCount - 5)
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
            DragAndDropManipulator mainpulato = new(bg, root);
        }
    }
}

public class DragAndDropManipulator : PointerManipulator
{
    public DragAndDropManipulator(VisualElement target, VisualElement _root)
    {
        this.target = target;
        root = _root;
        EquipmentUi = root.Q<VisualElement>("Equipment-BG");
        InventoryUi = root.Q<VisualElement>("Inventory-BG");
    }

    protected override void RegisterCallbacksOnTarget()
    {
        target.RegisterCallback<PointerDownEvent>(PointerDownHandler);
        target.RegisterCallback<PointerMoveEvent>(PointerMoveHandler);
        target.RegisterCallback<PointerUpEvent>(PointerUpHandler);
        target.RegisterCallback<PointerCaptureOutEvent>(PointerCaptureOutHandler);
    }

    protected override void UnregisterCallbacksFromTarget()
    {
        target.UnregisterCallback<PointerDownEvent>(PointerDownHandler);
        target.UnregisterCallback<PointerMoveEvent>(PointerMoveHandler);
        target.UnregisterCallback<PointerUpEvent>(PointerUpHandler);
        target.UnregisterCallback<PointerCaptureOutEvent>(PointerCaptureOutHandler);
    }

    private Vector2 targetStartPosition { get; set; }

    private Vector3 pointerStartPosition { get; set; }

    private bool enabled { get; set; }
    private Vector3 LastPosition { get; set; }

    private VisualElement root { get; }
    private VisualElement EquipmentUi { get; set; }
    private VisualElement InventoryUi { get; set; }

    private void PointerDownHandler(PointerDownEvent evt)
    {
        targetStartPosition = target.transform.position;
        pointerStartPosition = evt.position;
        target.parent.BringToFront(); // 현재 item 칸 id를 가장 앞으로 가져옴
        if (FindSpecificParent(target, "Equipment-BG") == EquipmentUi)
            EquipmentUi.BringToFront();
        else
            InventoryUi.BringToFront();
        target.CapturePointer(evt.pointerId);
        enabled = true;
        LastPosition = targetStartPosition;
    }

    private void PointerMoveHandler(PointerMoveEvent evt)
    {
        if (enabled && target.HasPointerCapture(evt.pointerId))
        {
            Vector3 pointerDelta = evt.position - pointerStartPosition;
            target.transform.position = new Vector2(targetStartPosition.x + pointerDelta.x,
                targetStartPosition.y + pointerDelta.y);
            LastPosition = evt.position;
        }
    }

    private void PointerUpHandler(PointerUpEvent evt)
    {
        if (enabled && target.HasPointerCapture(evt.pointerId))
        {
            target.ReleasePointer(evt.pointerId);
        }
    }

    private void PointerCaptureOutHandler(PointerCaptureOutEvent evt)
    {
        if (enabled && target.style.backgroundImage.value != null)
        {
            UQueryBuilder<VisualElement> allSlots =
                root.Query<VisualElement>(className: "Item-Edge");
            UQueryBuilder<VisualElement> overlappingSlots =
                allSlots.Where(OverlapsTarget);
            VisualElement closestOverlappingSlot =
                FindClosestSlot(overlappingSlots);

            if (closestOverlappingSlot != null && closestOverlappingSlot != target.parent)
            {
                VisualElement item = closestOverlappingSlot.Q<VisualElement>("Item-BG");

                // 이미지 스왚
                var temp = item.style.backgroundImage;
                item.style.backgroundImage = target.style.backgroundImage;
                target.style.backgroundImage = temp;

                // 나중에 실제 데이터 또한 스왚해야함
            }
        }
        target.transform.position = targetStartPosition;
        enabled = false;
    }
    private bool OverlapsTarget(VisualElement slot)
    {
        return target.worldBound.Overlaps(slot.worldBound);
    }

    private VisualElement FindClosestSlot(UQueryBuilder<VisualElement> slots)
    {
        List<VisualElement> slotsList = slots.ToList();
        foreach (VisualElement slot in slots.ToList())
        {
            if (slot.worldBound.Contains(LastPosition))
                return slot;
        }
        return null;
    }
    VisualElement FindSpecificParent(VisualElement element, string targetName)
    {
        while (element != null)
        {
            if (element.name == targetName)
            {
                return element;
            }
            element = element.parent; // 상위 부모로 이동
        }
        return null;
    }

}