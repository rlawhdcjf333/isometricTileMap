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

	int GetTileX() { return mCurrentTile->GetX(); }
	int GetTileY() { return mCurrentTile->GetY(); }

};