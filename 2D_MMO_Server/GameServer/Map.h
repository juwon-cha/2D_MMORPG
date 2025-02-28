#pragma once
#include "pch.h"

class GameObject;

struct Pos
{
	Pos(int32 y, int32 x) { Y = y; X = x; }

	int32 Y;
	int32 X;
};

struct Vector2Int
{
	Vector2Int() { X = 0; Y = 0; }
	Vector2Int(int32 x, int32 y) { X = x; Y = y; }

	static Vector2Int Up() { return Vector2Int(0, 1); }
	static Vector2Int Down() { return Vector2Int(0, -1); }
	static Vector2Int Left() { return Vector2Int(-1, 0); }
	static Vector2Int Right() { return Vector2Int(1, 0); }

	friend Vector2Int operator+(Vector2Int a, Vector2Int b)
	{
		return Vector2Int(a.X + b.X, a.Y + b.Y);
	}

	friend Vector2Int operator-(Vector2Int a, Vector2Int b)
	{
		return Vector2Int(a.X - b.X, a.Y - b.Y);
	}

	float magnitude() { return static_cast<float>(std::sqrt(squareMagnitude())); }
	int squareMagnitude() { return (X * X + Y * Y); }
	int CellDistance() { return std::abs(X) + std::abs(Y); } // 가고자 하는 좌표까지 거리

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
	shared_ptr<GameObject> Find(Vector2Int cellPos);
	bool ApplyMove(shared_ptr<GameObject> gameObj, Vector2Int dest);
	bool ApplyLeave(shared_ptr<GameObject> gameObj);
	void LoadMap(int32 mapId, string path = "../../Common/MapData");
	vector<Vector2Int> FindPathBFS(Vector2Int start, Vector2Int dest, bool checkObjects = true);

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
	vector<Vector2Int> CalcCellPathFromParent(vector<vector<Pos>> parent, Pos dest);
	Pos CellToPos(Vector2Int cell);
	Vector2Int PosToCell(Pos pos);

private:
	int32 _minX;
	int32 _maxX;
	int32 _minY;
	int32 _maxY;

	vector<vector<bool>> _collision;
	vector<vector<shared_ptr<GameObject>>> _objects;
};
