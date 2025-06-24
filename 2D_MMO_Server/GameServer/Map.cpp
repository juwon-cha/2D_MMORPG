#include "pch.h"
#include <fstream>
#include "Map.h"
#include "GameObject.h"

Map::Map()
	: _minX(0)
	, _maxX(0)
	, _minY(0)
	, _maxY(0)
{
}

bool Map::CanGo(Vector2Int cellPos, bool checkObjects)
{
	if (cellPos.X < _minX || cellPos.X > _maxX)
	{
		return false;
	}
	if (cellPos.Y < _minY || cellPos.Y > _maxY)
	{
		return false;
	}

	int32 x = cellPos.X - _minX;
	int32 y = _maxY - cellPos.Y;
	return !_collision[y][x] && (!checkObjects || _objects[y][x] == nullptr);
}

shared_ptr<GameObject> Map::Find(Vector2Int cellPos)
{
	if (cellPos.X < _minX || cellPos.X > _maxX)
	{
		return nullptr;
	}
	if (cellPos.Y < _minY || cellPos.Y > _maxY)
	{
		return nullptr;
	}

	int32 x = cellPos.X - _minX;
	int32 y = _maxY - cellPos.Y;

	return _objects[y][x];
}

bool Map::ApplyMove(shared_ptr<GameObject> gameObj, Vector2Int dest)
{
	ApplyLeave(gameObj);

	if (CanGo(dest, true) == false)
	{
		return false;
	}

	// 이동할 좌표에 게임 오브젝트 정보 저장
	{
		int32 x = dest.X - _minX;
		int32 y = _maxY - dest.Y;
		_objects[y][x] = gameObj;
	}

	// 실제 좌표 이동
	gameObj->SetObjectPosX(dest.X);
	gameObj->SetObjectPosY(dest.Y);

	return true;
}

bool Map::ApplyLeave(shared_ptr<GameObject> gameObj)
{
	if (gameObj->GetObjectPosX() < _minX || gameObj->GetObjectPosX() > _maxX)
	{
		return false;
	}
	if (gameObj->GetObjectPosY() < _minY || gameObj->GetObjectPosY() > _maxY)
	{
		return false;
	}

	// 게임 오브젝트가 위치해 있는 좌표를 맵 정보에서 삭제
	{
		int32 x = gameObj->GetObjectPosX() - _minX;
		int32 y = _maxY - gameObj->GetObjectPosY();
		if (_objects[y][x] == gameObj)
		{
			_objects[y][x] = nullptr;
		}
	}

	return true;
}

void Map::LoadMap(int32 mapId, string path)
{
	string mapNum = to_string(mapId);
	size_t mapNumSize = 3;

	size_t zeroPadding = mapNumSize - min(mapNumSize, mapNum.size());
	mapNum.insert(0, zeroPadding, '0');

	string mapName = "Map_" + mapNum;

	// Collision 관련 파일
	string mapFilepath = path + "/" + mapName + ".txt";
	ifstream file(mapFilepath, ios_base::in);
	if (file.is_open())
	{
		string line;
		getline(file, line);
		SetMinX(stoi(line));
		getline(file, line);
		SetMaxX(stoi(line) - 1);
		getline(file, line);
		SetMinY(stoi(line));
		getline(file, line);
		SetMaxY(stoi(line) - 1);

		int32 xCount = _maxX - _minX + 1;
		int32 yCount = _maxY - _minY + 1;

		InitMatrix<bool>(_collision, xCount, yCount);
		InitMatrix<shared_ptr<GameObject>>(_objects, xCount, yCount);

		for (int32 y = 0; y < yCount; y++)
		{
			getline(file, line);
			for (int32 x = 0; x < xCount; x++)
			{
				_collision[y][x] = (line[x] == '1' ? true : false);
			}
		}
	}
	else
	{
		cout << "파일을 찾을 수 없습니다!" << endl;
		CRASH();
		return;
	}
}

vector<Vector2Int> Map::FindPathBFS(Vector2Int start, Vector2Int dest, bool checkObjects)
{
	int32 deltaY[4] = { 1, -1, 0, 0 };
	int32 deltaX[4] = { 0, 0, -1, 1 };

	const int32 sizeY = _maxY - _minY + 1;
	const int32 sizeX = _maxX - _minX + 1;

	// 이차원 벡터 초기화
	vector<vector<bool>> found;
	InitMatrix<bool>(found, sizeX, sizeY);

	vector<vector<Pos>> parent;
	vector<Pos> colParent;
	colParent.assign(sizeX, Pos(0, 0));
	parent.assign(sizeY, colParent);

	Pos startPos = CellToPos(start);
	Pos destPos = CellToPos(dest);

	queue<Pos> queue;
	queue.push(Pos(startPos.Y, startPos.X));

	found[startPos.Y][startPos.X] = true;
	parent[startPos.Y][startPos.X] = Pos(startPos.Y, startPos.X);

	while (queue.size() > 0)
	{
		Pos pos = queue.front();
		queue.pop();
		int nowY = pos.Y;
		int nowX = pos.X;

		for (int32 i = 0; i < sizeof(deltaY)/sizeof(int32); ++i)
		{
			Pos next = Pos(nowY + deltaY[i], nowX + deltaX[i]);

			// 막혀있으면 스킵
			if (next.Y != destPos.Y || next.X != destPos.X)
			{
				if (CanGo(PosToCell(next), checkObjects) == false)
				{
					continue;
				}
			}
			// 이미 방문한 곳이면 스킵
			if (found[next.Y][next.X])
			{
				continue;
			}

			queue.push(Pos(next.Y, next.X));
			found[next.Y][next.X] = true;
			parent[next.Y][next.X] = Pos(nowY, nowX);
		}
	}

	return CalcCellPathFromParent(parent, destPos);
}

vector<Vector2Int> Map::CalcCellPathFromParent(vector<vector<Pos>> parent, Pos dest)
{
	vector<Vector2Int> cells;

	int y = dest.Y;
	int x = dest.X;
	while (parent[y][x].Y != y || parent[y][x].X != x)
	{
		cells.push_back(PosToCell(Pos(y, x)));
		Pos pos = parent[y][x];
		y = pos.Y;
		x = pos.X;
	}
	cells.push_back(PosToCell(Pos(y, x)));
	std::reverse(cells.begin(), cells.end());

	return cells;
}

Pos Map::CellToPos(Vector2Int cell)
{
	return Pos(_maxY - cell.Y, cell.X - _minX);
}

Vector2Int Map::PosToCell(Pos pos)
{
	return Vector2Int(pos.X + _minX, _maxY - pos.Y);
}
