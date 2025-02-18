using UnityEngine;

public class Utils
{
    public static GameObject FindChild(GameObject go, string name)
    {
        Transform transform = FindChild<Transform>(go, name);

        if(transform == null)
        {
            return null;
        }

        return transform.gameObject;
    }

    public static T FindChild<T>(GameObject go, string name) where T : Object
    {
        if(go == null)
        {
            return null;
        }

        for(int i = 0; i < go.transform.childCount; i++)
        {
            Transform transform = go.transform.GetChild(i);

            if(string.IsNullOrEmpty(name) || transform.name == name)
            {
                T component = transform.GetComponent<T>();
                if(component != null)
                {
                    return component;
                }
            }
        }

        return null;
    }
}
