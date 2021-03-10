#pragma once
#include "GameObject.h"
#include "Tile.h"

class Tile;
class TileSelect :public GameObject
{
	Tile* mCurrentTile;
	
public:
	void Init();
	void Release();
	void Update();
	void Render(HDC hdc);


	int GetIndexX() { return mCurrentTile->GetIndexX(); }
	int GetIndexY() { return mCurrentTile->GetIndexY(); }
	int GetTileX() { return mCurrentTile->GetX(); }
	int GetTileY() { return mCurrentTile->GetY(); }

};