using UnityEngine;

public class CameraController : MonoBehaviour
{
    Transform _playerTransform;

    Vector2 _center;

    float _cameraHeight;
    float _cameraWidth;
    [SerializeField]
    float CameraMoveSpeed = 1f;

    void Start()
    {
        _playerTransform = GameObject.Find("Player").GetComponent<Transform>();

        _cameraHeight = Camera.main.orthographicSize;
        _cameraWidth = _cameraHeight * Screen.width / Screen.height;
    }

    void LateUpdate()
    {
        //Camera.main.transform.position = new Vector3(_playerTransform.position.x, _playerTransform.position.y, -10);

        // 부드러운 카메라 움직임
        Camera.main.transform.position = Vector3.Lerp(Camera.main.transform.position, _playerTransform.position, Time.deltaTime * CameraMoveSpeed);

        RestrictCameraMovement();
    }

    void RestrictCameraMovement()
    {
        // 카메라 이동 제한
        float lx = Manager.Map.SizeX / 2 - _cameraWidth;
        float clampX = Mathf.Clamp(Camera.main.transform.position.x, -lx, lx - 1);

        float ly = Manager.Map.SizeY / 2 - _cameraHeight;
        float clampY = Mathf.Clamp(Camera.main.transform.position.y, -ly, ly - 1);

        Camera.main.transform.position = new Vector3(clampX, clampY, -10f);
    }
}
