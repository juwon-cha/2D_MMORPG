using NUnit.Framework;
using System;
using System.Collections.Generic;
using UnityEngine;

public class ObjectManager
{
    List<GameObject> _objects = new List<GameObject>();

    public void Add(GameObject obj)
    {
        _objects.Add(obj);
    }

    // 해당 좌표에 게임오브젝트가 있는지 찾고 반환
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

    public GameObject Find(Func<GameObject, bool> condition)
    {
        foreach (GameObject obj in _objects)
        {
            ObjectController controller = obj.GetComponent<ObjectController>();
            if (controller == null)
            {
                continue;
            }

            if (condition.Invoke(obj))
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
