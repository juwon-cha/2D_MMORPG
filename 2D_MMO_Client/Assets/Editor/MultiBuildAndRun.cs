using UnityEditor;
using UnityEngine;

public class MultiBuildAndRun
{
    [MenuItem("Tools/Multiplayer/2 Players")]
    static void BuildAndRun2Players()
    {
        BuildAndRunWin64(2);
    }

    [MenuItem("Tools/Multiplayer/3 Players")]
    static void BuildAndRun3Players()
    {
        BuildAndRunWin64(3);
    }

    static void BuildAndRunWin64(int count)
    {
        Application.runInBackground = true;
        EditorUserBuildSettings.SwitchActiveBuildTarget(BuildTargetGroup.Standalone, BuildTarget.StandaloneWindows);

        for(int i = 1; i <= count; ++i)
        {
            BuildPipeline.BuildPlayer(
                GetScenePaths()
                , "Build/win64/" + GetProjectName() + i.ToString() + "/" + GetProjectName() + i.ToString() + ".exe"
                , BuildTarget.StandaloneWindows64
                , BuildOptions.AutoRunPlayer);
        }
    }

    static string GetProjectName()
    {
        string[] name = Application.dataPath.Split('/');
        return name[name.Length - 2];
    }

    static string[] GetScenePaths()
    {
        string[] scenes = new string[EditorBuildSettings.scenes.Length];

        for(int i = 0; i < scenes.Length; ++i)
        {
            scenes[i] = EditorBuildSettings.scenes[i].path;
        }

        return scenes;
    }
}
