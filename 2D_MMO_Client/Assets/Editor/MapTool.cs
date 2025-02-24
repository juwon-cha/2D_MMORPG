using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Tilemaps;
using System.IO;

#if UNITY_EDITOR
using UnityEditor;
#endif

public class MapEditor
{
#if UNITY_EDITOR

    [MenuItem("Tools/GenerateMap")]
    private static void GenerateMap()
    {
        bool result = EditorUtility.DisplayDialog("MapTool", "Are you sure want to generate the map data?", "Ok", "Cancel");
        if (result)
        {
            GenerateByPath("Assets/Resources/MapData");
            GenerateByPath("../Common/MapData");

            EditorUtility.DisplayDialog("MapTool", "Map data generated successfully!", "Ok");
        }
    }

    private static void GenerateByPath(string path)
    {
        GameObject[] gameObjects = Resources.LoadAll<GameObject>("Prefabs/Maps");

        foreach (GameObject go in gameObjects)
        {
            Tilemap tmBase = Utils.FindChild<Tilemap>(go, "BG");
            Tilemap tm = Utils.FindChild<Tilemap>(go, "Collision");

            using (var writer = File.CreateText($"{path}/{go.name}.txt"))
            {
                writer.WriteLine(tmBase.cellBounds.xMin);
                writer.WriteLine(tmBase.cellBounds.xMax);
                writer.WriteLine(tmBase.cellBounds.yMin);
                writer.WriteLine(tmBase.cellBounds.yMax);

                for (int y = tmBase.cellBounds.yMax - 1; y >= tmBase.cellBounds.yMin; y--)
                {
                    for (int x = tmBase.cellBounds.xMin; x < tmBase.cellBounds.xMax; x++)
                    {
                        TileBase tile = tm.GetTile(new Vector3Int(x, y, 0));
                        if (tile != null)
                            writer.Write("1");
                        else
                            writer.Write("0");
                    }
                    writer.WriteLine();
                }
            }
        }
    }
#endif

}
