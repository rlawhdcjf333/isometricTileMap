#include "pch.h"
#include "GameScene.h"
#include "Player.h"
#include "Tile.h"
#include "Dumb.h"
void GameScene::Init()
{
	MapLoad();
	mPlayer = new Player(30,30, 30,30);
	mPlayer->Init();
	
	ObjectManager::GetInstance()->AddObject(ObjectLayer::Enemy,new Dumb());
	CAMERA->ChangeMode(Camera::Mode::Follow);
	CAMERA->SetTarget(mPlayer);
}

void GameScene::Update()
{
	ObjectManager::GetInstance()->Update();
	mPlayer->Update();
}

void GameScene::Render(HDC hdc)
{

	for (int y = 0; y < mTileList.size(); y++)
	{
		for (int x = 0; x < mTileList.size(); x++)
		{
			mTileList[y][x]->Render(hdc);
		}
	}
		
	mPlayer->Render(hdc);

	SetBkMode(hdc, TRANSPARENT);
	TextOut(hdc, 800, 100, L"이동: 우클릭, 꾹 누르고 있어도 됨.", 21);
	ObjectManager::GetInstance()->Render(hdc);
}

void GameScene::Release()
{
	for (auto elem : mTileList)
	{
		for (auto elemelem : elem)
		{
			SafeDelete(elemelem);
		}
	}


	SafeDelete(mPlayer);
}

void GameScene:: MapLoad()
{
	for (int y = 0; y < 75; y++)
	{
		vector <Tile*> tmp;
		for (int x = 0; x < 75; x++)
		{
			tmp.push_back
			(
				new Tile
				(
					IMAGEMANAGER->FindImage(L"Tiles"),
					StartX + (x - y) * TileSizeX / 2,
					StartY + (x + y) * TileSizeY / 2,
					2,
					3,
					TileSizeX,
					TileSizeY,
					x,
					y
				)
			);
		}
		mTileList.push_back(tmp);
	}

	ifstream loadStream(L"../04_Data/Test.txt");
	if (loadStream.is_open())
	{
		for (int y = 0; y < mTileList.size(); ++y)
		{
			for (int x = 0; x < mTileList[y].size(); ++x)
			{
				string key;
				int frameX;
				int frameY;
				int type;
				string buffer;

				getline(loadStream, buffer, ',');
				key = buffer;
				getline(loadStream, buffer, ',');
				frameX = stoi(buffer);
				getline(loadStream, buffer, ',');
				frameY = stoi(buffer);
				getline(loadStream, buffer);
				type = stoi(buffer);

				wstring wstr;
				wstr.assign(key.begin(), key.end());
				mTileList[y][x]->SetImage(IMAGEMANAGER->FindImage(wstr));
				mTileList[y][x]->SetFrameX(frameX);
				mTileList[y][x]->SetFrameY(frameY);
				mTileList[y][x]->SetType((TileType)type);
			}
		}
	}

	TileList::GetInstance()->SetMap(mTileList);
}