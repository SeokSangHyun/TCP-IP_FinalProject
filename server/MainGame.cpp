#include "stdafx.h"
#include "MainGame.h"

CMainGame::CMainGame()
{
}

CMainGame::~CMainGame()
{
}

void CMainGame::Initialize(void)
{
	m_bLogin[0] = false;
	m_bLogin[1] = false;

	m_iRoundCount = 1;
	m_iPackCount = 100;
	m_iRadius = 25;

	m_fTime_CreateFruit = 0.f;

	m_GameInfo.iScore_Player1 = 0;
	m_GameInfo.iScore_Player2 = 0;
	m_GameInfo.iFruitInfoCount = 0;
	m_GameInfo.enumGameState = GameState_Wait;
}

void CMainGame::Update(float fElapsedTimeInSecond)
{
	if (m_bLogin[0] == true && m_bLogin[1] == true)				// �� ���� �α����ߴٸ�
	{
		switch (m_GameInfo.enumGameState)						// ���� ���¿� ���� ó��
		{
		case GameState_Wait:									// ó�� �� ���� �α����ߴٸ� �� ���� ���´�. -> break ���� �ʴ´�. (�ٷ� GameState_Term�� ���� �����Ѵ�.)
			m_GameInfo.enumGameState = GameState_Term;			// ���� ���¸� GameState_Term���� �ٲ۴�.
			m_GameInfo.fTime = (float)GameTime_Term;			// ���� �ð��� GameTime_Term���� �ٲ۴�.
		case GameState_Term:
			m_GameInfo.fTime -= fElapsedTimeInSecond;			// �ð� ���

			if (m_GameInfo.fTime <= 0.f)						// �ð��� 0�� �Ǹ�
			{
				m_GameInfo.enumGameState = GameState_Play;		// ���� ���¸� GameState_Play�� �ٲ۴�.
				m_GameInfo.fTime = (float)GameTime_Play;		// ���� �ð��� GameTime_Play�� �ٲ۴�.
			}
			break;
		case GameState_Play:									
			m_GameInfo.fTime -= fElapsedTimeInSecond;			// �ð� ���

			CreateFruit(fElapsedTimeInSecond);					// ���� ����
			AddPos(fElapsedTimeInSecond);						// ���� ��ġ ����

			if (m_GameInfo.fTime <= 0.f)						// �ð��� 0�� �Ǹ�
			{
				InitFruit();									// ���� �ʱ�ȭ (����Ʈ�� �ִ� ��� ���� ����) -> ���尡 ����Ǿ��� ������

				if (m_iRoundCount != 3)							// ���尡 3�� �ƴ϶��
				{
					m_GameInfo.enumGameState = GameState_Term;	// ���� ���¸� GameState_Term���� �ٲ۴�.
					m_GameInfo.fTime = (float)GameTime_Term;	// ���� �ð��� GameTime_Term���� �ٲ۴�.

					m_iRoundCount++;							// ���� ī��Ʈ�� ������Ų��.
					m_iPackCount = m_iRoundCount * 100;			// ���� ���� ������ ������Ų��.
				}
				else											// ���尡 3�̶��
				{
					m_GameInfo.enumGameState = GameState_End;	// ���� ���¸� GameState_End�� �ٲ۴�.
					m_GameInfo.fTime = (float)GameTime_End;		// ���� �ð��� GameTime_End�� �ٲ۴�.
				}
			}
			break;
		case GameState_End:		
			m_GameInfo.fTime -= fElapsedTimeInSecond;			// �ð� ���

			if (m_GameInfo.fTime <= 0.f)						// �ð��� 0�� �Ǹ�
			{
				m_GameInfo.enumGameState = GameState_Term;		// ���� ���¸� GameState_Term���� �ٲ۴�.
				m_GameInfo.fTime = (float)GameTime_Term;		// ���� �ð��� GameTime_Term���� �ٲ۴�.

				m_iRoundCount = 1;								// ���� ī��Ʈ�� �ʱ�ȭ�Ѵ�.
				InitScore();									// ������ �ʱ�ȭ�Ѵ�.
			}
			break;
		default:
			break;
		}
	}
	else														// �� ���� �α������� �ʾҴٸ�
	{
		m_GameInfo.enumGameState = GameState_Wait;				// ���� ���¸� GameState_Wait���� �ٲ۴�.
		m_GameInfo.fTime = (float)GameTime_Wait;				// ���� �ð��� GameTime_Wait���� �ٲ۴�.

		InitFruit();											// ������ �ʱ�ȭ�Ѵ�.
		InitScore();											// ������ �ʱ�ȭ�Ѵ�.
		m_iRoundCount = 1;										// ���带 ī��Ʈ�� �ʱ�ȭ�Ѵ�.
	}
}

