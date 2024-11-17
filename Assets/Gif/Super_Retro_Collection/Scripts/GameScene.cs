using System;
using System.Collections.Generic;
using System.IO;
using UnityEditor.Experimental.GraphView;
using UnityEngine;
using UnityEngine.Experimental.Playables;
using UnityEngine.Tilemaps;

// 우선 샘플 게임 신에서 테스트(맵, 캐릭터 이동, 몬스터 이동, 전투)
public class GameScene : MonoBehaviour
{
    public int MinX { get; set; }
    public int MaxX { get; set; }
    public int MinY { get; set; }
    public int MaxY { get; set; }
    bool[,] _collision;
    GameObject _map;
    public Grid CurrentGrid {  get; set; }

    void Awake()
    {
        ObjectManager objManager = GetComponent<ObjectManager>();

        _map = Resources.Load<GameObject>("Prefabs/Player/TestMap");
        GameObject map = Instantiate(_map);
        map.name = "Map";
        GenerateMapFile();
        InitMap();

        GameObject playerOriginal = Resources.Load<GameObject>("Prefabs/Player/TestPlayer");
        GameObject player = Instantiate(playerOriginal);
        PlayerController playerController = player.GetComponent<PlayerController>();
        
        player.name = "Player";
        objManager.Add(player);

        for (int i = 0; i < 5; ++i)
        {
            GameObject monsterOriginal = Resources.Load<GameObject>("Prefabs/Player/SampleMonster");
            GameObject monster = Instantiate(monsterOriginal);
            monster.name = $"Monster{i + 1}";

            // 랜덤한 위치에 몬스터 생성
            Vector3Int tempPos = new Vector3Int()
            {
                x = UnityEngine.Random.Range(-10, 10),
                y = UnityEngine.Random.Range(-10, 10),
            };

            MonsterController monController = monster.GetComponent<MonsterController>();
            monController.CellPos = tempPos;

            objManager.Add(monster);
        }
    }

    void GenerateMapFile()
    {
        // ground는 갈 수 있고 env는 갈 수 없게 처리
        Tilemap ground = _map.transform.GetChild(0).GetComponent<Tilemap>();
        Tilemap env = _map.transform.GetChild(2).GetComponent<Tilemap>();

        using(var writer = File.CreateText($"Assets/Gif/Super_Retro_Collection/Resources/Prefabs/Player/{_map.name}.txt"))
        {
            writer.WriteLine(ground.cellBounds.xMin);
            writer.WriteLine(ground.cellBounds.xMax);
            writer.WriteLine(ground.cellBounds.yMin);
            writer.WriteLine(ground.cellBounds.yMax);

            // ground는 0 env는 1로 채워서 텍스트 파일 생성
            for (int y = ground.cellBounds.yMax; y >= ground.cellBounds.yMin; y--)
            {
                for (int x = ground.cellBounds.xMin; x <= ground.cellBounds.xMax; x++)
                {
                    TileBase tile = env.GetTile(new Vector3Int(x, y, 0));
                    if (tile != null)
                        writer.Write("1");
                    else
                        writer.Write("0");
                }
                writer.WriteLine();
            }
        }
    }

    public void InitMap()
    {
        CurrentGrid = _map.GetComponent<Grid>();

        TextAsset txt = Resources.Load<TextAsset>("Prefabs/Player/TestMap");
        StringReader reader = new StringReader(txt.text);

        MinX = int.Parse(reader.ReadLine());
        MaxX = int.Parse(reader.ReadLine());
        MinY = int.Parse(reader.ReadLine());
        MaxY = int.Parse(reader.ReadLine());

        int xCount = MaxX - MinX + 1;
        int yCount = MaxY - MinY + 1;
        _collision = new bool[yCount, xCount];

        // 맵 텍스트 파일을 한 줄씩 읽으면서 갈 수 있는 영역(0)은 false
        // 갈 수 없는 영역(1)은 true로 배열에 채움
        for (int y = 0; y < yCount; y++)
        {
            string line = reader.ReadLine();
            for (int x = 0; x < xCount; x++)
            {
                _collision[y, x] = (line[x] == '1' ? true : false);
            }
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

    public struct Pos
    {
        public Pos(int y, int x) { Y = y; X = x; }
        public int Y;
        public int X;
    }

    public List<Vector3Int> FindPathBFS(Vector3Int start, Vector3Int dest, bool ignoreDestCollision = false)
    {
        int[] deltaY = new int[] { 1, -1, 0, 0};
        int[] deltaX = new int[] { 0, 0, -1, 1};

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
                if(!ignoreDestCollision || next.Y != destPos.Y || next.X != destPos.X)
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
}
