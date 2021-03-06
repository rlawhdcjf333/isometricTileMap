#include "pch.h"
#include "Player.h"
#include "TileSelect.h"
#include "Bullet.h"
#include "Animation.h"

Player::Player(int indexX, int indexY, float sizeX, float sizeY)
	:GameObject("Player"), mTime(0), mFrameX(0)  
{
	mIndexX = indexX;
	mIndexY = indexY;
	mX = TILE[mIndexY][mIndexX]->GetX()+30;
	mY = TILE[mIndexY][mIndexX]->GetY()+15;
	mSpeed = mInitSpeed=200;
	mAngle = 0;
	mSizeX = sizeX;
	mSizeY = sizeY;
	mRect = RectMakeBottom(mX, mY, mSizeX, mSizeY);
	mIsDash = false;
	mName = "player";
}

void Player::Init()
{
	//IMAGEMANAGER->LoadFromFile(L"Sans", Resources(L"Sans.bmp"), 92, 30, 4, 1, true);
	//mImage = IMAGEMANAGER->GetInstance()->FindImage(L"Sans");
	mTileSelect = new TileSelect;
	
	IMAGEMANAGER->LoadFromFile(L"LittleBone", Resources(L"skul/skul_little_bone.bmp"), 3780, 2400, 27, 24, true);
	mImage = IMAGEMANAGER->GetInstance()->FindImage(L"LittleBone");


	mSizeX = mImage->GetFrameWidth();
	mSizeY = mImage->GetFrameHeight();

	Animation* rightIdle = new Animation(0, 0, 3, 0, false, true, 0.2f);
	mAnimationList.push_back(rightIdle);
	mCurrentAnimation = rightIdle;

	Animation* leftIdle = new Animation(0, 0, 10, 4, true, true, 0.2f);
	mAnimationList.push_back(leftIdle);


	mCurrentAnimation->Play();

}

void Player::Update()
{
	mCurrentAnimation->Update();

	mSpeed = mInitSpeed;
	if (TILE[mIndexY][mIndexX]->GetType() == TileType::Slow)
	{
		mSpeed = mInitSpeed / 2;
	}

	mTileSelect->Update();

	if (INPUT->GetKeyDown('D'))
	{
		Attack(1,2,AttackType::Whirlwind);
	}
	if (INPUT->GetKeyDown('A'))
	{
		Attack(1, 2, AttackType::Side);
	}
	if (INPUT->GetKeyDown('S'))
	{
		Attack(1, 2, AttackType::Stab);
	}
	if (INPUT->GetKeyDown('F')) {
		Attack(1, 300, AttackType::RangedAttack);
	}

	Dash(5);

	if (mIsDash)
	{
		Move(5*mInitSpeed); //대쉬 중 3배 가속
	}
	else
	{
		if (Input::GetInstance()->GetKey(VK_RBUTTON) and mTileSelect) //대쉬 중이 아닐 경우에만 클릭 이동 활성화 == 대쉬 중에 이동캔슬 안됨
		{
			if (TILE[mTileSelect->GetIndexY()][mTileSelect->GetIndexX()]->GetType() != TileType::Block)
			{
				if (PathFinder::GetInstance()->FindPath(TILE, mPath, mIndexX, mIndexY,
					mTileSelect->GetIndexX(), mTileSelect->GetIndexY())) mPathIndex = 1;
			}
		}
		Move(mSpeed);
	}

	mRect = RectMakeBottom(mX, mY, mSizeX, mSizeY);

}

void Player::Release()
{
	SafeDelete(mTileSelect);

	for (Animation* elem : mAnimationList)
	{
		SafeDelete(elem);
	}

}

void Player::Render(HDC hdc)
{
	CAMERA->ScaleFrameRender(hdc, mImage, mRect.left, mRect.top+25, mCurrentAnimation->GetNowFrameX(), mCurrentAnimation->GetNowFrameY(), mSizeX, mSizeY);
	mTileSelect->Render(hdc);

	//{{ 개발자용 타일 체크 렌더링
	TILE[mIndexY][mIndexX]->SelectRender(hdc);

	for (Tile* elem : mPath)
	{
		elem->SelectRender(hdc);
	}
	//}}
}

void Player::Move(float speed)
{
	if (!mPath.empty()) {
		if (mPath.size() <= mPathIndex) //목적지까지 이동 완료
		{
			mPath.clear();
			mPathIndex = 1; 
			mIsDash = false;
		}
		else //이동 중
		{
			float pathX = mPath[mPathIndex]->GetX() + TileSizeX / 2;
			float pathY = mPath[mPathIndex]->GetY() + TileSizeY / 2;
			mAngle = Math::GetAngle(mX, mY, pathX, pathY); //앵글 거리 계산이 0이 나올때 리턴 0으로 막음
			mX += speed * cosf(mAngle) * dTime;
			mY -= speed * sinf(mAngle) * dTime;

			if (abs(mX - pathX) < speed * dTime and abs(mY - pathY) < speed * dTime) //오차 보정
			{
				mX = pathX;
				mY = pathY;
				mIndexX = mPath[mPathIndex]->GetIndexX();
				mIndexY = mPath[mPathIndex]->GetIndexY();
				mPathIndex++;
			}

		}
	}
}

