#include "pch.h"
#include "PathFinder.h"

#include "Tile.h"
#include <algorithm>	//reverse�Լ� ��� ���İ��� �Լ��� �� �̾ȿ� ����

int PathFinder::CalcHeuristic(int idX1, int idY1, int idX2, int idY2)
{
	int dx = abs(idX1 - idX2);
	int dy = abs(idY1 - idY2);
	int result = dx + dy;
	return result;
}

vector<class Tile*> PathFinder::FindPath(const vector<vector<class Tile*>>& tileList,
	int startIndexX, int startIndexY, int arrivalX, int arrivalY)
{
	vector<Tile*> result;	//����� ���� ���� 

	//�������� ������ ���ٸ� �߸�ȣ��� ��
	if (startIndexX == arrivalX && startIndexY == arrivalY)return result;
	
	int tileCountY = tileList.size();	//���� Ÿ�� �� 
	int tileCountX = tileList[0].size();	//���� Ÿ�� �� 

	// {{ �ش� �Լ��� ���� �߸� �־��ٸ� 
	if (startIndexX < 0 || startIndexX >= tileCountX)return result;
	if (startIndexY < 0 || startIndexY >= tileCountY)return result;
	if (arrivalX < 0 || arrivalX >= tileCountY)return result;
	if (arrivalY < 0 || arrivalY >= tileCountY)return result;
	// }} 

	vector<vector<DummyTile>> dummyList;	// ���̽�Ÿ ���� ������ ���� Ÿ�� ����Ʈ
	// {{ 2���� ���� Ÿ�� ���� ~
	//ù��° ���ڷ� ���� �� ��ŭ �ι�° ���ڷ� ���� ���� �����ؼ� �ִ´� 
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

	//�켱���� ť�� �����ϸ� �� ������
	vector<Tile*> openList;	//�� �� �ִ� �� �ĺ����� ���� �༮

	Tile* startTile = tileList[startIndexY][startIndexX];
	Tile* arrivalTile = tileList[arrivalY][arrivalX];
	Tile* currentTile = startTile;

	// {{ ��ã�� ���� 
	while (true)
	{
		//���� �߸��Ǿ���??? 
		if (currentTile == nullptr)return result;
		int currentIndexX = currentTile->GetIndexX();
		int currentIndexY = currentTile->GetIndexY();
		// {{ �ֺ� 8�� Ÿ�� �˻�~

		for (int y = currentIndexY - 1; y <= currentIndexY + 1; ++y)
		{
			if (y < 0 || y >= tileCountY)continue;

			for (int x = currentIndexX - 1; x <= currentIndexX + 1; ++x)
			{
				if (x < 0 || x >= tileCountX)continue;

				if (x == currentIndexX && y == currentIndexY)continue;

				//�밢�� ���� ������ ���⿡ ���� �߰� ~

				if (dummyList[y][x].IsClose == true)continue;

				if (tileList[y][x]->GetType() == TileType::Block)
				{
					dummyList[y][x].IsClose = true;
					dummyList[y][x].IsOpen = true;
					continue;
				}
				//�˻��Ϸ��� Ÿ���� ó�� �˻��ϴ� Ÿ���̶�� 
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

					openList.push_back(tileList[y][x]);
				}
				//ó���� �ƴ϶�� ~ ������� Ȯ���ϴµ� �ٸ� ��θ� ���ؼ� Ȯ���� �Ǿ��ٴ� ��
				else
				{
					int newFromCost =
						dummyList[currentIndexY][currentIndexX].CostFromStart + 1;
					//���� ���� ���ؼ� �� ª�� ��Ʈ�� ����
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

		// {{ openList���� ���� ���� ����� Ÿ�� �˻� ~
		Tile* tileMin = nullptr;

		for (int i = 0; i < openList.size(); ++i)
		{
			if (openList[i] == currentTile)
			{
				openList.erase(openList.begin() + i);
				--i;
				continue;
			}

			if (tileMin == nullptr)
			{
				tileMin = openList[i];
				continue;
			}
			if (dummyList[openList[i]->GetIndexY()][openList[i]->GetIndexX()].CostTotal <
				dummyList[tileMin->GetIndexY()][tileMin->GetIndexX()].CostToEnd)
			{
				tileMin = openList[i];
			}

		}
		// }}

		//������� �Դٴ� ���� ���� ���� ����� Ÿ�� �����Ǿ��ٴ� ��
		if (tileMin != nullptr)
		{
			dummyList[tileMin->GetIndexY()][tileMin->GetIndexX()].IsClose = true;
			currentTile = tileMin;
		}
		//OpenList�� �ƹ��͵� ���ٴ� �� : �� �� �ִ� ���� ����.( Ž���� ������ ����)
		else
		{
			return result;
		}
		// {{ ���� ���� ã�Ҵ� ~!
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

			return result;
		}
		// }}
	}
	// }} 

	return result;
}
