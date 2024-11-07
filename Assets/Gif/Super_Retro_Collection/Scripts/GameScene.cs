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
                x = Random.Range(-10, 10),
                y = Random.Range(-10, 10),
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
}
