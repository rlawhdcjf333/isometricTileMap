#include "pch.h"
#include "Player.h"
#include "TileSelect.h"

Player::Player(int indexX, int indexY, float sizeX, float sizeY)
	:GameObject("Player"), mTime(0), mFrameX(0)  
{
	mIndexX = indexX;
	mIndexY = indexY;
	mX = TILE[mIndexY][mIndexX]->GetX()+30;
	mY = TILE[mIndexY][mIndexX]->GetY()+15;
	mSpeed = 200;
	mAngle = 0;
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

 
	if (TILE[mIndexY][mIndexX]->GetType() == TileType::Slow)
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
		mPath = PathFinder::GetInstance()->FindPath(TILE,mIndexX, mIndexY,
		mTileSelect->GetIndexX(), mTileSelect->GetIndexY());
		mPathIndex = 1;
	}
	
	
	if (!mPath.empty()) {
		if (mPath.size()<= mPathIndex){
			mPath.clear();
			mPathIndex = 1; //1부터 시작해야 처음 시작한 위치부터 계산을 안한다 // 첫 시작 위치 계산하면 한무 회귀 반복함
		}
		else 
		{
			int pathX = mPath[mPathIndex]->GetX() + TileSizeX/2;
			int pathY = mPath[mPathIndex]->GetY() + TileSizeY/2;
			mAngle = Math::GetAngle(mX, mY, pathX, pathY); //앵글 거리 계산이 0이 나올때 리턴 0으로 막음
			mX += mSpeed * cosf(mAngle)*dTime;
			mY -= mSpeed * sinf(mAngle) * dTime;

			if(abs(mX -pathX) <5 and abs(mY-pathY)<5)
			{
				mX = pathX;
				mY = pathY;
				mIndexX = mPath[mPathIndex]->GetIndexX();
				mIndexY = mPath[mPathIndex]->GetIndexY();
				mPathIndex++;
			}

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


	TILE[mIndexY][mIndexX]->SelectRender(hdc);
	
	for (Tile* elem : mPath)
	{
		elem->SelectRender(hdc);
	}
}
