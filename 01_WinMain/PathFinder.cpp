#include "pch.h"
#include "PathFinder.h"

#include "Tile.h"
#include <algorithm>	//reverseÇÔ¼ö µîµî Á¤·Ä°ü·Ã ÇÔ¼öµé ´Ù ÀÌ¾È¿¡ ÀÖÀ½

int PathFinder::CalcHeuristic(int idX1, int idY1, int idX2, int idY2)
{
	int dx = abs(idX1 - idX2);
	int dy = abs(idY1 - idY2);
	int result = dx + dy; //ÃÖ´ë·Î ´Ü¼øÈ­ÇÑ Manhattan Heuristic. »ç¹æ ÀÌµ¿ ÈŞ¸®½ºÆ½ °è»ê¿¡¸¸ »ç¿ëÇÏÁö¸¸ iso¿¡¼­´Â »ç¹æ ÀÌµ¿ÀÌ °ğ ´ë°¢ÀÌµ¿ÀÌ´Ù.
	return result;
}

bool PathFinder::FindPath(const vector<vector<class Tile*>>& tileList, vector<Tile*>& output,
	int startIndexX, int startIndexY, int arrivalX, int arrivalY)
{
	vector<Tile*> result;	//°á°ú°ª ´ãÀ» º¤ÅÍ 

	//½ÃÀÛÁ¡ÀÌ ³¡Á¡°ú °°´Ù¸é Àß¸øÈ£ÃâµÈ °Í
	if (startIndexX == arrivalX && startIndexY == arrivalY) return false;
	
	int tileCountY = tileList.size();	//¼¼·Î Å¸ÀÏ ¼ö 
	int tileCountX = tileList[0].size();	//°¡·Î Å¸ÀÏ ¼ö 

//	// {{ ÇØ´ç ÇÔ¼ö¿¡ °ªÀ» Àß¸ø ³Ö¾ú´Ù¸é 
//	if (startIndexX < 0 || startIndexX >= tileCountX)return false;
//	if (startIndexY < 0 || startIndexY >= tileCountY)return false;
//	if (arrivalX < 0 || arrivalX >= tileCountY)return false;
//	if (arrivalY < 0 || arrivalY >= tileCountY)return false;
	if(startIndexX+startIndexY<41)return false;
	if (startIndexX + startIndexY > 110)return false;
	if (arrivalX + arrivalY < 41)return false;
	if (arrivalX + arrivalY > 110)return false;
//	// }} 


	//vector<vector<DummyTile>> dummyList;	// ¿¡ÀÌ½ºÅ¸ ¿¬»ê µµ¿ÍÁÙ ´õ¹Ì Å¸ÀÏ ¸®½ºÆ®
	// {{ 2Â÷¿ø ´õ¹Ì Å¸ÀÏ ¼¼ÆÃ ~
	//Ã¹¹øÂ° ÀÎÀÚ·Î µé¾î¿Â ¼ö ¸¸Å­ µÎ¹øÂ° ÀÎÀÚ·Î µé¾î¿Â °ªÀ» º¹»çÇØ¼­ ³Ö´Â´Ù 
	dummyList.assign(tileCountY, vector<DummyTile>());
	for (int i = 0; i < dummyList.size(); ++i)
		dummyList[i].assign(tileCountX, DummyTile());
	// }} 

	dummyList[startIndexY][startIndexX].IsOpen = true;
	dummyList[startIndexY][startIndexX].IsClose = true;
	dummyList[startIndexY][startIndexX].CostFromStart = 0;
	dummyList[startIndexY][startIndexX].CostToEnd =
		CalcHeuristic(startIndexX, startIndexY, arrivalX, arrivalY);
	dummyList[startIndexY][startIndexX].CostTotal =
		dummyList[startIndexY][startIndexX].CostToEnd;

	//¿ì¼±¼øÀ§ Å¥·Î ¼öÁ¤ÇÏ¸é ´õ »¡¶óÁü
	struct comp //costTotalÀÌ °¡Àå ÀÛÀº Ä£±¸¸¦ °¡Àå ¾Õ¿¡ Á¤·ÄÇÏ´Â ºñ±³¿¬»êÁ¶°Ç... ÀÌ°Å ¿Ö ¶÷´Ù´Â ¾Èµé¾î°¡´Â °É±î¿ä
	{
		bool operator() (Tile* a, Tile* b)
		{
			DummyTile first = PathFinder::GetInstance()->dummyList[a->GetIndexY()][a->GetIndexX()];
			DummyTile second = PathFinder::GetInstance()->dummyList[b->GetIndexY()][b->GetIndexX()];

			return first.CostTotal > second.CostTotal;
		}
	};
	priority_queue<Tile*, vector<Tile*>, comp> openList;
	//vector<Tile*> openList;	//°¥ ¼ö ÀÖ´Â ±æ ÈÄº¸µéÀ» ´ãÀ» ³à¼®

	Tile* startTile = tileList[startIndexY][startIndexX];
	Tile* arrivalTile = tileList[arrivalY][arrivalX];
	Tile* currentTile = startTile;

	// {{ ±æÃ£±â ·çÇÁ 
	while (true)
	{
		if (openList.size() > 75) return false;
		//¹º°¡ Àß¸øµÇ¾ú´Ù??? 
		if (currentTile == nullptr)return false;
		int currentIndexX = currentTile->GetIndexX();
		int currentIndexY = currentTile->GetIndexY();
		
		// {{ ÁÖº¯ 8°³ Å¸ÀÏ °Ë»ç~
		for (int y = currentIndexY - 1; y <= currentIndexY + 1; ++y)
		{
			if (y < 0 || y >= tileCountY)continue;

			for (int x = currentIndexX - 1; x <= currentIndexX + 1; ++x)
			{
				if (x < 0 || x >= tileCountX)continue;

				if (x == currentIndexX && y == currentIndexY)continue;

				//´ë°¢¼± ¸·°í ½ÍÀ¸¸é ¿©±â¿¡ Á¶°Ç Ãß°¡ ~

				if (dummyList[y][x].IsClose == true)continue;

				if (tileList[y][x]->GetType() == TileType::Block)
				{
					dummyList[y][x].IsClose = true;
					dummyList[y][x].IsOpen = true;
					continue;
				}

				//ÆÈ¹æ ®c ÇÊ¿äÇÏ¸é ¾²¼À
				//if (tileList[y - 1][x - 1]->GetType() == TileType::Block && tileList[y][x - 1]->GetType() == TileType::Block &&
				//	tileList[y - 1][x]->GetType() == TileType::Block && tileList[y + 1][x - 1]->GetType() == TileType::Block &&
				//	tileList[y - 1][x + 1]->GetType() == TileType::Block && tileList[y + 1][x]->GetType() == TileType::Block &&
				//	tileList[y][x + 1]->GetType() == TileType::Block && tileList[y + 1][x + 2]->GetType() == TileType::Block)
				//{
				//	dummyList[y][x].IsClose = true;
				//	dummyList[y][x].IsOpen = true;
				//	continue;
				//}

				//°Ë»çÇÏ·Á´Â Å¸ÀÏÀÌ Ã³À½ °Ë»çÇÏ´Â Å¸ÀÏÀÌ¶ó¸é 
				if (dummyList[y][x].IsOpen == false)
				{
					dummyList[y][x].IsOpen = true;
					dummyList[y][x].Parent = currentTile;
					dummyList[y][x].CostFromStart =
						dummyList[currentIndexY][currentIndexX].CostFromStart + 1;
					dummyList[y][x].CostToEnd =
						CalcHeuristic(x, y, arrivalX, arrivalY);
					dummyList[y][x].CostTotal =
						dummyList[y][x].CostFromStart + dummyList[y][x].CostToEnd;

					openList.push(tileList[y][x]);
				}
				//Ã³À½ÀÌ ¾Æ´Ï¶ó¸é ~ ¿©±â±îÁö È®ÀåÇÏ´Âµ¥ ´Ù¸¥ °æ·Î¸¦ ÅëÇØ¼­ È®ÀåÀÌ µÇ¾ú´Ù´Â ¶æ
				else
				{
					int newFromCost =
						dummyList[currentIndexY][currentIndexX].CostFromStart + 1;
					//ÀÌÀü ºñ¿ë°ú ºñ±³ÇØ¼­ ´õ ÂªÀº ·çÆ®¸¦ ¼±ÅÃ
					if (newFromCost < dummyList[y][x].CostFromStart)
					{
						dummyList[y][x].CostFromStart = newFromCost;
						dummyList[y][x].CostToEnd =
							dummyList[y][x].CostFromStart + dummyList[y][x].CostToEnd;
						dummyList[y][x].Parent = currentTile;
					}
				}
			}
		}
		// }} 

		// {{ openList¿¡¼­ °¡Àå ÀûÀº ºñ¿ëÀÇ Å¸ÀÏ °Ë»ç ~
		Tile* tileMin = nullptr;

		if (openList.top() == currentTile)
		{
			openList.pop();
		}
		//if (tileMin == nullptr)
		//{
		//	tileMin = openList.top();
		//}
		if(!openList.empty()) //front() empty vector ÀÌ°Å º¸±â ½ÈÀ¸¸é ¸·ÀÚ. ¾ÈÇÏ¸é Æ´Æ´ÀÌ invalid comparator ÀÌ°Åµµ º¼ ¼ö ÀÖ´Ù.
		tileMin = openList.top();

		//for (int i = 0; i < openList.size(); ++i)
		//{
		//	if (openList[i] == currentTile)
		//	{
		//		openList.erase(openList.begin() + i);
		//		--i;
		//		continue;
		//	}
		//
		//	if (tileMin == nullptr)
		//	{
		//		tileMin = openList[i];
		//		continue;
		//	}
		//	if (dummyList[openList[i]->GetIndexY()][openList[i]->GetIndexX()].CostTotal <
		//		dummyList[tileMin->GetIndexY()][tileMin->GetIndexX()].CostToEnd)
		//	{
		//		tileMin = openList[i];
		//	}
		//
		//}


		
		//¿©±â±îÁö ¿Ô´Ù´Â °ÍÀº °¡Àå ÀûÀº ºñ¿ëÀÇ Å¸ÀÏ ¼±Á¤µÇ¾ú´Ù´Â °Í
		if (tileMin != nullptr)
		{
			dummyList[tileMin->GetIndexY()][tileMin->GetIndexX()].IsClose = true;
			currentTile = tileMin;
		}
		//OpenList¿¡ ¾Æ¹«°Íµµ ¾ø´Ù´Â ¶æ : °¥ ¼ö ÀÖ´Â ±æÀÌ ¾ø´Ù.( Å½»öÀÇ ¿©Áö°¡ ¾øÀ½)
		else
		{
			return false;
		}
		// {{ µµÂø ÁöÁ¡ Ã£¾Ò´Ù ~!
		if (tileMin == arrivalTile)
		{
			Tile* temp = tileMin;
			result.push_back(temp);
			while (dummyList[temp->GetIndexY()][temp->GetIndexX()].Parent != nullptr)
			{
				temp = dummyList[temp->GetIndexY()][temp->GetIndexX()].Parent;
				result.push_back(temp);
			}

			reverse(result.begin(), result.end());

			output.assign(result.begin(), result.end());
			return true;
		}
		// }}
	}
	// }} 

	return false;
}
