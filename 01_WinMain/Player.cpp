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
	mSpeed = mInitSpeed=200;
	mAngle = 0;
	mSizeX = sizeX;
	mSizeY = sizeY;
	mRect = RectMakeBottom(mX, mY, mSizeX, mSizeY);
	mIsDash = false;
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

 
	mSpeed = mInitSpeed;
	if (TILE[mIndexY][mIndexX]->GetType() == TileType::Slow)
	{
		mSpeed = mInitSpeed / 2;
	}

	mTileSelect->Update();

	Dash(5);

	if (mIsDash)
	{
		Move(5*mInitSpeed); //대쉬 중 3배 가속
	}
	else
	{
		if (Input::GetInstance()->GetKey(VK_RBUTTON) and mTileSelect) //대쉬 중이 아닐 경우에만 클릭 이동 활성화 == 대쉬 중에 이동캔슬 안됨
		{
			if (PathFinder::GetInstance()->FindPath(TILE, mPath, mIndexX, mIndexY,
				mTileSelect->GetIndexX(), mTileSelect->GetIndexY())) mPathIndex = 1;
		}

		Move(mSpeed);
	}


	mRect = RectMakeBottom(mX, mY, mSizeX, mSizeY);

}

void Player::Release()
{
	SafeDelete(mTileSelect);
}

void Player::Render(HDC hdc)
{
	CAMERA->ScaleFrameRender(hdc, mImage, mRect.left, mRect.top, mFrameX, 0, mSizeX, mSizeY);
	//mImage->ScaleFrameRender(hdc, mX-mSizeX/2, mY-mSizeY, mFrameX, 0, mSizeX, mSizeY);
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
		float angle = Math::GetAngle(mX, mY, _mousePosition.x, _mousePosition.y);
		
		if (angle >= PI2-(PI/8) or angle <(PI/8)) //우향 ->등축투영 가로 길이 보정때문에 대쉬 거리 조정
		{
			for (int i = 0; i < (dist+1)*2/3; i++) {mPath.push_back(TILE[mIndexY-i][mIndexX+i]);}
		}
		else if (angle >= (PI / 8) and angle < (3 * PI / 8)) //우상향
		{
			for (int i = 0; i < dist + 1; i++) { mPath.push_back(TILE[mIndexY - i][mIndexX]); }
		}
		else if (angle >= (3 * PI / 8) and angle < (5 * PI / 8)) //상향
		{
			for (int i = 0; i < dist + 1; i++) { mPath.push_back(TILE[mIndexY-i][mIndexX-i]); }
		}
		else if (angle >= (5 * PI / 8) and angle < (7 * PI / 8)) //좌상향
		{
			for (int i = 0; i < dist + 1; i++) { mPath.push_back(TILE[mIndexY][mIndexX - i]); }
		}
		else if (angle >= (7 * PI / 8) and angle < (9 * PI / 8)) //좌향  ->등축투영 가로 길이 보정때문에 대쉬거리 조정
		{
			for (int i = 0; i < (dist+1)*2/3; i++) { mPath.push_back(TILE[mIndexY+i][mIndexX - i]); }
		}
		else if (angle >= (9 * PI / 8) and angle < (11 * PI / 8)) //좌하향
		{
			for (int i = 0; i < dist + 1; i++) { mPath.push_back(TILE[mIndexY + i][mIndexX]); }
		}
		else if (angle >= (11 * PI / 8) and angle < (13 * PI / 8)) //하향
		{
			for (int i = 0; i < dist + 1; i++) { mPath.push_back(TILE[mIndexY + i][mIndexX+i]); }
		}
		else if (angle >= (13 * PI / 8) and angle < (15 * PI / 8)) //우하향
		{
			for (int i = 0; i < dist + 1; i++) { mPath.push_back(TILE[mIndexY][mIndexX + i]); }
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