void Player::Dash(int dist)
{
	//커서 방향으로 int dist의 타일 블록 크기만큼 대쉬
	if (INPUT->GetKeyDown(VK_SPACE))
	{
		mPath.clear();
		float angle = Math::GetAngle(mX, mY, CAMERA->CameraMouseX(), CAMERA->CameraMouseY());
		
		if (angle >= PI2-(PI/8) or angle <(PI/8)) //우향 ->등축투영 가로 길이 보정때문에 대쉬 거리 조정
		{
			for (int i = 0; i < (dist+1)*2/3; i++) {
				if (mIndexX + i < TILESizeX and mIndexY - i >= 0)
				{
					mPath.push_back(TILE[mIndexY - i][mIndexX + i]);
				}
			}
		}
		else if (angle >= (PI / 8) and angle < (3 * PI / 8)) //우상향
		{
			for (int i = 0; i < dist + 1; i++) { 
				if (mIndexY - i >= 0 and mIndexY - i < TILESizeY)
				{
					mPath.push_back(TILE[mIndexY - i][mIndexX]);
				}
			}
		}
		else if (angle >= (3 * PI / 8) and angle < (5 * PI / 8)) //상향
		{
			for (int i = 0; i < dist + 1; i++) { 
				if (mIndexX - i >= 0 and mIndexY - i >= 0)
				{
					mPath.push_back(TILE[mIndexY - i][mIndexX - i]);
				}
			}
		}
		else if (angle >= (5 * PI / 8) and angle < (7 * PI / 8)) //좌상향
		{
			for (int i = 0; i < dist + 1; i++) {
				if (mIndexX - i >= 0 )
				{
					mPath.push_back(TILE[mIndexY][mIndexX - i]);
				}
			}
		}
		else if (angle >= (7 * PI / 8) and angle < (9 * PI / 8)) //좌향  ->등축투영 가로 길이 보정때문에 대쉬거리 조정
		{
			for (int i = 0; i < (dist+1)*2/3; i++) {
				if (mIndexX - i >= 0 and mIndexY + i < TILESizeY)
				{
					mPath.push_back(TILE[mIndexY + i][mIndexX - i]);
				}
			}
		}
		else if (angle >= (9 * PI / 8) and angle < (11 * PI / 8)) //좌하향
		{
			for (int i = 0; i < dist + 1; i++) { 
				if (mIndexY + i < TILESizeY)
				{
					mPath.push_back(TILE[mIndexY + i][mIndexX]);
				}
			}
		}
		else if (angle >= (11 * PI / 8) and angle < (13 * PI / 8)) //하향
		{
			for (int i = 0; i < dist + 1; i++) { 
				if (mIndexX + i < TILESizeX and mIndexY +i < TILESizeY)
				{
					mPath.push_back(TILE[mIndexY + i][mIndexX + i]);
				}
			}
		}
		else if (angle >= (13 * PI / 8) and angle < (15 * PI / 8)) //우하향
		{
			for (int i = 0; i < dist + 1; i++) { 
				if (mIndexX + i < TILESizeX)
				{
					mPath.push_back(TILE[mIndexY][mIndexX + i]);
				}
			}
		}
		
		for (Tile* elem : mPath) //대쉬 경로에 막힌 길이 존재하면 막힌 타일 이전까지만 대쉬
		{
			if (elem->GetType() == TileType::Block)
			{
				mPath.assign(mPath.begin(), find(mPath.begin(), mPath.end(), elem));
			}
		}

		mPathIndex = 1; mIsDash = true;

	}

}

