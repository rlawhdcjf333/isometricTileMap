#include "pch.h"
#include "TileSelect.h"

void TileSelect::Init()
{
}

void TileSelect::Release()
{
}

void TileSelect::Update()
{
	mCurrentTile = nullptr;

	int x = _mousePosition.y / TileSizeY + _mousePosition.x / TileSizeX - (StartX / TileSizeX + StartY / TileSizeY);
	int y = _mousePosition.y / TileSizeY - _mousePosition.x / TileSizeX + (StartX / TileSizeX - StartY / TileSizeY);
	int offsetX = _mousePosition.x % TileSizeX;
	int offsetY = _mousePosition.y % TileSizeY;

	if (offsetY < TileSizeY / 2 - offsetX / 2) { x--; }
	if (offsetY < offsetX / 2 - TileSizeY / 2) { y--; }
	if (offsetY > offsetX / 2 + TileSizeY / 2) { y++; }
	if (offsetY > 3 * TileSizeY / 2 - offsetX / 2) { x++; }

	if (x >= 0 and x < TILE.size() and y >= 0 and y < TILE.size())
	{
		mCurrentTile = TILE[y][x];
	}


}

void TileSelect::Render(HDC hdc)
{
	if (mCurrentTile)
	{
		mCurrentTile->SelectRender(hdc);
	}
}
