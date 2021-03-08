#pragma once
class Tile;
class TileList
{
	Singleton(TileList)

private:

	typedef vector<vector<Tile*>> TileMap;
	TileMap mTileList;
	
public:
	inline void SetMap(TileMap tileMap) { mTileList = tileMap; }
	inline TileMap GetMap() { return mTileList; }

};

#define TILE  TileList::GetInstance()->GetMap()
