#include "pch.h"
#include "TileList.h"
#include "Tile.h"

int TileList::CalcTileX(Tile* tile) {
	int x = tile->GetX();
	int y = tile->GetY();
	int tileX = y / TileSizeY + x / TileSizeX - (StartX / TileSizeX + StartY / TileSizeY);
	int tileY = y / TileSizeY - x / TileSizeX + (StartX / TileSizeX - StartY / TileSizeY);
	int offsetX = x % TileSizeX;
	int offsetY = y % TileSizeY;

	if (offsetY < TileSizeY / 2 - offsetX / 2) { tileX--; }
	if (offsetY < offsetX / 2 - TileSizeY / 2) { tileY--; }
	if (offsetY > offsetX / 2 + TileSizeY / 2) { tileY++; }
	if (offsetY > 3 * TileSizeY / 2 - offsetX / 2) { tileX++; }

	return tileX;
}
int TileList::CalcTileY(Tile* tile) {
	int x = tile->GetX();
	int y = tile->GetY();
	int tileX = y / TileSizeY + x / TileSizeX - (StartX / TileSizeX + StartY / TileSizeY);
	int tileY = y / TileSizeY - x / TileSizeX + (StartX / TileSizeX - StartY / TileSizeY);
	int offsetX = x % TileSizeX;
	int offsetY = y % TileSizeY;

	if (offsetY < TileSizeY / 2 - offsetX / 2) { tileX--; }
	if (offsetY < offsetX / 2 - TileSizeY / 2) { tileY--; }
	if (offsetY > offsetX / 2 + TileSizeY / 2) { tileY++; }
	if (offsetY > 3 * TileSizeY / 2 - offsetX / 2) { tileX++; }

	return tileY;
}