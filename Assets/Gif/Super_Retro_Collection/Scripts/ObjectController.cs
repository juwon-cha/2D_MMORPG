using UnityEngine;

public enum MoveDir
{
    Idle,
    Up,
    Down,
    Left,
    Right
}

public class ObjectController : MonoBehaviour
{
    public float _speed = 8.0f;
    protected bool _bIsMoving = false;

    protected Vector3Int _cellPos = Vector3Int.zero;
    public Grid _grid;

    protected Animator _animator;
    MoveDir _dir = MoveDir.Idle;
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

            switch (value)
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

                // 마지막으로 바라보는 방향 Idle
                case MoveDir.Idle:
                    if (_dir == MoveDir.Up)
                    {
                        _animator.Play("IDLE_UP");
                        transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    }
                    else if (_dir == MoveDir.Down)
                    {
                        _animator.Play("IDLE_DOWN");
                        transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    }
                    else if (_dir == MoveDir.Left)
                    {
                        _animator.Play("IDLE_RIGHT");
                        transform.localScale = new Vector3(-1.0f, 1.0f, 1.0f);
                    }
                    else // 오른쪽 IDLE
                    {
                        _animator.Play("IDLE_RIGHT");
                        transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
                    }
                    break;

                default:
                    break;
            }

            _dir = value;
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
        _animator = GetComponent<Animator>();
        Vector3 playerPos = _grid.CellToWorld(_cellPos) + new Vector3(0.5f, 0.5f, 0);
        transform.position = playerPos;
    }

    protected virtual void UpdateController()
    {
        UpdatePosition();
        UpdateMovement();
    }

    // 자연스러운 이동 처리
    void UpdatePosition()
    {
        if (_bIsMoving == false)
        {
            return;
        }

        Vector3 destPos = _grid.CellToWorld(_cellPos) + new Vector3(0.5f, 0.5f, 0);
        Vector3 moveDir = destPos - transform.position;

        // 도착 여부 체크
        float dist = moveDir.magnitude; // 방향 벡터 크기
        if (dist < _speed * Time.deltaTime)
        {
            transform.position = destPos;
            _bIsMoving = false;
        }
        else
        {
            // 자연스러운 움직임
            transform.position += moveDir.normalized * _speed * Time.deltaTime;
            _bIsMoving = true;
        }
    }

    // 실제 좌표 이동
    void UpdateMovement()
    {
        if (_bIsMoving == false)
        {
            switch (_dir)
            {
                case MoveDir.Up:
                    _cellPos += Vector3Int.up;
                    _bIsMoving = true;
                    break;

                case MoveDir.Down:
                    _cellPos += Vector3Int.down;
                    _bIsMoving = true;
                    break;

                case MoveDir.Left:
                    _cellPos += Vector3Int.left;
                    _bIsMoving = true;
                    break;

                case MoveDir.Right:
                    _cellPos += Vector3Int.right;
                    _bIsMoving = true;
                    break;

                default:
                    break;
            }
        }
    }
}
