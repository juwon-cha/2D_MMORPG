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

* 로그인/회원가입
* 플레이어 입장/퇴장
* 이동
  - 몬스터 패트롤
    1. 1초 마다 몬스터가 현재 좌표에서 일정 범위의 랜덤 좌표로 갈 수 있는지 경로를 탐색
    2. 갈 수 있는 좌표라면 몬스터의 상태를 이동 상태로 변경
    3. 이동 상태로 바뀐 몬스터는 이동 후 이동 정보를 같은 룸 안의 다른 플레이어들에게 패킷을 전송한다.
  - 몬스터가 플레이어 추적
    1. 몬스터가 타겟을 찾을 수 있는 범위 안에서 1초 마다 현재 게임 룸에서 플레이어를 찾는다.
    2. 타겟을 못 찾았거나 타겟이 다른 룸에 있다면 그 자리에서 패트롤 한다.
    3. 타겟을 정상적으로 찾았다면 몬스터가 타겟을 쫓아간다.(이동상태로 바뀜)
    4. 만약 타겟이 일정 거리 이상으로 벌어지면 몬스터는 쫓아가는 것을 포기하고 그 자리에서 패트롤한다.
  - 플레이어 이동
      1.	클라이언트에서 방향키 입력해서 좌표 변경
      2.	C_MOVE 패킷(오브젝트 좌표, 방향, 상태)을 서버로 전송
      3.	서버에서 오브젝트 정보(좌표, 방향, 상태) 저장 및 좌표 검증
      4.	SC_MOVE(오브젝트 아이디, 이동정보) 패킷을 현재 게임 룸에 있는 다른 플레이어들에게 브로드캐스트
      5.	클라이언트에서 SC_MOVE로 부터 받은 오브젝트 아이디로 게임 오브젝트 찾고 클라이언트에서 이동 정보 갱신
   
```cpp
void GameRoom::HandleMove(shared_ptr<Player> player, const C_MOVE* movePkt)
{
    if (player == nullptr)
    {
        return;
    }

    WRITE_LOCK;
    {
        // 클라이언트에서 받은 플레이어 정보 저장
        player->SetObjectInfo(player->GetObjectId(), player->GetObjectName());
        player->SetPosInfo(player->GetObjectPosX(), player->GetObjectPosY(), movePkt->posInfo()->state(), movePkt->posInfo()->moveDir());

        // 이동 검증
        // 다른 좌표로 이동할 경우, 갈 수 있는지 체크
        if (movePkt->posInfo()->posX() != player->GetObjectPosX() || movePkt->posInfo()->posY() != player->GetObjectPosY())
        {
            if (_map->CanGo(Vector2Int(movePkt->posInfo()->posX(), movePkt->posInfo()->posY())) == false)
            {
                return;
            }
        }

        // 서버에서 좌표 저장(이동)
        _map->ApplyMove(player, Vector2Int(movePkt->posInfo()->posX(), movePkt->posInfo()->posY()));

        flatbuffers::FlatBufferBuilder builder;
        auto name = builder.CreateString(player->GetObjectName());
        auto posInfo = CreatePositionInfo(builder, movePkt->posInfo()->state(), movePkt->posInfo()->moveDir(), movePkt->posInfo()->posX(), movePkt->posInfo()->posY());

        // 다른 플레이어들에게 알려줌
        auto move = CreateSC_MOVE(builder, player->GetObjectId(), posInfo);
        auto respondMovePkt = PacketManager::Instance().CreatePacket(move, builder, PacketType_SC_MOVE);

        Broadcast(respondMovePkt);
    }
}
```

<br>

