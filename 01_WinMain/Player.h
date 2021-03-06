#pragma once
#include "GameObject.h"
enum class AttackType : int {
	Side= 1,
	Stab,
	Whirlwind,
	RangedAttack
};

enum class Motion : int {

	RightIdle=0,
	LeftIdle,
	RigthWalk,
	LeftWalk

};
class TileSelect;
class Tile;
class Animation;
class Player : public GameObject
{
	Image* mImage;
	TileSelect* mTileSelect;
	vector<Tile*> mPath;

	Animation* mCurrentAnimation;
	vector <Animation*> mAnimationList;


	float mSpeed;
	float mInitSpeed;
	float mAngle;
	float mTime;
	int mFrameX;

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
	void Attack(int damage, int range, AttackType type);
};