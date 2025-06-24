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

	// ��Ƽ������ ȯ�濡���� Jobs�� ����
public:
	void FlushJob();

	template<typename T, typename Ret, typename... Args>
	void PushJob(Ret(T::*memFunc)(Args...), Args... args)
	{
		auto job = make_shared<MemberJob<T, Ret, Args...>>(static_cast<T*>(this), memFunc, args...);
		_jobs.Push(job);
	}

	// ���� �� �Ϲ� �Լ��� PushJob �����ε�
	// ���ø��� ����Ͽ� � ������ ȣ�� ������ ��ü�� ���� �� �ִ�
	template<typename F>
	void PushJob(F&& job)
	{
		// LambdaJob�� �����Ͽ� �۾��� ����(wrapping)�ϰ� ť�� �ִ´�
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
// �� ����� �Լ� �ϳ��� Ŭ������ �����ǰ� ������ �ؼ� ���ŷӴ� -> �� ���� ��� �ʿ�
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