float CMainGame::Distance(Point_F A, Point_F B)
{
	float fDistX = B.x - A.x;
	float fDistY = B.y - A.y;

	return sqrtf(fDistX * fDistX + fDistY * fDistY);
}

void CMainGame::CollisionCheck()
{
	list<FruitInfo>::iterator iter = m_listFruitPacket.begin();
	list<FruitInfo>::iterator iter_end = m_listFruitPacket.end();

	// ��� ������ ���� ���Ͽ� �˻�
	for (iter; iter != iter_end; ++iter)
	{
		// �̹� �߷��� ���Ͽ� ���ؼ��� �˻����� �ʴ´�.
		if (iter->FruitPacket.bSliced)
			continue;

		// ���� �ʱ�ȭ
		m_vecCollisionPoint_Player1.clear();
		m_vecCollisionPoint_Player2.clear();

		// �÷��̾�1�� �׸� ���� ��� ���� ���Ͽ� �˻�
		for (int j = 0; j < m_GameInfo.LinePosInfo_Player1.iLinePosCount; ++j)
		{
			// ���ϰ� ���� �Ÿ��� ������ ���������� �۰ų� ���� ��� (�÷��̾�1)
			if (Distance(iter->FruitPacket.ptPos, m_GameInfo.LinePosInfo_Player1.LinePos[j]) <= m_iRadius)
			{
				// ���Ϳ� ���� (�÷��̾�1)
				m_vecCollisionPoint_Player1.push_back(m_GameInfo.LinePosInfo_Player1.LinePos[j]);
			}
		}
		// �÷��̾�2�� �׸� ���� ��� ���� ���Ͽ� �˻�
		for (int k = 0; k < m_GameInfo.LinePosInfo_Player2.iLinePosCount; ++k)
		{
			// ���ϰ� ���� �Ÿ��� ������ ���������� �۰ų� ���� ��� (�÷��̾�2)
			if (Distance(iter->FruitPacket.ptPos, m_GameInfo.LinePosInfo_Player2.LinePos[k]) <= m_iRadius)
			{
				// ���Ϳ� ���� (�÷��̾�2)
				m_vecCollisionPoint_Player2.push_back(m_GameInfo.LinePosInfo_Player2.LinePos[k]);
			}
		}

		// �Ÿ��� �� �ʱ�ȭ
		float fSumDist_Player1 = 0.f;
		float fSumDist_Player2 = 0.f;

		// ���ϰ� ���� �Ÿ��� ������ ���������� ���� ��� ���� ���� (�÷��̾�1)
		for (size_t i = 1; i < m_vecCollisionPoint_Player1.size(); ++i)
		{
			// ���� �Ÿ��� ���� ���� (�÷��̾�1)
			fSumDist_Player1 += Distance(m_vecCollisionPoint_Player1[i - 1], m_vecCollisionPoint_Player1[i]);
		}
		// ���ϰ� ���� �Ÿ��� ������ ���������� ���� ��� ���� ���� (�÷��̾�2)
		for (size_t i = 1; i < m_vecCollisionPoint_Player2.size(); ++i)
		{
			// ���� �Ÿ��� ���� ���� (�÷��̾�2)
			fSumDist_Player2 += Distance(m_vecCollisionPoint_Player2[i - 1], m_vecCollisionPoint_Player2[i]);
		}

		// ������ ���� ������ ���������� Ŭ ��� �߸� ������ ����
		if (fSumDist_Player1 >= m_iRadius / 2)
		{
			// ������ ���¸� �߸� ������ �ٲ۴�.
			iter->FruitPacket.bSliced = true;

			// ������ �߷ȴٸ� ������ ����(�̹���)�� �ٲ۴�.
			switch (iter->FruitPacket.iSort)
			{
			case Lemon:
				iter->FruitPacket.iSort = Lemon_Sliced;
				m_GameInfo.iScore_Player1 += 10;
				break;
			case Strawberry:
				iter->FruitPacket.iSort = Strawberry_Sliced;
				m_GameInfo.iScore_Player1 += 10;
				break;
			case Kiwi:
				iter->FruitPacket.iSort = Kiwi_Sliced;
				m_GameInfo.iScore_Player1 += 10;
				break;
			case Peach:
				iter->FruitPacket.iSort = Peach_Sliced;
				m_GameInfo.iScore_Player1 += 10;
				break;
			case Watermelon:
				iter->FruitPacket.iSort = Watermelon_Sliced;
				m_GameInfo.iScore_Player1 += 10;
				break;
			case Bomb:
				iter->FruitPacket.iSort = Bomb_Sliced;
				m_GameInfo.iScore_Player1 -= 10;
				break;
			}
		}

		if (fSumDist_Player2 >= m_iRadius / 2)
		{
			// ������ ���¸� �߸� ������ �ٲ۴�.
			iter->FruitPacket.bSliced  = true;

			// ������ �߷ȴٸ� ������ ����(�̹���)�� �ٲ۴�.
			switch (iter->FruitPacket.iSort)
			{
			case Lemon:
				iter->FruitPacket.iSort = Lemon_Sliced;
				m_GameInfo.iScore_Player2 += 10;
				break;
			case Strawberry:
				iter->FruitPacket.iSort = Strawberry_Sliced;
				m_GameInfo.iScore_Player2 += 10;
				break;
			case Kiwi:
				iter->FruitPacket.iSort = Kiwi_Sliced;
				m_GameInfo.iScore_Player2 += 10;
				break;
			case Peach:
				iter->FruitPacket.iSort = Peach_Sliced;
				m_GameInfo.iScore_Player2 += 10;
				break;
			case Watermelon:
				iter->FruitPacket.iSort = Watermelon_Sliced;
				m_GameInfo.iScore_Player2 += 10;
				break;
			case Bomb:
				iter->FruitPacket.iSort = Bomb_Sliced;
				m_GameInfo.iScore_Player2 -= 10;
				break;
			}
		}
	}
}

