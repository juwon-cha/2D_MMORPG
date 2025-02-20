using Google.FlatBuffers;
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
                    FlatBufferBuilder builder = new FlatBufferBuilder(1);
                    var chatText = builder.CreateString(text);
                    var data = C_Chat.CreateC_Chat(builder, chatText);
                    var pkt = Manager.Packet.CreatePacket(data, builder, PacketType.C_Chat);
                    Manager.Network.Send(pkt);
                    //PushChat(text);
                    inputTextField.value = "";
                    Invoke(() =>
                    {
                        inputTextField.Focus();
                    });
                }
            }
        }
    }
    public void PushChat(int id, string text)
    {
        var chatContainer = new VisualElement();

        chatContainer.AddToClassList("Chat-TextContainer");
        var textId = new Label();
        var textChat = new Label();

        textId.AddToClassList("Chat-Text");
        textChat.AddToClassList("Chat-Text");

        chatContainer.Add(textId);
        chatContainer.Add(textChat);

        textId.text = id.ToString();
        textChat.text = text;
        scrollView.Add(chatContainer);
    }
}
