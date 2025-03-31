using UnityEngine;
using UnityEngine.UI;

public class HpBar : MonoBehaviour
{
    [SerializeField]
    Slider _hpBar = null;

    public void SetHpBar(float hp)
    {
        hp = Mathf.Clamp(hp, 0, 1);
        _hpBar.value = hp;
    }
}
