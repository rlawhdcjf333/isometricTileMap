#include "pch.h"
#include "MapToolScene.h"
#include "Button.h"
#include "Command.h"
#include "Tile.h"
#include "Pallete.h"
#include "MapObject.h"
#include "MapObjectPallete.h"
#include "Camera.h"

void MapToolScene::RegisterCommand(ICommand* command)
{
	command->Execute();
	if (mCurrentLayer==Layer::Object) {
		mObjectCommandList.emplace(command);
	}
	else if (mCurrentLayer==Layer::Tile) {
		mCommandList.emplace(command);
	}
}

void MapToolScene::Save()
{
	ofstream saveStream(L"../04_Data/Tile.txt");
	if (saveStream.is_open())
	{
		for (int y = 0; y < mTileList.size(); ++y)
		{
			for (int x = 0; x < mTileList[y].size(); ++x)
			{
				string str;
				wstring keyName;
				if (mTileList[y][x]->GetImage() != nullptr)
					keyName = mTileList[y][x]->GetImage()->GetKeyName();
				str.assign(keyName.begin(), keyName.end());

				saveStream << str.c_str();
				saveStream << ',';
				saveStream << mTileList[y][x]->GetFrameX();
				saveStream << ',';
				saveStream << mTileList[y][x]->GetFrameY();
				saveStream << ',';
				saveStream << (int)mTileList[y][x]->GetType();
				saveStream << endl;
			}
		}
	}
	saveStream.close();
	saveStream.open(L"../04_Data/Object.txt");
	if (saveStream.is_open())
	{
		for ( MapObject* elem: mMapObjectList)
		{
			string str;
			wstring keyName;
			if (elem->GetImageKey() != L"")
				keyName = elem->GetImageKey();
			str.assign(keyName.begin(), keyName.end());

			saveStream << str.c_str();
			saveStream << ',';
			saveStream << elem->GetX();
			saveStream << ',';
			saveStream << elem->GetY();
			saveStream << endl;
		}
	}
}

void MapToolScene::Load()
{
	ifstream loadStream(L"../04_Data/Tile.txt");
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

				getline(loadStream, buffer,',');
				key = buffer;
				getline(loadStream, buffer,',');
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
	loadStream.close(); //위에 열러있는 txt 파일을 닫아 연결을 해제한다.
	//다른 파일을 저장 읽을 시 하나의 파일만 이용 가능
	//원래 open을 하면 자동으로 닫혀야 하지만 이상하게 닫히지 않아서 close를 써서 강제로 해제 중
	loadStream.open(L"../04_Data/Object.txt");
	if (loadStream.is_open()) //파일을 정상적으로 열었는지 확인
	{
		vector <MapObject*> tmp;
		while (loadStream.peek() != EOF) {
			string key;
			int x;
			int y;
			int type;
			string buffer;
			getline(loadStream, buffer, ',');
			key = buffer;
			getline(loadStream, buffer, ',');
			x = stoi(buffer);
			getline(loadStream, buffer);
			y = stoi(buffer);
			wstring wstr;
			wstr.assign(key.begin(), key.end());
			MapObject* mapObject = new MapObject(IMAGEMANAGER->FindImage(wstr), x, y);
			tmp.emplace_back(mapObject);
		 }

		mMapObjectList.assign(tmp.begin(), tmp.end());
	}
}

void MapToolScene::Undo()
{
	if (mCurrentLayer == Layer::Object) {
		if (mObjectCommandList.empty()) return;

		mObjectRedoList.emplace(mObjectCommandList.top());
		mObjectCommandList.top()->Undo();
		mObjectCommandList.pop();
	}
	else if (mCurrentLayer == Layer::Tile) {
		if (mCommandList.empty()) return;

		mRedoList.emplace(mCommandList.top());
		mCommandList.top()->Undo();
		mCommandList.pop();
	}
}

