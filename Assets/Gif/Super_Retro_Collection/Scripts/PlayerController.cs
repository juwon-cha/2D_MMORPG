using UnityEditor.Experimental.GraphView;
using UnityEngine;

public enum MoveDir
{
    Idle,
    Up,
    Down,
    Left,
    Right
}

public class PlayerController : MonoBehaviour
{
    public float _speed = 8.0f;
    MoveDir _dir = MoveDir.Idle;
    bool _bIsMoving = false;

    Vector3Int _cellPos = Vector3Int.zero;
    public Grid _grid;

    void Start()
    {
        Vector3 playerPos = _grid.CellToWorld(_cellPos) + new Vector3(0.5f, 0.5f, 0);
        transform.position = playerPos;
    }
    
    void Update()
    {
        InputDirection();
        UpdatePosition();
        UpdateMovement();
    }

    // 방향 설정
    void InputDirection()
    {
        if (Input.GetKey(KeyCode.UpArrow))
        {
            _dir = MoveDir.Up;
        }
        else if (Input.GetKey(KeyCode.DownArrow))
        {
            _dir = MoveDir.Down;
        }
        else if (Input.GetKey(KeyCode.LeftArrow))
        {
            _dir = MoveDir.Left;
        }
        else if (Input.GetKey(KeyCode.RightArrow))
        {
            _dir = MoveDir.Right;
        }
        else
        {
            _dir = MoveDir.Idle;
        }
    }

    // 자연스러운 이동 처리
    void UpdatePosition()
    {
        if(_bIsMoving == false)
        {
            return;
        }

        Vector3 destPos = _grid.CellToWorld(_cellPos) + new Vector3(0.5f, 0.5f, 0);
        Vector3 moveDir = destPos - transform.position;

        // 도착 여부 체크
        float dist = moveDir.magnitude; // 방향 벡터 크기
        if(dist < _speed * Time.deltaTime)
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
