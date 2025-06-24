#pragma once
#include "pch.h"
#include "Job.h"

class Map;
class GameObject;
class Player;
class Monster;
class Projectile;
class Item;

class GameRoom : public enable_shared_from_this<GameRoom>
{
public:
	GameRoom();
	~GameRoom() = default;

public:
	void Init(int32 mapId);
	void EnterGame(shared_ptr<GameObject> gameObj);
	void LeaveGame(int32 objectId);
	void HandleMove(shared_ptr<Player> player, const C_MOVE* movePkt);
	void HandleSkill(shared_ptr<Player> player, const C_SKILL* skillPkt);
	void HandleEquipItem(shared_ptr<Player> player, const C_EQUIP_ITEM* equipItemPkt);
	void HandleChangeMap(shared_ptr<Player> player, const C_CHANGE_MAP* changeMapPkt);
	shared_ptr<Player> FindPlayer(function<bool(shared_ptr<GameObject>)> condition);
	void Broadcast(SendBufferRef buffer);
	void Update();

	// 멀티스레드 환경에서는 Jobs로 접근
public:
	void FlushJob();

	template<typename T, typename Ret, typename... Args>
	void PushJob(Ret(T::*memFunc)(Args...), Args... args)
	{
		auto job = make_shared<MemberJob<T, Ret, Args...>>(static_cast<T*>(this), memFunc, args...);
		_jobs.Push(job);
	}

	// 람다 및 일반 함수용 PushJob 오버로딩
	// 템플릿을 사용하여 어떤 종류의 호출 가능한 객체도 받을 수 있다
	template<typename F>
	void PushJob(F&& job)
	{
		// LambdaJob을 생성하여 작업을 래핑(wrapping)하고 큐에 넣는다
		auto lambdaJob = make_shared<LambdaJob>(std::forward<F>(job));
		_jobs.Push(lambdaJob);
	}

public:
	int32 GetRoomId() const { return _roomId; }
	void SetRoomId(int32 roomId) { _roomId = roomId; }

	shared_ptr<Map> GetMap() const { return _map; }

private:
	USE_LOCK;
	int32 _roomId;
	map<int32, shared_ptr<Player>> _players;
	map<int32, shared_ptr<Monster>> _monsters;
	map<int32, shared_ptr<Projectile>> _projectiles;
	map<int32, shared_ptr<Item>> _items;
	shared_ptr<Map> _map;

	JobQueue _jobs;
};

// GameRoom Jobs
// 이 방법은 함수 하나에 클래스를 대응되게 만들어야 해서 번거롭다 -> 더 좋은 방법 필요
//class EnterGameJob : public IJob
//{
//public:
//	EnterGameJob(shared_ptr<GameRoom> room, shared_ptr<GameObject> gameObj)
//		: _room(room),
//		_gameObj(gameObj)
//	{
//
//	}
//
//	virtual void Execute() override
//	{
//		_room->EnterGame(_gameObj);
//	}
//
//private:
//	shared_ptr<GameRoom> _room;
//	shared_ptr<GameObject> _gameObj;
//};