void MapToolScene::Redo()
{
	if (mCurrentLayer == Layer::Object) {
		if (mObjectRedoList.empty()) return;

		mObjectCommandList.emplace(mObjectRedoList.top());
		mObjectRedoList.top()->Redo();
		mObjectRedoList.pop();
	}
	else if (mCurrentLayer == Layer::Tile) {
		if (mRedoList.empty()) return;

		mCommandList.emplace(mRedoList.top());
		mRedoList.top()->Redo();
		mRedoList.pop();
	}
}

void MapToolScene::ChangeLayer()
{
	mCurrentLayer = Layer((int)mCurrentLayer + 1);
	if (mCurrentLayer == Layer::End)
	{
		mCurrentLayer = Layer::Tile;
	} 
	mCurrentPallete = nullptr;
}

void MapToolScene::stackClear(stack<ICommand*>* stack)
{
	while (!stack->empty()) {
		stack->pop();
	}
}

void MapToolScene::Init()
{
	IMAGEMANAGER->LoadFromFile(L"Tiles", Resources(L"tile_test.bmp"), 372, 372, 3,6, true);
	IMAGEMANAGER->LoadFromFile(L"Tree2", ResourcesObject(L"Tree2.bmp"), 86, 142, true);
	IMAGEMANAGER->LoadFromFile(L"Tree3", ResourcesObject(L"Tree3.bmp"), 92, 190, true);
	IMAGEMANAGER->LoadFromFile(L"Tree9", ResourcesObject(L"Tree9.bmp"), 344, 290, true);
	IMAGEMANAGER->LoadFromFile(L"Bush6", ResourcesObject(L"Bush6.bmp"), 57, 48, true);
	IMAGEMANAGER->LoadFromFile(L"Stage1DeactivateHeadDoor", ResourcesObject(L"Stage1DeactivateHeadDoor.bmp"), 176, 129, true);
	IMAGEMANAGER->LoadFromFile(L"Stage1DeactivateItemDoor", ResourcesObject(L"Stage1DeactivateItemDoor.bmp"), 199, 129, true);
	IMAGEMANAGER->LoadFromFile(L"Stage1DeactivateMoneyDoor", ResourcesObject(L"Stage1DeactivateMoneyDoor.bmp"), 215, 159, true);
	IMAGEMANAGER->LoadFromFile(L"witch", ResourcesObject(L"witch.bmp"), 90, 90, true);
	mImage = IMAGEMANAGER->FindImage(L"Tiles");

	for (int y = 0; y < 75; y++) // 타일 레이어 도화지
	{
		vector <Tile*> tmp;
		for (int x = 0; x < 75; x++)
		{
			tmp.push_back
			( 
				new Tile
				(
				mImage,
				StartX+(x-y)*TileSizeX/2,
				StartY+(x+y)*TileSizeY/2, 
				2,
				4,
				TileSizeX,
				TileSizeY,
				x,
				y
				)
			);
		}
		mTileList.push_back(tmp);
	}
	for (int y = 0; y < 3; y++) //타일 팔레트
	{
		vector <Pallete*> tmp;
		for (int x = 0; x < 5; x++)
		{
			tmp.push_back(
				new Pallete
				(
					mImage,
					900 + 50 * x,
					100 + 25 * y,
					y,
					x,
					50,
					25
				)
			);
		}
		mPalleteList.push_back(tmp);
	}
	vector <MapObjectPallete*> tmp;
	tmp.push_back(new MapObjectPallete(IMAGEMANAGER->FindImage(L"Tree2"), 300, 70));
	tmp.push_back(new MapObjectPallete(IMAGEMANAGER->FindImage(L"Tree3"), 350, 70));
	tmp.push_back(new MapObjectPallete(IMAGEMANAGER->FindImage(L"Tree9"), 400, 70));
	tmp.push_back(new MapObjectPallete(IMAGEMANAGER->FindImage(L"Bush6"), 450, 70));
	tmp.push_back(new MapObjectPallete(IMAGEMANAGER->FindImage(L"Stage1DeactivateHeadDoor"), 500, 70));
	tmp.push_back(new MapObjectPallete(IMAGEMANAGER->FindImage(L"Stage1DeactivateItemDoor"), 550, 70));
	tmp.push_back(new MapObjectPallete(IMAGEMANAGER->FindImage(L"Stage1DeactivateMoneyDoor"), 600, 70));
	tmp.push_back(new MapObjectPallete(IMAGEMANAGER->FindImage(L"witch"), 650, 70));
	mMapObjectPallete.push_back(tmp);

	mPalletRc = RectMake(900, 100, 5 * 50, 3 * 25);
	mSave = new Button(L"저장", 150, 25, 200, 50, [this]() {Save();});
	mLoad = new Button(L"불러오기", 360, 25, 200, 50, [this]() {Load();});
	mUndo = new Button(L"되돌리기", 570, 25, 200, 50, [this]() {Undo();});
	mRedo = new Button(L"되돌리기 취소", 780, 25, 200, 50, [this]() {Redo();});
	mNext = new Button(L"게임로드", 990, 25, 200, 50, []() {SceneManager::GetInstance()->LoadScene(L"GameScene");});
	mChangeLayer = new Button(L"레이어 변경", 150, 80, 200, 50, [this]() {ChangeLayer(); });
	mMenuRc = RectMake(0, 0, 1280, 180);

	CAMERA->ChangeMode(Camera::Mode::Free);
	CAMERA->SetX(StartX);
	CAMERA->SetY(StartY);
	CAMERA->SetMoveSpeed(20.f);

}

