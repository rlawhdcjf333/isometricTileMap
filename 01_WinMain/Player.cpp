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

	if (INPUT->GetKeyDown('D'))
	{
		Attack4Direction(mAttackRange);
	}

	Dash(5);

	if (mIsDash)
	{
		Move(5*mInitSpeed); //�뽬 �� 3�� ����
	}
	else
	{
		if (Input::GetInstance()->GetKey(VK_RBUTTON) and mTileSelect) //�뽬 ���� �ƴ� ��쿡�� Ŭ�� �̵� Ȱ��ȭ == �뽬 �߿� �̵�ĵ�� �ȵ�
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

	//{{ �����ڿ� Ÿ�� üũ ������
	TILE[mIndexY][mIndexX]->SelectRender(hdc);

	for (Tile* elem : mPath)
	{
		elem->SelectRender(hdc);
	}

	for (Tile* elem : mAttackRange)
	{
		elem->SelectRenderBlue(hdc);
	}
	//}}
}

void Player::Move(float speed)
{
	if (!mPath.empty()) {
		if (mPath.size() <= mPathIndex) //���������� �̵� �Ϸ�
		{
			mPath.clear();
			mPathIndex = 1; 
			mIsDash = false;
		}
		else //�̵� ��
		{
			float pathX = mPath[mPathIndex]->GetX() + TileSizeX / 2;
			float pathY = mPath[mPathIndex]->GetY() + TileSizeY / 2;
			mAngle = Math::GetAngle(mX, mY, pathX, pathY); //�ޱ� �Ÿ� ����� 0�� ���ö� ���� 0���� ����
			mX += speed * cosf(mAngle) * dTime;
			mY -= speed * sinf(mAngle) * dTime;

			if (abs(mX - pathX) < speed * dTime and abs(mY - pathY) < speed * dTime) //���� ����
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
	//Ŀ�� �������� int dist�� Ÿ�� ��� ũ�⸸ŭ �뽬
	if (INPUT->GetKeyDown(VK_SPACE))
	{
		mPath.clear();
		float angle = Math::GetAngle(mX, mY, CAMERA->CameraMouseX(), CAMERA->CameraMouseY());
		
		if (angle >= PI2-(PI/8) or angle <(PI/8)) //���� ->�������� ���� ���� ���������� �뽬 �Ÿ� ����
		{
			for (int i = 0; i < (dist+1)*2/3; i++) {
				if (mIndexX + i < TILESizeX and mIndexY - i >= 0)
				{
					mPath.push_back(TILE[mIndexY - i][mIndexX + i]);
				}
			}
		}
		else if (angle >= (PI / 8) and angle < (3 * PI / 8)) //�����
		{
			for (int i = 0; i < dist + 1; i++) { 
				if (mIndexY - i >= 0 and mIndexY - i < TILESizeY)
				{
					mPath.push_back(TILE[mIndexY - i][mIndexX]);
				}
			}
		}
		else if (angle >= (3 * PI / 8) and angle < (5 * PI / 8)) //����
		{
			for (int i = 0; i < dist + 1; i++) { 
				if (mIndexX - i >= 0 and mIndexY - i >= 0)
				{
					mPath.push_back(TILE[mIndexY - i][mIndexX - i]);
				}
			}
		}
		else if (angle >= (5 * PI / 8) and angle < (7 * PI / 8)) //�»���
		{
			for (int i = 0; i < dist + 1; i++) {
				if (mIndexX - i >= 0 )
				{
					mPath.push_back(TILE[mIndexY][mIndexX - i]);
				}
			}
		}
		else if (angle >= (7 * PI / 8) and angle < (9 * PI / 8)) //����  ->�������� ���� ���� ���������� �뽬�Ÿ� ����
		{
			for (int i = 0; i < (dist+1)*2/3; i++) {
				if (mIndexX - i >= 0 and mIndexY + i < TILESizeY)
				{
					mPath.push_back(TILE[mIndexY + i][mIndexX - i]);
				}
			}
		}
		else if (angle >= (9 * PI / 8) and angle < (11 * PI / 8)) //������
		{
			for (int i = 0; i < dist + 1; i++) { 
				if (mIndexY + i < TILESizeY)
				{
					mPath.push_back(TILE[mIndexY + i][mIndexX]);
				}
			}
		}
		else if (angle >= (11 * PI / 8) and angle < (13 * PI / 8)) //����
		{
			for (int i = 0; i < dist + 1; i++) { 
				if (mIndexX + i < TILESizeX and mIndexY +i < TILESizeY)
				{
					mPath.push_back(TILE[mIndexY + i][mIndexX + i]);
				}
			}
		}
		else if (angle >= (13 * PI / 8) and angle < (15 * PI / 8)) //������
		{
			for (int i = 0; i < dist + 1; i++) { 
				if (mIndexX + i < TILESizeX)
				{
					mPath.push_back(TILE[mIndexY][mIndexX + i]);
				}
			}
		}
		
		for (Tile* elem : mPath) //�뽬 ��ο� ���� ���� �����ϸ� ���� Ÿ�� ���������� �뽬
		{
			if (elem->GetType() == TileType::Block)
			{
				mPath.assign(mPath.begin(), find(mPath.begin(), mPath.end(), elem));
			}
		}

		mPathIndex = 1; mIsDash = true;

	}

}

void Player::Attack4Direction(vector<Tile*>& attackRange)
{
	attackRange.clear();

	float angle = Math::GetAngle(mX, mY, CAMERA->CameraMouseX(), CAMERA->CameraMouseY());

	if (angle > 7 * PI / 4 or angle <= PI / 4) //����
	{
		attackRange.push_back(TILE[mIndexY - 1][mIndexX - 1]);
		attackRange.push_back(TILE[mIndexY - 1][mIndexX]);
		attackRange.push_back(TILE[mIndexY - 1][mIndexX + 1]);
		attackRange.push_back(TILE[mIndexY][mIndexX + 1]);
		attackRange.push_back(TILE[mIndexY + 1][mIndexX + 1]);
	}
	else if (angle > PI / 4 and angle <= 3 * PI / 4) //���
	{
		attackRange.push_back(TILE[mIndexY - 1][mIndexX - 1]);
		attackRange.push_back(TILE[mIndexY - 1][mIndexX]);
		attackRange.push_back(TILE[mIndexY - 1][mIndexX + 1]);
		attackRange.push_back(TILE[mIndexY][mIndexX - 1]);
		attackRange.push_back(TILE[mIndexY + 1][mIndexX - 1]);

	}
	else if (angle > 3 * PI / 4 and angle <= 5 * PI / 4) //����
	{
		attackRange.push_back(TILE[mIndexY - 1][mIndexX - 1]);
		attackRange.push_back(TILE[mIndexY][mIndexX-1]);
		attackRange.push_back(TILE[mIndexY +1][mIndexX - 1]);
		attackRange.push_back(TILE[mIndexY+1][mIndexX]);
		attackRange.push_back(TILE[mIndexY + 1][mIndexX + 1]);
	}
	else if(angle>5*PI/4 and angle<= 7*PI/4)//�Ϲ�
	{
		attackRange.push_back(TILE[mIndexY + 1][mIndexX - 1]);
		attackRange.push_back(TILE[mIndexY + 1][mIndexX]);
		attackRange.push_back(TILE[mIndexY + 1][mIndexX + 1]);
		attackRange.push_back(TILE[mIndexY][mIndexX + 1]);
		attackRange.push_back(TILE[mIndexY-1][mIndexX + 1]);
	}

}
