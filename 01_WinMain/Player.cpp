#include "pch.h"
#include "Player.h"
#include "TileSelect.h"

Player::Player(float x, float y, float sizeX, float sizeY)
	:GameObject("Player"), mTime(0), mFrameX(0)  
{
	mX = x;
	mY = y;
	mMoveToX = x;
	mMoveToY = y;
	mSpeed = 200;
	mSizeX = sizeX;
	mSizeY = sizeY;
	mRect = RectMakeBottom(mX, mY, mSizeX, mSizeY);
}

void Player::Init()
{
	IMAGEMANAGER->LoadFromFile(L"Sans", Resources(L"Sans.bmp"), 92, 30, 4, 1, true);
	mImage = IMAGEMANAGER->GetInstance()->FindImage(L"Sans");
	mTileSelect = new TileSelect;
}

void Player::Update()
{
	mTime += dTime;

	if (mTime > 0.2)
	{
		mTime = 0;
		mFrameX++;
	}
	if (mFrameX > 3) mFrameX = 0;

	CalcTile(mX, mY);
	if (mStandingTile->GetType() == TileType::Slow)
	{
		mSpeed = 100;
	}
	else
	{
		mSpeed = 200;
	}

	mTileSelect->Update();

	if (Input::GetInstance()->GetKey(VK_RBUTTON) and mTileSelect)
	{
		mPath = PathFinder::GetInstance()->FindPath(TILE,mStandingTile->GetIndexX(), mStandingTile->GetIndexY(),
		mTileSelect->GetIndexX(), mTileSelect->GetIndexY());
		mPathIndex = 0;
	}
	
	
	if (!mPath.empty()) {
		if (mPath.size() <= mPathIndex){
			mPath.clear();
			mPathIndex = 0;
		}
		else {

			mX = mPath[mPathIndex]->GetX()+30;
			mY = mPath[mPathIndex]->GetY()+15;
			mPathIndex++;
		}
	}


	mRect = RectMakeBottom(mX, mY, mSizeX, mSizeY);

}

void Player::Release()
{
	SafeDelete(mTileSelect);
}

void Player::Render(HDC hdc)
{
	CAMERA->ScaleFrameRender(hdc, mImage,mRect.left, mRect.top, mFrameX, 0, mSizeX, mSizeY);
	//mImage->ScaleFrameRender(hdc, mX-mSizeX/2, mY-mSizeY, mFrameX, 0, mSizeX, mSizeY);
	mTileSelect->Render(hdc);
}

void Player::CalcTile(int x, int y)
{
	int tileX = y / TileSizeY + x / TileSizeX - (StartX / TileSizeX + StartY / TileSizeY);
	int tileY = y / TileSizeY - x / TileSizeX + (StartX / TileSizeX - StartY / TileSizeY);
	int offsetX = x % TileSizeX;
	int offsetY = y % TileSizeY;

	if (offsetY < TileSizeY / 2 - offsetX / 2) { tileX--; }
	if (offsetY < offsetX / 2 - TileSizeY / 2) { tileY--; }
	if (offsetY > offsetX / 2 + TileSizeY / 2) { tileY++; }
	if (offsetY > 3 * TileSizeY / 2 - offsetX / 2) { tileX++; }

	mStandingTile = TILE[tileY][tileX];
}
