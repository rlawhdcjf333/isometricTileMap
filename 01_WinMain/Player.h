#pragma once
#include "GameObject.h"

class TileSelect;
class Tile;
class Player : public GameObject
{
	Image* mImage;
	TileSelect* mTileSelect;
	vector<Tile*> mPath;
	float mSpeed;
	float mAngle;
	float mTime;
	int mFrameX;

	int mIndexX;
	int mIndexY;

	int mPathIndex = 0;

public:

	Player(int indexX, int indexY, float sizeX, float sizeY);
	void Init();
	void Update();
	void Release();
	void Render(HDC hdc);
};