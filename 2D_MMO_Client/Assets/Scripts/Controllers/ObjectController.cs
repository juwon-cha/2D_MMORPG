using Google.FlatBuffers;
using UnityEngine;

public class ObjectController : MonoBehaviour
{
    public int Id { get; set; }

    public float _speed = 8.0f;

    protected bool _updated = false;

    protected Vector3Int _cellPos = new Vector3Int();
    public Vector3Int CellPos
    {
        get { return _cellPos; }
        set
        {
            if(_cellPos == value)
            {
                return;
            }

            _cellPos.x = value.x;
            _cellPos.y = value.y;
            _cellPos.z = 0;
            UpdateAnim();
            _updated = true;
        }
    }
    protected Animator _animator;

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

    protected Define.MoveDir _dir = Define.MoveDir.None;
    protected Define.MoveDir _lastFacingDir = Define.MoveDir.Down;
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
            if(value != Define.MoveDir.None)
            {
                _lastFacingDir = value;
            }

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
        Vector3 position = Manager.Map.CurrentGrid.CellToWorld(CellPos) + new Vector3(0.5f, 0.5f, 0);
        transform.position = position;

        State = Define.ObjectState.Idle;
        MoveDir = Define.MoveDir.None;
        CellPos = new Vector3Int(0, 0, 0);
        UpdateAnim();
    }

    protected virtual void UpdateAnim()
    {
        if (State == Define.ObjectState.Idle)
        {
            // 마지막으로 바라보는 방향 Idle
            switch (_lastFacingDir)
            {
                case Define.MoveDir.Up:
                    _animator.Play("IDLE_UP");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;

                case Define.MoveDir.Down:
                    _animator.Play("IDLE_DOWN");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;

                case Define.MoveDir.Left:
                    _animator.Play("IDLE_RIGHT");
                    transform.localScale = new Vector3(-1.0f, 1.0f, 1.0f);
                    break;

                case Define.MoveDir.Right:
                    _animator.Play("IDLE_RIGHT");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;

                default:
                    break;
            }
        }
        else if (State == Define.ObjectState.Moving)
        {
            switch (_dir)
            {
                case Define.MoveDir.Up:
                    _animator.Play("WALK_UP");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;

                case Define.MoveDir.Down:
                    _animator.Play("WALK_DOWN");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;

                // 오른쪽 애니메이션 반전
                case Define.MoveDir.Left:
                    _animator.Play("WALK_RIGHT");
                    transform.localScale = new Vector3(-1.0f, 1.0f, 1.0f);
                    break;

                case Define.MoveDir.Right:
                    _animator.Play("WALK_RIGHT");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;

                default:
                    break;
            }
        }
        else if (State == Define.ObjectState.Skill)
        {
            // TODO: skill anim
            // 마지막으로 바라본 방향 기준으로 스킬 시전
            switch (_lastFacingDir)
            {
                case Define.MoveDir.Up:
                    _animator.Play("ATTACK_UP");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;

                case Define.MoveDir.Down:
                    _animator.Play("ATTACK_DOWN");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;

                // 오른쪽 애니메이션 반전
                case Define.MoveDir.Left:
                    _animator.Play("ATTACK_RIGHT");
                    transform.localScale = new Vector3(-1.0f, 1.0f, 1.0f);
                    break;

                case Define.MoveDir.Right:
                    _animator.Play("ATTACK_RIGHT");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;

                default:
                    break;
            }
        }
        else if (State == Define.ObjectState.Dead)
        {
            // TODO: Death anim
        }
    }

    protected virtual void UpdateController()
    {
        switch(State)
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

    protected virtual void UpdateIdle()
    {

    }

    // 자연스러운 이동 처리
    protected virtual void UpdateMovement()
    {
        Vector3 destPos = Manager.Map.CurrentGrid.CellToWorld(CellPos) + new Vector3(0.5f, 0.5f, 0);
        Vector3 moveDir = destPos - transform.position;

        // 도착 여부 체크
        float dist = moveDir.magnitude; // 방향 벡터 크기
        if (dist < _speed * Time.deltaTime)
        {
            transform.position = destPos;
            UpdateCoordinates();
        }
        else
        {
            // 자연스러운 움직임
            transform.position += moveDir.normalized * _speed * Time.deltaTime;
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

        switch (_lastFacingDir)
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

    public virtual void OnDamaged() { }
}