void MapToolScene::Release()
{

	for (int y = 0; y < mTileList.size(); y++)
	{
		for (int x = 0; x < mTileList[y].size(); x++)
		{
			SafeDelete(mTileList[y][x]);
		}
	}
	mTileList.clear();
	for (int y = 0; y < mPalleteList.size(); y++)
	{
		for (int x = 0; x < mPalleteList[y].size(); x++)
		{
			SafeDelete(mPalleteList[y][x]);
		}
	}
	mPalleteList.clear();
	for (MapObject* elem : mMapObjectList)
	{
		SafeDelete(elem);
	}
	mMapObjectList.clear();
	while (!mCommandList.empty())
	{
		auto iter = mCommandList.top();
		SafeDelete(iter);
		mCommandList.pop();
	}

	while (!mRedoList.empty())
	{
		auto iter = mRedoList.top();
		SafeDelete(iter);
		mRedoList.pop();
	}
	mRenderList.clear();
	SafeDelete(mSave);
	SafeDelete(mLoad);
	SafeDelete(mUndo);
	SafeDelete(mRedo);
	SafeDelete(mChangeLayer);
	SafeDelete(mNext);
}

void MapToolScene::Update()
{
	mCurrentTile = nullptr;

	int x = CAMERA->CameraMouseY() / TileSizeY + CAMERA->CameraMouseX() / TileSizeX - (StartX / TileSizeX + StartY / TileSizeY);
	int y = CAMERA->CameraMouseY() / TileSizeY - CAMERA->CameraMouseX() / TileSizeX + (StartX / TileSizeX - StartY / TileSizeY);
	int offsetX = CAMERA->CameraMouseX() % TileSizeX;
	int offsetY = CAMERA->CameraMouseY() % TileSizeY;


	if (offsetY < TileSizeY / 2 - offsetX / 2) { x--; }
	if (offsetY < offsetX / 2 - TileSizeY / 2) { y--; }
	if (offsetY > offsetX / 2 + TileSizeY / 2) { y++; }
	if (offsetY > 3 * TileSizeY / 2 - offsetX / 2) { x++; }
	
	if (x >= 0 and x < mTileList.size() and y >= 0 and y < mTileList.size())
	{
		mCurrentTile = mTileList[y][x];
	}

	if (INPUT->GetKeyDown(VK_TAB)) {
		mTabKey = !mTabKey;
	}
	if (INPUT->GetKeyDown(VK_ESCAPE)) {
		mRenderToggle = !mRenderToggle;
	}

	if (mCurrentLayer != Layer::Tile) mCurrentPallete= nullptr;	//레이어 바꿀 때 마다 현재 들고 있는 팔레트 초기화
	if (mCurrentLayer != Layer::Object) mCurrentMapObjectPallete = nullptr;

	if (mCurrentPallete and INPUT->GetKeyDown(VK_RBUTTON)) //타일 타입 바꾸기
	{
		mCurrentPallete->SetType(TileType((int)mCurrentPallete->GetType() + 1));
		if (mCurrentPallete->GetType() == TileType::End)
		{
			mCurrentPallete->SetType(TileType::Normal);
		}
	}
	if (mCurrentTile and mCurrentPallete)
	{
		if (!PtInRect(&mPalletRc, nonC_mousePosition) and !PtInRect(&mMenuRc, nonC_mousePosition) and Input::GetInstance()->GetKey(VK_LBUTTON))
		{
			if (mCurrentTile->GetImage() != mCurrentPallete->GetImage() or
				mCurrentTile->GetFrameX() != mCurrentPallete->GetFrameX() or
				mCurrentTile->GetFrameY() != mCurrentPallete->GetFrameY() or
				mCurrentTile->GetType() != mCurrentPallete->GetType())
			{
				IBrushHandle* command = new IBrushHandle(mCurrentTile, mCurrentPallete);
				RegisterCommand(command);
			}
		}
	}
	if (mCurrentMapObjectPallete)
	{
		if (!PtInRect(&mPalletRc, nonC_mousePosition) and !PtInRect(&mMenuRc, nonC_mousePosition) and Input::GetInstance()->GetKeyUp(VK_LBUTTON))
		{
			IMapObject* command = new IMapObject(_mousePosition, mCurrentMapObjectPallete,&mMapObjectList);
			RegisterCommand(command);
		}
	}
	if (!mTabKey) {
		if (mCurrentLayer == Layer::Tile)
		{
			for (auto elem : mPalleteList)
			{
				for (auto elemelem : elem)
				{
					if (PtInRect(elemelem->GetRect(), nonC_mousePosition) and Input::GetInstance()->GetKeyUp(VK_LBUTTON))
					{
						mCurrentPallete = elemelem;
					}
				}
			}
		}
		else if (mCurrentLayer == Layer::Object)
		{
			for (auto elem : mMapObjectPallete)
			{
				for (auto elemelem : elem)
				{
					RECT rc = elemelem->GetRect();
					if (PtInRect(&rc, nonC_mousePosition) and Input::GetInstance()->GetKeyUp(VK_LBUTTON))
					{
						mCurrentMapObjectPallete = elemelem;
					}
				}
			}

		
		}

		function <bool(MapObject* a, MapObject* b)> comp = [](MapObject* a, MapObject* b) 
		{ 
			return a->GetY() < b->GetY();
		};
		mRenderList.assign(mMapObjectList.begin(), mMapObjectList.end());
		sort(mRenderList.begin(), mRenderList.end(), comp);

		mSave->Update();
		mLoad->Update();
		mUndo->Update();
		mRedo->Update();
		mChangeLayer->Update();
		mNext->Update();

	}
}

