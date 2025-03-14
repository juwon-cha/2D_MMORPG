using Google.FlatBuffers;
using ServerCore;
using UnityEngine;

public partial class PacketHandler
{
    static void HandleLoginError(bool success)
    {
        if (success)
        {
            Manager.Scene.LoadSceneAsync("GameScene", () =>
            {
                FlatBufferBuilder builder = new FlatBufferBuilder(1);
                C_ENTER_GAME.StartC_ENTER_GAME(builder);
                var data = C_ENTER_GAME.EndC_ENTER_GAME(builder);
                var packet = Manager.Packet.CreatePacket(data, builder, PacketType.C_ENTER_GAME);
                Manager.Network.Send(packet);
            });
        }
        else
        {
            var lc = GameObject.Find("UIDocument").GetComponent<LoginUIController>();
            lc.ErrorPopup.RemoveFromClassList("Hide-ErrorPopup");
            lc.ErrorResult.text = "Failed";
        }
    }
    public static void SC_SIGNUPHandler(PacketSession session, ByteBuffer buffer)
    {
        var pkt = SC_SIGNUP.GetRootAsSC_SIGNUP(buffer);

        switch (pkt.Ok)
        {
            case SignUpError.OVERLAP_ID:
            case SignUpError.UNKNOWN:
                HandleLoginError(false);
                break;
            case SignUpError.SUCCESS:
                HandleLoginError(true);
                break;
        }
    }
    public static void SC_SIGNINHandler(PacketSession session, ByteBuffer buffer)
    {
        var pkt = SC_SIGNIN.GetRootAsSC_SIGNIN(buffer);

        switch (pkt.Ok)
        {
            case SignInError.INVALID_ID:
            case SignInError.INVALID_PASSWORD:
            case SignInError.UNKNOWN:
                HandleLoginError(false);
                break;
            case SignInError.SUCCESS:
                HandleLoginError(true);
                break;
        }
    }
}
