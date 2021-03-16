#pragma once
#include "Scene.h"
enum class Layer : int {
	Tile,
	Object,
	End
};
class Tile;
class Button;
class Pallete;
class ICommand;
class MapToolScene  : public Scene
{
	Image* mImage;

	vector <vector<Tile*>> mTileList;
	vector <vector<Tile*>> mObjectList;
	vector <vector<Pallete*>> mPalleteList;

	Tile* mCurrentTile;
	Pallete* mCurrentPallete;
	

	Button* mSave;
	Button* mLoad;
	Button* mUndo;
	Button* mRedo;
	Button* mNext;
	Button* mChangeLayer;

	stack <ICommand*>  mCommandList;
	stack <ICommand*> mRedoList;
	RECT mPalletRc;
	Layer mIsLayer = Layer::Tile;
	
	bool mTabKey = false;
	bool mRenderToggle = false;
private:
	
	void RegisterCommand(ICommand* command);
	void Save();
	void Load();
	void Undo();
	void Redo();
	void ChangeLayer();
public:

	void Init()override;
	void Release()override;
	void Update()override;
	void Render(HDC hdc) override;
};