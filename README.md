# 2D_MMORPG
## <a id="toc"></a>목차

1. [개요](#1-개요)
2. [일정](#2-일정)
3. [모듈 구성도](#3-모듈-구성도)
4. [플레이 영상](#4-플레이-영상)
5. [기능](#5-기능)
6. [툴](#6-툴)
7. [오류와 문제](#7-오류와-문제)

---

## 1. 개요

* 프로젝트 주제: 2D Top-down MMORPG
* 프로젝트 목적: 소규모 2D MMORPG 게임 클라이언트 및 서버 시스템을 개발하여 MMORPG의 기능들(플레이어 입장, 이동, 전투, 아이템 착용 등)을 구현하는 것을 목표
* 개발 인원: 2명(김예찬, 차주원)
* 기술적 요구사항
  - 클라이언트: Unity
      + 에셋: 유니티 에셋 스토어
        - [2D RPG topdown tilesets - pixelart assets FULL BUNDLE (GIF)](https://assetstore.unity.com/packages/2d/environments/2d-rpg-topdown-tilesets-pixelart-assets-full-bundle-212921?srsltid=AfmBOopKUJeEHbA1DxKX5pMVisUIUUxkSA92mx3TKyl_un04bszOdwrW)
        - [GUI – Casual Fantasy (Layer Lab)](https://assetstore.unity.com/packages/2d/gui/gui-casual-fantasy-265651?srsltid=AfmBOopPzixrj3RTw8TtNb-8dhKecO-QPc5pTIwLG3YZ7aMY1gQ7tPGp)
      + UI: UI toolkit, UGUI
  - 서버
      + 게임 서버(C++ IOCP 모델)
      + DB 서버
  - DB: MySQL
  - 프로토콜 직렬화 도구: FlatBuffers
  - 통신 프로토콜: TCP/IP
  - 버전 관리: Git / Github

[BackToTop](#toc)

---

## 2. 일정

![2D_MMORPG_SCHEDULE](https://github.com/user-attachments/assets/62b97d73-73b8-4099-b71a-a1ae680a9ae4)

[BackToTop](#toc)

---

## 3. 모듈 구성도

![README 모듈 구성도](https://github.com/user-attachments/assets/879b7cc8-80b2-4d19-9422-cf5d898807d2)

[BackToTop](#toc)

---

## 4. 플레이 영상

<div align="center">

  <a href="https://www.youtube.com/watch?v=Rj2cyzisgQY">
    <img src="https://img.youtube.com/vi/Rj2cyzisgQY/maxresdefault.jpg" width="600" alt="첫 번째 유튜브 영상 썸네일">
  </a>

  <br>
  <br>

  <a href="https://www.youtube.com/watch?v=UeihCh27v1s">
    <img src="https://img.youtube.com/vi/UeihCh27v1s/maxresdefault.jpg" width="600" alt="두 번째 유튜브 영상 썸네일">
  </a>

</div>

[BackToTop](#toc)

---

## 5. 기능

* 플레이어 입장/퇴장
* 이동
  - 플레이어
  - 몬스터 패트롤
  - 몬스터가 플레이어 추적
* 전투
  - 일반 공격
  - 화살 공격
  - 몬스터 공격
  - 피격
  - 체력 변화
  - 리스폰
* 아이템
* 맵 이동

[BackToTop](#toc)

---

## 6. 툴

* 맵툴
  - 클라이언트에서 제작한 맵을 서버에서도 맵 정보를 알기 위해 맵을 텍스트 파일로 변환하는 툴. 플레이어가 갈 수 있는 좌표는 0, 갈 수 없는 좌표는 1로 표시한다.
 
* 멀티 빌드
  - 유니티 에디터에서 멀티플레이를 테스트를 위한 빌드 툴

[BackToTop](#toc)

---

## 7. 오류와 문제

* 멀티플레이 테스트 시 백그라운드에서 동기화가 안되는 문제
  - 멀티 빌드 시 백그라운드 플레이가 되게끔 변경

 ```cs
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
```

<br>

* 에디터 모드 종료 후 바로 다시 플레이하면 서버에 플레이어 오브젝트 남아있는 문제
  - 에디터 모드에서 ‘esc’키를 누르면 명시적으로 서버와의 연결을 끊고 에디터 플레이 모드 종료하게끔 변경

<br>

* 플레이어가 화살 발사 후 화살을 실시간 처리하는 과정에서 크래시
  - 화살을 발사하면 조건(오브젝트와 충돌, 공격 범위 벗어남)에 따라 화살들이 삭제되는데 가장 먼저 map에 들어간 map의 앞쪽부터 화살 객체들을 삭제하고 있다.
  - 그런데 투사체 Update 함수에서 아직 삭제되지 않은 화살들의 좌표나 충돌을 업데이트해야 하는데 map의 앞쪽부터 순회하면 이미 삭제된 화살에 접근하기 때문에 에러 발생.
  - 화살이 많지 않다고 판단하여 map의 뒤쪽부터 순회하면서 화살을 업데이트.
  - 가장 마지막에 생성된 화살부터 실시간 처리하기 때문에 가끔씩 몬스터와 충돌하지 않는 화살이 있다.

<br>

* 연속으로 공격하거나 화살을 발사하면 서버에서 락 타임아웃이 걸리는 문제
  - 서버에서 몬스터의 이동 및 공격, 화살과 같은 투사체 오브젝트 처리 등 실시간 로직을 다룰 때 각 함수마다 락을 걸어 동시성 문제를 해결하려 했으나, 이후 서버에서 락 타임아웃이 발생하는 문제 발생
  - 문제 해결을 위해 잡 큐 시스템을 도입
  - 실시간으로 처리해야 할 작업들을 서버가 직접 즉시 실행하는 대신 작업들을 우선 큐에 저장해두고 여유가 생겼을 때 큐에서 작업을 하나씩 꺼내 실행하는 방식으로 구조를 변경

<br>

* 맵 이동 시 플레이어 ID가 바뀌는 문제
  - 맵의 개수 만큼 게임 룸을 생성(맵 아이디와 룸 아이디 동일)했다. 플레이어가 맵을 이동하면 이동하려는 맵의 아이디를 이용해 다른 룸으로 접속. 이 과정에서 기존의 플레이어 아이디가 삭제되고 새로운 아이디가 부여되면서 맵 이동이 되는 문제가 발생
  - 기존에 클라이언트에서 게임에 입장할 때마다 새로운 아이디를 부여하도록 설정하였다. 이미 게임에 플레이어 객체가 존재한 상태에서 게임에 입장하는 상황이라면 새로운 아이디를 부여하지 않고 기존 플레이어의 정보를 유지하면서 새로운 맵으로 이동하게끔 변경

```cs
    public static void SC_ENTER_GAMEHandler(PacketSession session, ByteBuffer buffer)
    {
        var enter = SC_ENTER_GAME.GetRootAsSC_ENTER_GAME(buffer);
        ObjectInfo info = enter.Player.Value;

        // 오브젝트 매니저에서 캐릭터 게임 오브젝트 생성
        // 문제 -> ObjectManager 데이터 중복 -> Dictionary에 이미 존재하는 Key값으로 또 다른 아이템을 추가하려고 시도했기 때문
        // 맵 전환 시 MyPlayer는 이미 존재하고 ObjectManager.Clear()에 의해 파괴되지도 않음
        // 따라서 새로 생성하면 안 되고, 기존 MyPlayer 객체의 정보(위치, 상태 등)를 갱신(Update) 필요
        //Manager.Object.Add(enter.Player.Value, myPlayer: true);
        //Debug.Log("SC_ENTER_GAMEHandler");
        //Debug.Log($"ID: {enter.Player.Value.ObjectId}, NAME: {enter.Player.Value.Name.ToString()}");

        // MyPlayer 객체가 이미 존재하는지 (맵 전환 상황인지) 확인합니다.
        if (Manager.Object.MyPlayer != null)
        {
            // 1. 맵 전환
            // MyPlayer가 이미 존재하므로, 새로 생성하지 않고 상태만 갱신
            Debug.Log("SC_ENTER_GAME: Map Change. Updating MyPlayer state.");

            MyPlayerController myPlayer = Manager.Object.MyPlayer;

            // ObjectManager.Add()의 초기화 로직을 참고하여 상태를 갱신
            myPlayer.Id = info.ObjectId; // ID는 동일하겠지만, 명확성을 위해 갱신
            myPlayer.CellPos = new Vector3Int(info.PosInfo.Value.PosX, info.PosInfo.Value.PosY, 0);
            myPlayer.State = Define.ObjectState.Idle; // 맵 전환 후에는 Idle 상태로 시작
            myPlayer.MoveDir = (Define.MoveDir)info.PosInfo.Value.MoveDir; // myPlayer.MoveDir는 서버의 PosInfo에 따라 설정

            // 체력 등 다른 스탯 정보도 갱신
            myPlayer.HP = info.StatInfo.Value.Hp;
            myPlayer.MaxHP = info.StatInfo.Value.MaxHp;
            myPlayer.Speed = info.StatInfo.Value.Speed;

            // 갱신된 위치로 캐릭터를 즉시 이동
            myPlayer.SyncPos();
        }
        else
        {
            // 2. 최초 접속
            // MyPlayer가 없으므로, 새로 생성(기존 로직과 동일)
            Debug.Log("SC_ENTER_GAME: First entry. Creating new MyPlayer.");
            Manager.Object.Add(info, myPlayer: true);
        }
    }
```

<br>

* 게임을 시작하고 2-3분 내로 클라이언트가 서버와 연결이 끊어지는 문제
  - 와이어샤크를 통해 클라이언트의 RST 패킷 확인
  - AI를 활용해 문제 해결 시도
  - 소켓 설정을 TCP-Alive로 변경해도 문제 해결 안됨
  - 클라이언트에서 서버로 주기적으로 하트비트 패킷 보내서 서버와 연결 유지하려고 했지만 문제 해결 안됨
  - AI가 의심하는 문제
    1. 가비지 컬렉션(GC)과 네트워크 버퍼 문제(C#의 GC가 비동기 작업에 사용 중인 버퍼를 잘못 건드려 발생하는 문제)
    2. 비동기 콜백에서의 처리되지 않은 예외(OnSendCompleted, OnRecvCompleted 등에서 발생한 예외가 조용히 스레드를 종료시키고 소켓 상태를 불안정하게 만드는 경우)
    3. 게임 서버의 비정상적인 세션 관리(특정 상황에서 세션 정보가 꼬여 서버가 해당 세션을 유효하지 않다고 판단하고 정리)

[BackToTop](#toc)
