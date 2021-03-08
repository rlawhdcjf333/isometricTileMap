#pragma once
#include "GameObject.h"

class TileSelect;
class Tile;
class Player : public GameObject
{
	Image* mImage;
	TileSelect* mTileSelect;
	Tile* mStandingTile;

	float mSpeed;
	float mTime;
	int mFrameX;

	int mMoveToX;
	int mMoveToY;

public:

	Player(float x, float y, float sizeX, float sizeY);
	void Init();
	void Update();
	void Release();
	void Render(HDC hdc);


	void CalcTile(int x, int y);
	
};