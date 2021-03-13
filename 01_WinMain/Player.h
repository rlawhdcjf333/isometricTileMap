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
	float mInitSpeed;
	float mAngle;
	float mTime;
	int mFrameX;

	int mIndexX;
	int mIndexY;

	bool mIsDash;
	int mPathIndex = 0;

public:

	Player(int indexX, int indexY, float sizeX, float sizeY);
	void Init();
	void Update();
	void Release();
	void Render(HDC hdc);

	void Move(float speed);
	void Dash(int dist);
};