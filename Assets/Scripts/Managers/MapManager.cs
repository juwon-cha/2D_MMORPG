using System.Collections.Generic;
using System.IO;
using UnityEngine;
using static GameScene;

public struct Pos
{
    public Pos(int y, int x) { Y = y; X = x; }
    public int Y;
    public int X;
}

public class MapManager
{
    public Grid CurrentGrid { get; private set; }

    public int MinX { get; set; }
    public int MaxX { get; set; }
    public int MinY { get; set; }
    public int MaxY { get; set; }

    public int SizeX { get { return MaxX - MinX + 1; } }
    public int SizeY { get { return MaxY - MinY + 1; } }

    bool[,] _collision;

    public void LoadMap(int mapId)
    {
        DestroyMap();

        string mapName = "Map_" + mapId.ToString("000");
        GameObject map = Resources.Load<GameObject>($"Prefabs/Maps/{mapName}");
        GameObject go = UnityEngine.Object.Instantiate(map);
        go.name = "Map";

        GameObject collision = Utils.FindChild(go, "Collision");
        if (collision == null)
        {
            throw new System.Exception("Can not find Collision!");
        }

        collision.SetActive(false);

        CurrentGrid = go.GetComponent<Grid>();

        // Collision 관련 파일
        TextAsset txt = Resources.Load<TextAsset>($"MapData/{mapName}");
        StringReader reader = new StringReader(txt.text);

        MinX = int.Parse(reader.ReadLine());
        MaxX = int.Parse(reader.ReadLine()) - 1;
        MinY = int.Parse(reader.ReadLine());
        MaxY = int.Parse(reader.ReadLine()) - 1;

        int xCount = MaxX - MinX + 1;
        int yCount = MaxY - MinY + 1;
        _collision = new bool[yCount, xCount];

        for (int y = 0; y < yCount; y++)
        {
            string line = reader.ReadLine();
            for (int x = 0; x < xCount; x++)
            {
                _collision[y, x] = (line[x] == '1' ? true : false);
            }
        }
    }

    public void DestroyMap()
    {
        GameObject map = GameObject.Find("Map");
        if (map != null)
        {
            GameObject.Destroy(map);
            CurrentGrid = null;
        }
    }

    // 플레이어가 갈 수 있는지 없는지 판별
    public bool CanMove(Vector3Int cellPos)
    {
        if (cellPos.x < MinX || cellPos.x > MaxX)
            return false;
        if (cellPos.y < MinY || cellPos.y > MaxY)
            return false;

        int x = cellPos.x - MinX;
        int y = MaxY - cellPos.y;
        // 갈 수 있는 영역(0, false)은 반전시켜서 반환(CanMove가 참이면 움직일 수 있는 영역)
        return !_collision[y, x];
    }

    #region PathFinding

    public List<Vector3Int> FindPathBFS(Vector3Int start, Vector3Int dest, bool ignoreDestCollision = false)
    {
        int[] deltaY = new int[] { 1, -1, 0, 0 };
        int[] deltaX = new int[] { 0, 0, -1, 1 };

        bool[,] found = new bool[MaxY - MinY + 1, MaxX - MinX + 1];
        Pos[,] parent = new Pos[MaxY - MinY + 1, MaxX - MinX + 1];

        Pos startPos = CellToPos(start);
        Pos destPos = CellToPos(dest);

        Queue<Pos> queue = new Queue<Pos>();
        queue.Enqueue(new Pos(startPos.Y, startPos.X));

        found[startPos.Y, startPos.X] = true;
        parent[startPos.Y, startPos.X] = new Pos(startPos.Y, startPos.X);

        while (queue.Count > 0)
        {
            Pos pos = queue.Dequeue();
            int nowY = pos.Y;
            int nowX = pos.X;

            for (int i = 0; i < deltaY.Length; ++i)
            {
                Pos next = new Pos(nowY + deltaY[i], nowX + deltaX[i]);

                // 막혀있으면 스킵
                if (!ignoreDestCollision || next.Y != destPos.Y || next.X != destPos.X)
                {
                    if (CanMove(PosToCell(next)) == false)
                    {
                        continue;
                    }
                }
                // 이미 방문한 곳이면 스킵
                if (found[next.Y, next.X])
                {
                    continue;
                }

                queue.Enqueue(new Pos(next.Y, next.X));
                found[next.Y, next.X] = true;
                parent[next.Y, next.X] = new Pos(nowY, nowX);
            }
        }

        return CalcCellPathFromParent(parent, destPos);
    }

    List<Vector3Int> CalcCellPathFromParent(Pos[,] parent, Pos dest)
    {
        List<Vector3Int> cells = new List<Vector3Int>();

        int y = dest.Y;
        int x = dest.X;
        while (parent[y, x].Y != y || parent[y, x].X != x)
        {
            cells.Add(PosToCell(new Pos(y, x)));
            Pos pos = parent[y, x];
            y = pos.Y;
            x = pos.X;
        }
        cells.Add(PosToCell(new Pos(y, x)));
        cells.Reverse();

        return cells;
    }

    Pos CellToPos(Vector3Int cell)
    {
        return new Pos(MaxY - cell.y, cell.x - MinX);
    }

    Vector3Int PosToCell(Pos pos)
    {
        return new Vector3Int(pos.X + MinX, MaxY - pos.Y, 0);
    }

    #endregion
}
