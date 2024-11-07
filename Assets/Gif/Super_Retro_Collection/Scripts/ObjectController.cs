using System.Runtime.Serialization;
using UnityEditor.Experimental.GraphView;
using UnityEngine;
using static UnityEngine.Rendering.DebugUI;

public enum MoveDir
{
    Idle,
    Up,
    Down,
    Left,
    Right
}

public enum ObjectState
{
    Idle,
    Moving,
    Skill,
    Dead
}

public class ObjectController : MonoBehaviour
{
    public float _speed = 8.0f;
    public Vector3Int CellPos { get; set; } = Vector3Int.zero;
    protected Animator _animator;
    public ObjectManager ObjManager { get; set; }
    public GameScene GameScene {  get; set; }

    ObjectState _state = ObjectState.Idle;
    public ObjectState State
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
        }
    }

    MoveDir _dir = MoveDir.Down;
    MoveDir _lastFacingDir = MoveDir.Down;
    public MoveDir MoveDir
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
            if(value != MoveDir.Idle)
            {
                _lastFacingDir = value;
            }

            UpdateAnim();
        }
    }

    protected virtual void UpdateAnim()
    {
        if(State == ObjectState.Idle)
        {
            // 마지막으로 바라보는 방향 Idle
            switch (_lastFacingDir)
            {
                case MoveDir.Up:
                    _animator.Play("IDLE_UP");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;

                case MoveDir.Down:
                    _animator.Play("IDLE_DOWN");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;

                case MoveDir.Left:
                    _animator.Play("IDLE_RIGHT");
                    transform.localScale = new Vector3(-1.0f, 1.0f, 1.0f);
                    break;

                case MoveDir.Right:
                    _animator.Play("IDLE_RIGHT");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;

                default:
                    break;
            }
        }
        else if(State == ObjectState.Moving)
        {
            switch (_dir)
            {
                case MoveDir.Up:
                    _animator.Play("WALK_UP");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;

                case MoveDir.Down:
                    _animator.Play("WALK_DOWN");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;

                // 오른쪽 애니메이션 반전
                case MoveDir.Left:
                    _animator.Play("WALK_RIGHT");
                    transform.localScale = new Vector3(-1.0f, 1.0f, 1.0f);
                    break;

                case MoveDir.Right:
                    _animator.Play("WALK_RIGHT");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;

                default:
                    break;
            }
        }
        else if(State == ObjectState.Skill)
        {
            // TODO: skill anim
            // 마지막으로 바라본 방향 기준으로 스킬 시전
            switch (_lastFacingDir)
            {
                case MoveDir.Up:
                    _animator.Play("ATTACK_UP");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;

                case MoveDir.Down:
                    _animator.Play("ATTACK_DOWN");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;

                // 오른쪽 애니메이션 반전
                case MoveDir.Left:
                    _animator.Play("ATTACK_RIGHT");
                    transform.localScale = new Vector3(-1.0f, 1.0f, 1.0f);
                    break;

                case MoveDir.Right:
                    _animator.Play("ATTACK_RIGHT");
                    transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    break;

                default:
                    break;
            }
        }
        else if(State == ObjectState.Dead)
        {
            // TODO: Death anim
        }
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
        GameObject obj = GameObject.Find("GameScene");
        GameScene = obj.GetComponent<GameScene>();
        ObjManager = obj.GetComponent<ObjectManager>();

        _animator = GetComponent<Animator>();
        Vector3 position = GameScene.CurrentGrid.CellToWorld(CellPos) + new Vector3(0.5f, 0.5f, 0);
        transform.position = position;
    }

    protected virtual void UpdateController()
    {
        switch(State)
        {
            case ObjectState.Idle:
                UpdateCoordinates();
                break;

            case ObjectState.Moving:
                UpdateMovement();
                break;

            case ObjectState.Skill:
                break;

            case ObjectState.Dead:
                break;

            default:
                break;
        }  
    }

    // 실제 좌표 이동
    protected virtual void UpdateCoordinates()
    {
        if (State == ObjectState.Idle && _dir != MoveDir.Idle)
        {
            Vector3Int destPos = CellPos;

            switch (_dir)
            {
                case MoveDir.Up:
                    destPos += Vector3Int.up;
                    break;

                case MoveDir.Down:
                    destPos += Vector3Int.down;
                    break;

                case MoveDir.Left:
                    destPos += Vector3Int.left;
                    break;

                case MoveDir.Right:
                    destPos += Vector3Int.right;
                    break;

                default:
                    break;
            }

            State = ObjectState.Moving;

            if (GameScene.CanMove(destPos))
            {
                // 객체가 없으면 이동 가능
                if (ObjManager.Find(destPos) == null)
                {
                    CellPos = destPos;
                }
            }
        }
    }

    // 자연스러운 이동 처리
    protected virtual void UpdateMovement()
    {
        Vector3 destPos = GameScene.CurrentGrid.CellToWorld(CellPos) + new Vector3(0.5f, 0.5f, 0);
        Vector3 moveDir = destPos - transform.position;

        // 도착 여부 체크
        float dist = moveDir.magnitude; // 방향 벡터 크기
        if (dist < _speed * Time.deltaTime)
        {
            transform.position = destPos;

            // 애니메이션 직접 컨트롤
            _state = ObjectState.Idle;
            if(_dir == MoveDir.Idle)
            {
                UpdateAnim();
            }
        }
        else
        {
            // 자연스러운 움직임
            transform.position += moveDir.normalized * _speed * Time.deltaTime;
            State = ObjectState.Moving;
        }
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
            case MoveDir.Up:
                curCellPos += Vector3Int.up;
                break;

            case MoveDir.Down:
                curCellPos += Vector3Int.down;
                break;

            case MoveDir.Left:
                curCellPos += Vector3Int.left;
                break;

            case MoveDir.Right:
                curCellPos += Vector3Int.right;
                break;

            default:
                break;
        }

        return curCellPos;
    }
}
