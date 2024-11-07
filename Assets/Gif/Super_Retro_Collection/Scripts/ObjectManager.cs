using NUnit.Framework;
using System.Collections.Generic;
using UnityEngine;

public class ObjectManager : MonoBehaviour
{
    List<GameObject> _objects = new List<GameObject>();

    public void Add(GameObject obj)
    {
        _objects.Add(obj);
    }

    public GameObject Find(Vector3Int cellPos)
    {
        foreach (GameObject obj in _objects)
        {
            ObjectController controller = obj.GetComponent<ObjectController>();
            if (controller == null)
            {
                continue;
            }

            if(controller.CellPos == cellPos)
            {
                return obj;
            }
        }

        return null;
    }

    public void Remove(GameObject obj)
    {
        _objects.Remove(obj);
    }

    public void Clear(GameObject obj)
    {
        _objects.Clear();
    }
}
