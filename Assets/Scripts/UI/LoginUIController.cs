using UnityEngine;
using UnityEngine.UIElements;
public class LoginUIController : BaseUIController
{
    VisualElement root;
    VisualElement Popup;

    bool _signIn = false;
    bool signIn
    {
        get { return _signIn; }
        set
        {
            _signIn = value;
            if (value == false)
            {
                PopupTitle.text = "����";
                PopupSubmit.text = "ȸ������";
            }
            else
            {
                PopupTitle.text = "�α���";
                PopupSubmit.text = "�α���";
            }
            Popup.RemoveFromClassList("Hide-Popup");
        }
    }

    Button SignUp;
    Button SignIn;
    Button GameExit;

    Button PopupSubmit;
    Button PopupClose;
    Label PopupTitle;
    TextField id;
    TextField password;
    protected override void Init()
    {
        base.Init();
        root = document.rootVisualElement;
        Popup = root.Q<VisualElement>("Container-PopUp");

        SignIn = root.Q<Button>("Button-SignIn");
        SignIn.RegisterCallback<ClickEvent>((e) => { signIn = true; });
        SignUp = root.Q<Button>("Button-SignUp");
        SignUp.RegisterCallback<ClickEvent>((e) => { signIn = false; });
        GameExit = root.Q<Button>("Button-GameExit");
        GameExit.RegisterCallback<ClickEvent>((e) => {
#if UNITY_EDITOR
            UnityEditor.EditorApplication.isPlaying = false;
#else
        Application.Quit(); // ���ø����̼� ����
#endif
        });

        PopupSubmit = root.Q<Button>("Button-PopupSubmit");
        PopupSubmit.RegisterCallback<ClickEvent>((e) => {
            // todo
        });
        PopupTitle = root.Q<Label>("Text-PopupTitle");
        PopupClose = root.Q<Button>("Button-PopupClose");
        PopupClose.RegisterCallback<ClickEvent>((e) => { Popup.AddToClassList("Hide-Popup"); });
        id = root.Q<TextField>("TextField-ID");
        password = root.Q<TextField>("TextField-Password");
    }
}