void Player::Attack(int damage, int range, AttackType type)
{
	float angle = Math::GetAngle(mX, mY, CAMERA->CameraMouseX(), CAMERA->CameraMouseY());

	switch (type) {
		case AttackType::Side:
		if (angle > 7 * PI / 4 or angle <= PI / 4) //우측
		{
			TILE[mIndexY - 1][mIndexX - 1]->AttackDamage(damage);
			TILE[mIndexY - 1][mIndexX]->AttackDamage(damage);
			TILE[mIndexY - 1][mIndexX + 1]->AttackDamage(damage);
			TILE[mIndexY][mIndexX + 1]->AttackDamage(damage);
			TILE[mIndexY + 1][mIndexX + 1]->AttackDamage(damage);
		}
		else if (angle > PI / 4 and angle <= 3 * PI / 4) //상방
		{
			TILE[mIndexY - 1][mIndexX - 1]->AttackDamage(damage);
			TILE[mIndexY - 1][mIndexX]->AttackDamage(damage);
			TILE[mIndexY - 1][mIndexX + 1]->AttackDamage(damage);
			TILE[mIndexY][mIndexX - 1]->AttackDamage(damage);
			TILE[mIndexY + 1][mIndexX - 1]->AttackDamage(damage);

		}
		else if (angle > 3 * PI / 4 and angle <= 5 * PI / 4) //좌측
		{
			TILE[mIndexY - 1][mIndexX - 1]->AttackDamage(damage);
			TILE[mIndexY][mIndexX - 1]->AttackDamage(damage);
			TILE[mIndexY + 1][mIndexX - 1]->AttackDamage(damage);
			TILE[mIndexY + 1][mIndexX]->AttackDamage(damage);
			TILE[mIndexY + 1][mIndexX + 1]->AttackDamage(damage);
		}
		else if (angle > 5 * PI / 4 and angle <= 7 * PI / 4)//하방
		{
			TILE[mIndexY + 1][mIndexX - 1]->AttackDamage(damage);
			TILE[mIndexY + 1][mIndexX]->AttackDamage(damage);
			TILE[mIndexY + 1][mIndexX + 1]->AttackDamage(damage);
			TILE[mIndexY][mIndexX + 1]->AttackDamage(damage);
			TILE[mIndexY - 1][mIndexX + 1]->AttackDamage(damage);
		}
		break;

		case AttackType::Stab:
		if (angle >= PI2 - (PI / 8) or angle < (PI / 8)) //우향 ->등축투영 가로 길이 보정때문에 대쉬 거리 조정
		{
			for (int i = 0; i < range + 1; i++) {
				if (mIndexX + i < TILESizeX and mIndexY - i >= 0)
				{
					if (TILE[mIndexY - i][mIndexX + i]->GetType() == TileType::Block)
						break;
					TILE[mIndexY - i][mIndexX + i]->AttackDamage(damage);
				}
			}
		}
		else if (angle >= (PI / 8) and angle < (3 * PI / 8)) //우상향
		{
			for (int i = 0; i < range + 1; i++) {
				if (mIndexY - i >= 0 and mIndexY - i < TILESizeY)
				{
					if (TILE[mIndexY - i][mIndexX]->GetType() == TileType::Block)
						break;
					TILE[mIndexY - i][mIndexX]->AttackDamage(damage);
				}
			}
		}
		else if (angle >= (3 * PI / 8) and angle < (5 * PI / 8)) //상향
		{
			for (int i = 0; i < range + 1; i++) {
				if (mIndexX - i >= 0 and mIndexY - i >= 0)
				{
					if (TILE[mIndexY - i][mIndexX - i]->GetType() == TileType::Block)
						break;
					TILE[mIndexY - i][mIndexX - i]->AttackDamage(damage);
				}
			}
		}
		else if (angle >= (5 * PI / 8) and angle < (7 * PI / 8)) //좌상향
		{
			for (int i = 0; i < range + 1; i++) {
				if (mIndexX - i >= 0)
				{
					if (TILE[mIndexY][mIndexX - i]->GetType() == TileType::Block)
						break;
					TILE[mIndexY][mIndexX - i]->AttackDamage(damage);
				}
			}
		}
		else if (angle >= (7 * PI / 8) and angle < (9 * PI / 8)) //좌향  ->등축투영 가로 길이 보정때문에 대쉬거리 조정
		{
			for (int i = 0; i < range + 1; i++) {
				if (mIndexX - i >= 0 and mIndexY + i < TILESizeY)
				{
					if (TILE[mIndexY + i][mIndexX - i]->GetType() == TileType::Block)
						break;
					TILE[mIndexY + i][mIndexX - i]->AttackDamage(damage);
				}
			}
		}
		else if (angle >= (9 * PI / 8) and angle < (11 * PI / 8)) //좌하향
		{
			for (int i = 0; i < range + 1; i++) {
				if (mIndexY + i < TILESizeY)
				{
					if (TILE[mIndexY + i][mIndexX]->GetType() == TileType::Block)
						break;
					TILE[mIndexY + i][mIndexX]->AttackDamage(damage);
				}
			}
		}
		else if (angle >= (11 * PI / 8) and angle < (13 * PI / 8)) //하향
		{
			for (int i = 0; i < range + 1; i++) {
				if (mIndexX + i < TILESizeX and mIndexY + i < TILESizeY)
				{
					if (TILE[mIndexY + i][mIndexX + i]->GetType() == TileType::Block)
						break;
					TILE[mIndexY + i][mIndexX + i]->AttackDamage(damage);
				}
			}
		}
		else if (angle >= (13 * PI / 8) and angle < (15 * PI / 8)) //우하향
		{
			for (int i = 0; i < range + 1; i++) {
				if (mIndexX + i < TILESizeX)
				{
					if (TILE[mIndexY][mIndexX + i]->GetType() == TileType::Block)
						break;
					TILE[mIndexY][mIndexX + i]->AttackDamage(damage);
				}
			}
		}
		break;

		case AttackType::Whirlwind:
		for (int y = mIndexY - range; y < mIndexY + range; y++) {
			for (int x = mIndexX - range; x < mIndexX + range; x++) {
				if (y <=0 || y>TILESizeY || x<=0 || x>TILESizeX) {
						continue;
				}
				TILE[y][x]->AttackDamage(damage);
			}
		}
		break;
		case AttackType::RangedAttack:
			new Bullet(nullptr,"Bullet",this,damage,300,range,angle,BulletType::Straight);
		break;
	}
}
