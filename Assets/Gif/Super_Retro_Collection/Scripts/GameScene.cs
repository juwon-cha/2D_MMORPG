using System.IO;
using UnityEditor.Experimental.GraphView;
using UnityEngine;
using UnityEngine.Experimental.Playables;
using UnityEngine.Tilemaps;

// �켱 ���� ���� �ſ��� �׽�Ʈ(��, ĳ���� �̵�, ���� �̵�, ����)
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

            // ������ ��ġ�� ���� ����
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
        // ground�� �� �� �ְ� env�� �� �� ���� ó��
        Tilemap ground = _map.transform.GetChild(0).GetComponent<Tilemap>();
        Tilemap env = _map.transform.GetChild(2).GetComponent<Tilemap>();

        using(var writer = File.CreateText($"Assets/Gif/Super_Retro_Collection/Resources/Prefabs/Player/{_map.name}.txt"))
        {
            writer.WriteLine(ground.cellBounds.xMin);
            writer.WriteLine(ground.cellBounds.xMax);
            writer.WriteLine(ground.cellBounds.yMin);
            writer.WriteLine(ground.cellBounds.yMax);

            // ground�� 0 env�� 1�� ä���� �ؽ�Ʈ ���� ����
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

        // �� �ؽ�Ʈ ������ �� �پ� �����鼭 �� �� �ִ� ����(0)�� false
        // �� �� ���� ����(1)�� true�� �迭�� ä��
        for (int y = 0; y < yCount; y++)
        {
            string line = reader.ReadLine();
            for (int x = 0; x < xCount; x++)
            {
                _collision[y, x] = (line[x] == '1' ? true : false);
            }
        }
    }

    // �÷��̾ �� �� �ִ��� ������ �Ǻ�
    public bool CanMove(Vector3Int cellPos)
    {
        if (cellPos.x < MinX || cellPos.x > MaxX)
            return false;
        if (cellPos.y < MinY || cellPos.y > MaxY)
            return false;

        int x = cellPos.x - MinX;
        int y = MaxY - cellPos.y;
        // �� �� �ִ� ����(0, false)�� �������Ѽ� ��ȯ(CanMove�� ���̸� ������ �� �ִ� ����)
        return !_collision[y, x];
    }
}
