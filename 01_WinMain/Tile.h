#pragma once

class Image;
enum class TileType : int
{
	Normal = 1,
	Block = 2,
	Slow = 3,
};

class Tile
{

	Image* mImage;
	float mX;
	float mY;
	int mFrameX;
	int mFrameY;
	int mSizeX;
	int mSizeY;
	int mIndexX;
	int mIndexY;
	RECT mRect;
	Diam mDiam;
	TileType mTileType;


public:

	Tile(Image* pImage,  float x, float y, int frameX, int frameY, int sizeX, int sizeY, int indexX, int indexY);
	void Render(HDC hdc);
	void SelectRender(HDC hdc);
	void SelectRenderBlue(HDC hdc);

	Image* GetImage() { return mImage; }
	void SetImage(Image* pImage) { mImage = pImage; }
	
	RECT GetRect() { return mRect; }
	
	int GetFrameX() { return mFrameX;}
	void SetFrameX(int val) { mFrameX = val; }
	int GetFrameY() { return mFrameY; }
	void SetFrameY(int val) { mFrameY = val; }

	int GetIndexX() { return mIndexX; }
	int GetIndexY() { return mIndexY; }


	float GetX() { return mX; }
	float GetY() { return mY; }

	void SetType(TileType val) { mTileType = val; }
	TileType GetType() { return mTileType; }

	
};