using Define;
using System;
using UnityEngine;
using UnityEngine.SceneManagement;

public class SceneManagerEx
{
    public BaseScene CurScene { get { return GameObject.FindAnyObjectByType<BaseScene>(); } }

    public void LoadScene(string sceneName)
    {
        SceneManager.LoadScene(sceneName);
    }

    public void LoadScene(int sceneId)
    {
        SceneManager.LoadScene(sceneId);
    }

    public void LoadScene(Define.SceneType sceneType)
    {
        Manager.Clear();

        SceneManager.LoadScene(GetSceneName(sceneType));
    }
    public void LoadSceneAsync(string sceneName, Action action = null)
    {
        SceneManager.LoadSceneAsync(sceneName).completed += (a) => { action?.Invoke(); };
    }
    string GetSceneName(Define.SceneType sceneType)
    {
        string sceneName = System.Enum.GetName(typeof(Define.SceneType), sceneType);

        return sceneName;
    }

    public void Clear()
    {
        CurScene.Clear();
    }
}
