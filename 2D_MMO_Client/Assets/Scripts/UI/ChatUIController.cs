using UnityEngine;
using UnityEngine.UIElements;

public class ChatUIController : BaseUIController
{
    VisualElement container;
    ScrollView scrollView;
    TextField inputTextField;
    protected override void Init()
    {
        base.Init();
        VisualElement root = document.rootVisualElement;

        container = root.Q<VisualElement>("Container-Chat");
        scrollView = root.Q<ScrollView>();
        inputTextField = root.Q<TextField>();
    }
    private void Update()
    {
        if (Input.GetKeyDown(KeyCode.Return))
        {
            if (container.ClassListContains("Chat-Hide"))
            {
                container.RemoveFromClassList("Chat-Hide");
                Invoke(() =>
                {
                    inputTextField.Focus();
                });
            }
            else
            {
                string text = inputTextField.text.Trim();
                if (text.Length == 0)
                    container.AddToClassList("Chat-Hide");
                else
                {
                    PushChat(text);
                    inputTextField.value = "";
                    Invoke(() =>
                    {
                        inputTextField.Focus();
                    });
                }
            }
        }
    }
    static int i = 0;
    public void PushChat(string chat)
    {
        var chatContainer = new VisualElement();

        chatContainer.AddToClassList("Chat-TextContainer");
        var textId = new Label();
        var textChat = new Label();

        textId.AddToClassList("Chat-Text");
        textChat.AddToClassList("Chat-Text");

        chatContainer.Add(textId);
        chatContainer.Add(textChat);

        textId.text = i.ToString();
        textChat.text = chat;
        scrollView.Add(chatContainer);
        i++;
    }
}
