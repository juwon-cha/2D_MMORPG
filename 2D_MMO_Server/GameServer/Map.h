#pragma once
#include "pch.h"

class Player;

struct Vector2Int
{
	Vector2Int(int32 x, int32 y)
	{
		this->X = x;
		this->Y = y;
	}

	static Vector2Int Up() { return Vector2Int(0, 1); }
	static Vector2Int Down() { return Vector2Int(0, -1); }
	static Vector2Int Left() { return Vector2Int(-1, 0); }
	static Vector2Int Right() { return Vector2Int(1, 0); }

	friend Vector2Int operator+(Vector2Int a, Vector2Int b)
	{
		return Vector2Int(a.X + b.X, a.Y + b.Y);
	}

	int32 X;
	int32 Y;
};

class Map
{
public:
	Map();
	~Map() = default;

public:
	bool CanGo(Vector2Int cellPos, bool checkObjects = true);
	bool ApplyMove(shared_ptr<Player> player, Vector2Int dest);
	void LoadMap(int32 mapId, string path = "../../Common/MapData");
	shared_ptr<Player> Find(Vector2Int cellPos);

	template<typename T>
	void InitMatrix(vector<vector<T>>& matrix, uint32 x, uint32 y)
	{
		vector<T> colMatrix;
		colMatrix.assign(x, 0);
		matrix.assign(y, colMatrix);
	}

public:
	int32 GetMinX() const { return _minX; }
	int32 GetMaxX() const { return _maxX; }
	int32 GetMinY() const { return _minY; }
	int32 GetMaxY() const { return _maxY; }
	void SetMinX(int32 x) { _minX = x; }
	void SetMaxX(int32 x) { _maxX = x; }
	void SetMinY(int32 y) { _minY = y; }
	void SetMaxY(int32 y) { _maxY = y; }

private:
	int32 _minX;
	int32 _maxX;
	int32 _minY;
	int32 _maxY;

	vector<vector<bool>> _collision;
	vector<vector<shared_ptr<Player>>> _players;
};
