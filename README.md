# 2D_MMORPG
## <a id="toc"></a>목차

1. [개요](#1-개요)
2. [일정](#2-일정)
3. [모듈 구성도](#3-모듈-구성도)
4. [플레이 영상](#4-플레이-영상)

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

<iframe width="894" height="503" src="https://www.youtube.com/embed/Rj2cyzisgQY" title="2D Multiplayer RPG" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" referrerpolicy="strict-origin-when-cross-origin" allowfullscreen></iframe>

[BackToTop](#toc)
