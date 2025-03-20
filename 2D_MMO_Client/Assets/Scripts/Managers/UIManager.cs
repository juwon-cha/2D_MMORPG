using UnityEngine;

public class UIManager
{
    public InventoryController Inven { get; private set; }

    public GameObject Root
    {
        get
        {
            GameObject root = GameObject.Find("UI_Root");
            if (root == null)
                root = new GameObject { name = "UI_Root" };
            return root;
        }
    }

    public InventoryController ShowInventoryUI()
    {
        GameObject invenOriginal = Resources.Load<GameObject>("Prefabs/UI/Inventory");
        GameObject inventory = UnityEngine.Object.Instantiate(invenOriginal);
        inventory.name = "Inventory";

        InventoryController invenController = inventory.GetComponent<InventoryController>();
        Inven = invenController;

        inventory.transform.SetParent(Root.transform);

        return invenController;
    }
}