void MapToolScene::Render(HDC hdc)
{
	RECT cameraRect = CAMERA->GetRect();
	float left = cameraRect.left;
	float top = cameraRect.top;

	int x = (int)top / TileSizeY + (int)left / TileSizeX - (StartX / TileSizeX + StartY / TileSizeY);	//카메라 레프트 값에 대한 대략적인 인덱스값
	int y = (int)top / TileSizeY - (int)left / TileSizeX + (StartX / TileSizeX - StartY / TileSizeY);
	int offsetX = (int)left % TileSizeX;
	int offsetY = (int)top % TileSizeY;

	if (offsetY < TileSizeY / 2 - offsetX / 2) { x--; }
	if (offsetY < offsetX / 2 - TileSizeY / 2) { y--; }
	if (offsetY > offsetX / 2 + TileSizeY / 2) { y++; }
	if (offsetY > 3 * TileSizeY / 2 - offsetX / 2) { x++; }

	



	//for (int y = 0; y < mTileList.size(); y++)//타일
	//{
	//	for (int x = 0; x < mTileList[y].size(); x++)
	//	{
	//		if (mCurrentLayer == Layer::Tile) {
	//			mTileList[y][x]->Render(hdc);
	//		}
	//		else {
	//			mTileList[y][x]->AlphaRender(hdc);
	//		}
	//		if (!mRenderToggle) {
	//			mTileList[y][x]->SelectRenderMargenta(hdc);
	//		}
	//	}
	//}
	if (x >= 1 and x < 75 and y >= 1 and y < 75)
	{
		if (mTileList[y - 1][x - 1])
		{
			//일단 카메라기준으로 레프트탑 타일을 가져옴 융통성 있게 x좌표 만큼 좀 뺌 (22,24)
			for (int j = 0; j < 26; j++)
			{
				for (int i = 0; i < 23; i++)
				{
					if (y - 1 - i + j < 0 || x - 1 + i + j>74 ||
						y - 1 - i + j > 74 || x - 1 + i + j < 0 ||
						y - 1 - i + j + 1 < 0 || y - 1 - i + j + 1 > 74)
						continue;
					if (mCurrentLayer == Layer::Tile)
					{
						mTileList[y - 1 - i + j][x - 1 + i + j]->Render(hdc);
						mTileList[y - 1 - i + j + 1][x - 1 + i + j]->Render(hdc);
					}
					else
					{
						mTileList[y - 1 - i + j][x - 1 + i + j]->AlphaRender(hdc);
						mTileList[y - 1 - i + j + 1][x - 1 + i + j]->AlphaRender(hdc);
					}

				}
			}
		}
	}

	for (MapObject* elem : mRenderList)//오브젝트
	{
		if (mCurrentLayer == Layer::Object) {
			elem->Render(hdc);
		}
		else {
			elem->AlphaRender(hdc);
		}
	}
	if (mCurrentTile)
	{
		mCurrentTile->SelectRender(hdc);
	}
	RenderRect(hdc, mMenuRc);
	if (mCurrentPallete)
	{
		mCurrentPallete->Render(hdc, _mousePosition.x - 25, _mousePosition.y - 12);
		if (mCurrentPallete->GetType() == TileType::Normal) {
			wstring text = L"Normal";
			TextOut(hdc, 50, 100, text.c_str(), text.length());
		}
		else if (mCurrentPallete->GetType() == TileType::Slow) {
			wstring text = L"Slow";
			TextOut(hdc, 50, 100, text.c_str(), text.length());
		}
		else if (mCurrentPallete->GetType() == TileType::Block) {
			wstring text = L"Block";
			TextOut(hdc, 50, 100, text.c_str(), text.length());
		}
	}
	
	if (mCurrentMapObjectPallete)
	{
		mCurrentMapObjectPallete->Render(hdc, _mousePosition.x - 25, _mousePosition.y - 12);
	}
	if (mCurrentLayer == Layer::Object) {
		wstring text = L"Object Layer";
		TextOut(hdc, 50, 150, text.c_str(), text.length());
	}
	else if (mCurrentLayer==Layer::Tile) {
		wstring text = L"Tile Layer";
		TextOut(hdc, 50, 150, text.c_str(), text.length());
	}
	
	if (!mTabKey) {
		mSave->Render(hdc);
		mLoad->Render(hdc);
		mUndo->Render(hdc);
		mRedo->Render(hdc);
		mNext->Render(hdc);
		mChangeLayer->Render(hdc);

		if (mCurrentLayer == Layer::Tile)
		{
			for (int y = 0; y < mPalleteList.size(); y++)
			{
				for (int x = 0; x < mPalleteList[y].size(); x++)
				{
					mPalleteList[y][x]->Render(hdc);
				}
			}
		}
		else if (mCurrentLayer == Layer::Object)
		{
			for (int y = 0; y < mMapObjectPallete.size(); y++)
			{
				for (int x = 0; x < mMapObjectPallete[y].size(); x++)
				{
					mMapObjectPallete[y][x]->Render(hdc);
				}
			}
		}
	}

	
}