void CMainGame::CreateFruit(float fElapsedTimeInSecond)
{
	m_fTime_CreateFruit += fElapsedTimeInSecond;

	if (m_fTime_CreateFruit >= 1.f)
	{
		FruitInfo fruit;
		POINT temp;
		int num = rand() % (2 + m_iRoundCount) + 1;

		while (num != 0 && m_iPackCount != 0)
		{
			fruit.FruitPacket.iState = rise;
			fruit.FruitPacket.ptPos = { (float)(rand() % CLIENT_WIDTH), (float)(CLIENT_HEIGHT - 100.f) };
			temp.x = (rand() % (CLIENT_WIDTH / 3) + (CLIENT_WIDTH / 3));
			temp.y = (rand() % (CLIENT_HEIGHT / 10) + (CLIENT_HEIGHT / 2));

			fruit.ptAccel = { (float)(rand() % 20 + 25.0), (float)(rand() % 30 + 60.0) };
			fruit.ptSpeed = { (rand() % 50) + 70.f, (rand() % 100) + 150.f };
			fruit.ptDir = { (float)((int)(temp.x - fruit.FruitPacket.ptPos.x) % 100) * 0.01f,
				(float)((int)(temp.y - fruit.FruitPacket.ptPos.y) % 100) * 0.01f };

			fruit.FruitPacket.iSort = rand() % 6;
			fruit.FruitPacket.bSliced = false;
			fruit.fDeleteTime = 0.f;

			m_listFruitPacket.push_back(fruit);
			num--;
			m_iPackCount--;
		}

		m_fTime_CreateFruit = 0.f;
	}

	// ȭ�� ������ ������ ���� ����
	DeleteFruit();

	// ���� ������ �浹üũ
	CollisionCheck();

	// �߸� ������ �����ð��� ���� �� ����
	DeleteFruit_Sliced(fElapsedTimeInSecond);

	// ����Ʈ�� �ִ� ���� ������ Ŭ���̾�Ʈ�� ������ ���� ��Ŷ�� �Ű� ��´�.
	list<FruitInfo>::iterator iter = m_listFruitPacket.begin();
	list<FruitInfo>::iterator iter_end = m_listFruitPacket.end();

	m_GameInfo.iFruitInfoCount = 0;

	for (; iter != iter_end; ++iter)
	{
		m_GameInfo.FruitInfo[m_GameInfo.iFruitInfoCount].ptPos = iter->FruitPacket.ptPos;
		m_GameInfo.FruitInfo[m_GameInfo.iFruitInfoCount].iState = iter->FruitPacket.iState;
		m_GameInfo.FruitInfo[m_GameInfo.iFruitInfoCount].iRadian = iter->FruitPacket.iRadian;
		m_GameInfo.FruitInfo[m_GameInfo.iFruitInfoCount].iSort = iter->FruitPacket.iSort;
		m_GameInfo.FruitInfo[m_GameInfo.iFruitInfoCount].bSliced = iter->FruitPacket.bSliced;
		++m_GameInfo.iFruitInfoCount;
	}
}

