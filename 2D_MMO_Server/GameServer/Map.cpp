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
	if (cellPos.X < GetMinX() || cellPos.X > GetMaxX())
	{
		return false;
	}
	if (cellPos.Y < GetMinY() || cellPos.Y > GetMaxY())
	{
		return false;
	}

	int32 x = cellPos.X - GetMinX();
	int32 y = GetMaxY() - cellPos.Y;
	return !_collision[y][x] && (!checkObjects || _objects[y][x] == nullptr);
}

shared_ptr<GameObject> Map::Find(Vector2Int cellPos)
{
	if (cellPos.X < GetMinX() || cellPos.X > GetMaxX())
	{
		return nullptr;
	}
	if (cellPos.Y < GetMinY() || cellPos.Y > GetMaxY())
	{
		return nullptr;
	}

	int32 x = cellPos.X - GetMinX();
	int32 y = GetMaxY() - cellPos.Y;

	return _objects[y][x];
}

bool Map::ApplyMove(shared_ptr<GameObject> gameObj, Vector2Int dest)
{
	ApplyLeave(gameObj);

	if (CanGo(dest, true) == false)
	{
		return false;
	}

	// 이동할 좌표에 플레이어 정보 저장
	{
		int32 x = dest.X - GetMinX();
		int32 y = GetMaxY() - dest.Y;
		_objects[y][x] = gameObj;
	}

	// 실제 좌표 이동
	gameObj->SetObjectPosX(dest.X);
	gameObj->SetObjectPosY(dest.Y);

	return true;
}

bool Map::ApplyLeave(shared_ptr<GameObject> gameObj)
{
	if (gameObj->GetObjectPosX() < GetMinX() || gameObj->GetObjectPosX() > GetMaxX())
	{
		return false;
	}
	if (gameObj->GetObjectPosY() < GetMinY() || gameObj->GetObjectPosY() > GetMaxY())
	{
		return false;
	}

	// 게임 오브젝트가 위치해 있는 좌표를 맵 정보에서 삭제
	{
		int32 x = gameObj->GetObjectPosX() - GetMinX();
		int32 y = GetMaxY() - gameObj->GetObjectPosY();
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

		int32 xCount = GetMaxX() - GetMinX() + 1;
		int32 yCount = GetMaxY() - GetMinY() + 1;

		InitMatrix(_collision, xCount, yCount);
		InitMatrix(_objects, xCount, yCount);

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