* 전투
  - 스킬 아이디(번호)를 클라이언트와 서버가 주고 받으면서 클라이언트에서 스킬 아이디에 따라 스킬을 사용한다.
  - 플레이어 공격
    1.	클라이언트에서 공격 버튼 입력
    2.	C_SKILL 패킷(스킬 아이디)을 서버로 전송
    3.	서버에서 플레이어 상태 확인 및 변경
    4.	SC_SKILL 패킷(스킬 아이디, 오브젝트 아이디)을 같은 게임 룸 안의 플레이어들에게 브로드캐스트
    5.	클라이언트로부터 받은 스킬 아이디로 json 스킬 데이터 로드
    6.	서버에서 스킬 아이디에 따라 피격 판정, 데미지 판정
    7.	클라이언트는 서버로부터 스킬 아이디를 받아서 아이디에 맞는 스킬을 사용
    
        ```cpp
        void GameRoom::HandleSkill(shared_ptr<Player> player, const C_SKILL* skillPkt)
        {
            if (player == nullptr)
            {
                return;
            }
        
            WRITE_LOCK;
            {
                if (player->GetObjectState() != ObjectState_IDLE)
                {
                    return;
                }
        
                // 플레이어 상태를 스킬 상태로 변경
                player->SetObjectInfo(player->GetObjectId(), player->GetObjectName());
                player->SetPosInfo(player->GetObjectPosX(), player->GetObjectPosY(), player->GetObjectState(), player->GetObjectMoveDir());
                if (player->GetObjectState() != ObjectState_IDLE)
                {
                    return;
                }
                player->SetObjectState(ObjectState_SKILL);
        
                flatbuffers::FlatBufferBuilder builder;
                int32 skillId = skillPkt->skillInfo()->skillId();
                auto skillInfo = CreateSkillInfo(builder, skillId);
        
                // 다른 플레이어들에게 알려줌
                auto skill = CreateSC_SKILL(builder, player->GetObjectId(), skillInfo);
                auto respondSkillPkt = PacketManager::Instance().CreatePacket(skill, builder, PacketType_SC_SKILL);
        
                Broadcast(respondSkillPkt);
        
                // json 스킬 데이터 로드
                Skill skillData;
                auto iter = DataManager::Skills.find(skillPkt->skillInfo()->skillId());
                if (iter != DataManager::Skills.end())
                {
                    skillData = iter->second;
                }
        
                switch (skillData.SkillType)
                {
                    case SkillType_SKILL_AUTO:
                    {
                        Vector2Int skillPos = player->GetFrontCellPos(player->GetObjectMoveDir());
                        shared_ptr<GameObject> target = _map->Find(skillPos);
                        if (target != nullptr)
                        {
                            // NON PVP
                            if (target->GetObjectType() == GameObjectType_PLAYER)
                            {
                                return;
                            }
        
                            cout << target->GetObjectName() << " was hit!" << endl;
                            target->OnDamaged(player, skillData.Damage + player->GetObjectAttack()/*스킬 공격력 + 플레이어 공격력*/);
                        }
                    }
                    break;
        
                    case SkillType_SKILL_PROJECTILE:
                    {
                        shared_ptr<Projectile> projectile = ObjectManager::Instance().Add<Projectile>();
                        if (projectile == nullptr)
                        {
                            return;
                        }
        
                        // 플레이어 앞에 화살 생성
                        Vector2Int FrontCell = player->GetFrontCellPos();
                        projectile->SetOwner(player);
                        projectile->SetOwnerPos(player->GetObjectPosX(), player->GetObjectPosY());
                        projectile->SetSkillData(skillData);
                        projectile->SetObjectInfo(projectile->GetObjectId(), "Projectile" + to_string(projectile->GetObjectId()));
                        projectile->SetPosInfo(FrontCell.X, FrontCell.Y, ObjectState_MOVING, player->GetObjectMoveDir());
                        projectile->SetObjectSpeed(skillData.Projectile.Speed);
                        EnterGame(projectile);
                    }
                    break;
        
                case SkillType_SKILL_NONE:
                    return;
                }
            }
        }   
        ```
  - 몬스터 공격
  - 피격
  - 체력 변화
  - 리스폰

<br>

* 아이템
  - 인벤토리에 아이템 추가
    + Json 데이터를 읽어와서 게임에 입장하기 전에 플레이어가 아이템을 가지고 시작할 수 있도록 플레이어가 아이템 정보가 담겨있는 인벤토리 클래스를 들고 있음.
  - 아이템 장착
    + 아이템 아이디에 따라 공격 스킬을 변화시킴. (아이템 아이디와 공격 스킬 아이디를 일대일 대응시킴)

<br>

* 맵 이동
  
1. 플레이어가 다른 맵으로 갈 수 있는 특정 좌표에 도달
2. C_CHANGE_MAP패킷으로 맵 아이디를 서버로 전송
3. 서버에서 현재 플레이어의 맵 아이디를 전달받으면 전환할 맵 아이디와 플레이어가 스폰되어야 할 좌표 설정(예를 들어, 1번 맵에서 (10, 10)에 도달하면 2번 맵으로 이동)
4. 현재 맵에서 플레이어를 퇴장시킴
5. 서버 내부의 플레이어 정보(맵 아이디, 좌표) 갱신
6. 맵 아이디와 게임 룸 아이디를 일대일 대응시켜 놓았기 때문에 맵 아이디에 해당하는 게임 룸에 플레이어 입장 시킴
7. C_CHANGE_MAP패킷으로 맵 변경 정보를 클라이언트로 전송


[BackToTop](#toc)

---

## 6. 툴

* 맵툴
  - 클라이언트에서 제작한 맵을 서버에서도 맵 정보를 알기 위해 맵을 텍스트 파일로 변환하는 툴. 플레이어가 갈 수 있는 좌표는 0, 갈 수 없는 좌표는 1로 표시한다.
 
* 멀티 빌드
  - 유니티 에디터에서 멀티 플레이 테스트를 위한 빌드 툴

[BackToTop](#toc)

---

## 7. 오류와 문제

* 멀티 플레이 빌드 테스트 시 백그라운드에서 동기화가 안되는 문제
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

* Unity 에디터에서 플레이어 입장/퇴장 반복 테스트 중, 클라이언트의 오브젝트 매니저에서 Key 중복 예외 발생
  - 에디터의 Play 모드 중지 후에도 서버 연결이  해제되지 않아 유령 플레이어 데이터가 서버에 남아 재접속 시 클라이언트가 유령 플레이어 정보까지 받아 동일 ID 중복 추가로 Key 충돌이 발생
  - 에디터 모드에서 ‘esc’키를 누르면 명시적으로 서버와의 연결을 끊고 에디터 플레이 모드 종료하게끔 변경
  - 서버에 남아 있는 유령 플레이어는 하트비트 시스템 추가하여 게임 서버에서 일정 주기마다 모든 클라이언트 세션을 검사하여 오랫동안 응답이 없는 클라이언트를 정리

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
    1. 클라이언트 서버 코어의 가비지 컬렉션(GC)과 네트워크 버퍼 문제(C#의 GC가 비동기 작업에 사용 중인 버퍼를 잘못 건드려 발생하는 문제)
    2. 클라이언트 서버 코어의 비동기 콜백에서의 처리되지 않은 예외(OnSendCompleted, OnRecvCompleted 등에서 발생한 예외가 조용히 스레드를 종료시키고 소켓 상태를 불안정하게 만드는 경우)
    3. 게임 서버의 비정상적인 세션 관리(특정 상황에서 세션 정보가 꼬여 서버가 해당 세션을 유효하지 않다고 판단하고 정리)

[BackToTop](#toc)
