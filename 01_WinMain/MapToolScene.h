#pragma once
#include "Scene.h"

class Tile;
class Button;
class Pallete;
class ICommand;
class MapToolScene  : public Scene
{
	Image* mImage;

	vector <vector<Tile*>> mTileList;
	vector <vector<Pallete*>> mPalleteList;

	Tile* mCurrentTile;
	Pallete* mCurrentPallete;

	Button* mSave;
	Button* mLoad;
	Button* mUndo;
	Button* mRedo;
	Button* mNext;

	stack <ICommand*>  mCommandList;
	stack <ICommand*> mRedoList;
	RECT mPalletRc;

	bool mTabKey = false;
	bool mRenderToggle = false;
private:
	
	void RegisterCommand(ICommand* command);
	void Save();
	void Load();
	void Undo();
	void Redo();

public:

	void Init()override;
	void Release()override;
	void Update()override;
	void Render(HDC hdc) override;
};