void CMainGame::AddPos(float fElapsedTimeInSecond)
{
	list<FruitInfo>::iterator iter = m_listFruitPacket.begin();
	list<FruitInfo>::iterator iter_end = m_listFruitPacket.end();

	for (iter; iter != iter_end; ++iter)
	{
		if (iter->FruitPacket.bSliced)
			continue;

		switch (iter->FruitPacket.iState)
		{
		case rise:
			if (iter->ptSpeed.y <= 1)
				iter->FruitPacket.iState = down;
			break;
		case down:
			if (iter->FruitPacket.ptPos.y >= CLIENT_HEIGHT)
				iter->FruitPacket.iState = rise;
			break;
		default:
			break;
		}
		iter->ptSpeed.x = iter->ptSpeed.x + iter->ptAccel.x * fElapsedTimeInSecond;
		iter->ptSpeed.y = iter->ptSpeed.y - iter->ptAccel.y * fElapsedTimeInSecond;

		iter->FruitPacket.ptPos.x = iter->FruitPacket.ptPos.x + iter->ptDir.x*iter->ptSpeed.x * fElapsedTimeInSecond;
		iter->FruitPacket.ptPos.y = iter->FruitPacket.ptPos.y + iter->ptDir.y*iter->ptSpeed.y * fElapsedTimeInSecond;
	}
}

// ȭ�� ������ ���� ���� ����
void CMainGame::DeleteFruit()
{
	list<FruitInfo>::iterator iter = m_listFruitPacket.begin();
	list<FruitInfo>::iterator iter_end = m_listFruitPacket.end();

	for (iter; iter != iter_end;)
	{
		if ((*iter).FruitPacket.ptPos.x < -m_iRadius ||
			(*iter).FruitPacket.ptPos.x > CLIENT_WIDTH + m_iRadius ||
			(*iter).FruitPacket.ptPos.y < -m_iRadius ||
			(*iter).FruitPacket.ptPos.y > CLIENT_HEIGHT + m_iRadius)
		{
			iter = m_listFruitPacket.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

// �߷��� ���� 2�� �Ŀ� ����
void CMainGame::DeleteFruit_Sliced(float fElapsedTimeInSecond)
{
	list<FruitInfo>::iterator iter = m_listFruitPacket.begin();
	list<FruitInfo>::iterator iter_end = m_listFruitPacket.end();

	for (iter; iter != iter_end;)
	{
		if (iter->FruitPacket.bSliced)
			iter->fDeleteTime += fElapsedTimeInSecond;

		if (iter->fDeleteTime >= 2.f)
		{
			iter = m_listFruitPacket.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void CMainGame::InitFruit()
{
	m_listFruitPacket.clear();

	m_GameInfo.iFruitInfoCount = 0;
}

void CMainGame::InitScore()
{
	m_GameInfo.iScore_Player1 = 0;
	m_GameInfo.iScore_Player2 = 0;
}