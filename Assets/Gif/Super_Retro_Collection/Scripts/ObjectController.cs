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
            // ���������� �ٶ󺸴� ���� Idle
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

                // ������ �ִϸ��̼� ����
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
            // ���������� �ٶ� ���� �������� ��ų ����
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

                // ������ �ִϸ��̼� ����
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

    // ���� ��ǥ �̵�
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
                // ��ü�� ������ �̵� ����
                if (ObjManager.Find(destPos) == null)
                {
                    CellPos = destPos;
                }
            }
        }
    }

    // �ڿ������� �̵� ó��
    protected virtual void UpdateMovement()
    {
        Vector3 destPos = GameScene.CurrentGrid.CellToWorld(CellPos) + new Vector3(0.5f, 0.5f, 0);
        Vector3 moveDir = destPos - transform.position;

        // ���� ���� üũ
        float dist = moveDir.magnitude; // ���� ���� ũ��
        if (dist < _speed * Time.deltaTime)
        {
            transform.position = destPos;

            // �ִϸ��̼� ���� ��Ʈ��
            _state = ObjectState.Idle;
            if(_dir == MoveDir.Idle)
            {
                UpdateAnim();
            }
        }
        else
        {
            // �ڿ������� ������
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
