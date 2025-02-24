#include "pch.h"
#include <fstream>
#include "Map.h"
#include "Player.h"

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
	return !_collision[y][x] && (!checkObjects || _players[y][x] == nullptr);
}

bool Map::ApplyMove(shared_ptr<Player> player, Vector2Int dest)
{
	if (player->GetPlayerPosX() < GetMinX() || player->GetPlayerPosX() > GetMaxX())
	{
		return false;
	}
	if (player->GetPlayerPosY() < GetMinY() || player->GetPlayerPosY() > GetMaxY())
	{
		return false;
	}
	if (CanGo(dest, true) == false)
	{
		return false;
	}

	// ������ ������ �ִ� �� ����(��ǥ)�� �÷��̾� ����
	// �����̱� �� ��ǥ�� �÷��̾� ���� ����
	{
		int32 x = player->GetPlayerPosX() - GetMinX();
		int32 y = GetMaxY() - player->GetPlayerPosY();
		if (_players[y][x] == player)
		{
			_players[y][x] = nullptr;
		}
	}

	// �̵��� ��ǥ�� �÷��̾� ���� ����
	{
		int32 x = dest.X - GetMinX();
		int32 y = GetMaxY() - dest.Y;
		_players[y][x] = player;
	}

	// ���� ��ǥ �̵�
	player->SetPlayerPosX(dest.X);
	player->SetPlayerPosY(dest.Y);
	return true;
}

void Map::LoadMap(int32 mapId, string path)
{
	// ���̵� ���� �� ���� �̸� ����. ��) Map_001.txt
	string mapNum = to_string(mapId);
	size_t mapNumSize = 3;

	size_t zeroPadding = mapNumSize - min(mapNumSize, mapNum.size());
	mapNum.insert(0, zeroPadding, '0');

	string mapName = "Map_" + mapNum;
	string mapFilepath = path + "/" + mapName + ".txt";

	// ��ο� �ִ� �� ���� �� �پ� �ε�
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
		InitMatrix(_players, xCount, yCount);

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
		cout << "Not Found Files!" << endl;
		CRASH();
		return;
	}
}

shared_ptr<Player> Map::Find(Vector2Int cellPos)
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

	return _players[y][x];
}
