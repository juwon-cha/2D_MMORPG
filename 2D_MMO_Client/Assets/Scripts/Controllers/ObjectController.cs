using Google.FlatBuffers;
using UnityEngine;

public class ObjectController : MonoBehaviour
{
    public int Id { get; set; }
    public int SkillId { get; set; }
    public float Speed { get; set; }
    public int MaxHP { get; set; }

    int _hp;
    public int HP
    {
        get { return _hp; }
        set
        {
            _hp = value;
            UpdateHpBar();
        }
    }

    protected bool _updated = false;

    private HpBar _hpBar;

    protected Vector3Int _cellPos = new Vector3Int();
    public Vector3Int CellPos
    {
        get { return _cellPos; }
        set
        {
            if (_cellPos == value)
            {
                return;
            }

            _cellPos.x = value.x;
            _cellPos.y = value.y;
            _cellPos.z = 0;
            _updated = true;
        }
    }

    protected Animator _animator;
    // localscale로 캐릭터를 뒤집으면 UI도 뒤집혀서 캐릭터의 sprite만 flip시킴
    protected SpriteRenderer _sprite;

    protected Define.ObjectState _state = Define.ObjectState.Idle;
    public virtual Define.ObjectState State
    {
        get
        {
            return _state;
        }
        set
        {
            if (_state == value)
            {
                return;
            }

            _state = value;
            UpdateAnim();
            _updated = true;
        }
    }

    protected Define.MoveDir _dir = Define.MoveDir.Down;
    public Define.MoveDir MoveDir
    {
        get
        {
            return _dir;
        }
        set
        {
            if (_dir == value)
            {
                return;
            }

            _dir = value;

            UpdateAnim();
            _updated = true;
        }
    }

    void Awake()
    {
        Application.runInBackground = true;
    }

    void Start()
    {
        Init();
    }

    void Update()
    {
        UpdateController();
    }

    protected virtual void Init()
    {
        _animator = GetComponent<Animator>();
        _sprite = GetComponent<SpriteRenderer>();
        Vector3 position = Manager.Map.CurrentGrid.CellToWorld(CellPos) + new Vector3(0.5f, 0.5f, 0);
        transform.position = position;

        State = Define.ObjectState.Idle;
        MoveDir = Define.MoveDir.Down;
        UpdateAnim();
    }

    protected virtual void UpdateAnim()
    {

    }

    protected virtual void UpdateController()
    {
        switch (State)
        {
            case Define.ObjectState.Idle:
                UpdateIdle();
                break;

            case Define.ObjectState.Moving:
                UpdateMovement();
                break;

            case Define.ObjectState.Skill:
                UpdateSkill();
                break;

            case Define.ObjectState.Dead:
                UpdateDead();
                break;

            default:
                break;
        }
    }

    // 실제 좌표 이동
    protected virtual void UpdateIdle()
    {

    }

    public void SyncPos()
    {
        Vector3 destPos = Manager.Map.CurrentGrid.CellToWorld(CellPos) + new Vector3(0.5f, 0.5f, 0);
        transform.position = destPos;
    }

    // 자연스러운 이동 처리
    protected virtual void UpdateMovement()
    {
        Vector3 destPos = Manager.Map.CurrentGrid.CellToWorld(CellPos) + new Vector3(0.5f, 0.5f, 0);
        Vector3 moveDir = destPos - transform.position;

        // 도착 여부 체크
        float dist = moveDir.magnitude; // 방향 벡터 크기
        if (dist < Speed * Time.deltaTime)
        {
            transform.position = destPos;
            UpdateCoordinates();
        }
        else
        {
            // 자연스러운 움직임
            transform.position += moveDir.normalized * Speed * Time.deltaTime;
            State = Define.ObjectState.Moving;
        }
    }

    protected virtual void UpdateCoordinates()
    {

    }

    protected virtual void UpdateSkill()
    {

    }

    protected virtual void UpdateDead()
    {

    }

    public Vector3Int GetFacingCellPostition()
    {
        Vector3Int curCellPos = CellPos;

        switch (MoveDir)
        {
            case Define.MoveDir.Up:
                curCellPos += Vector3Int.up;
                break;

            case Define.MoveDir.Down:
                curCellPos += Vector3Int.down;
                break;

            case Define.MoveDir.Left:
                curCellPos += Vector3Int.left;
                break;

            case Define.MoveDir.Right:
                curCellPos += Vector3Int.right;
                break;

            default:
                break;
        }

        return curCellPos;
    }

    public virtual void UseSkill(int skillId)
    {

    }

    public virtual void OnDead()
    {
        State = Define.ObjectState.Dead;

        UpdateAnim();
    }

    protected void AddHpBar()
    {
        GameObject HpBarOriginal = Resources.Load<GameObject>("Prefabs/UI/HP_Bar");
        GameObject HpBar = UnityEngine.Object.Instantiate(HpBarOriginal, transform);
        HpBar.transform.localPosition = new Vector3(0, 0.8f, 1);
        HpBar.name = "HpBar";
        _hpBar = HpBar.GetComponent<HpBar>();

        UpdateHpBar();
    }

    void UpdateHpBar()
    {
        if (_hpBar == null)
        {
            return;
        }

        float hp = 0.0f;
        if (MaxHP > 0)
        {
            hp = ((float)HP / MaxHP);
        }

        _hpBar.SetHpBar(hp);
    }
}
