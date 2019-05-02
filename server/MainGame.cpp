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
	if (m_bLogin[0] == true && m_bLogin[1] == true)				// 두 명이 로그인했다면
	{
		switch (m_GameInfo.enumGameState)						// 게임 상태에 따라 처리
		{
		case GameState_Wait:									// 처음 두 명이 로그인했다면 이 곳에 들어온다. -> break 하지 않는다. (바로 GameState_Term일 때를 수행한다.)
			m_GameInfo.enumGameState = GameState_Term;			// 게임 상태를 GameState_Term으로 바꾼다.
			m_GameInfo.fTime = (float)GameTime_Term;			// 게임 시간을 GameTime_Term으로 바꾼다.
		case GameState_Term:
			m_GameInfo.fTime -= fElapsedTimeInSecond;			// 시간 계산

			if (m_GameInfo.fTime <= 0.f)						// 시간이 0이 되면
			{
				m_GameInfo.enumGameState = GameState_Play;		// 게임 상태를 GameState_Play로 바꾼다.
				m_GameInfo.fTime = (float)GameTime_Play;		// 게임 시간을 GameTime_Play로 바꾼다.
			}
			break;
		case GameState_Play:									
			m_GameInfo.fTime -= fElapsedTimeInSecond;			// 시간 계산

			CreateFruit(fElapsedTimeInSecond);					// 과일 생성
			AddPos(fElapsedTimeInSecond);						// 과일 위치 변경

			if (m_GameInfo.fTime <= 0.f)						// 시간이 0이 되면
			{
				InitFruit();									// 과일 초기화 (리스트에 있는 모든 과일 삭제) -> 라운드가 종료되었기 때문에

				if (m_iRoundCount != 3)							// 라운드가 3이 아니라면
				{
					m_GameInfo.enumGameState = GameState_Term;	// 게임 상태를 GameState_Term으로 바꾼다.
					m_GameInfo.fTime = (float)GameTime_Term;	// 게임 시간을 GameTime_Term으로 바꾼다.

					m_iRoundCount++;							// 라운드 카운트를 증가시킨다.
					m_iPackCount = m_iRoundCount * 100;			// 생성 과일 개수를 증가시킨다.
				}
				else											// 라운드가 3이라면
				{
					m_GameInfo.enumGameState = GameState_End;	// 게임 상태를 GameState_End로 바꾼다.
					m_GameInfo.fTime = (float)GameTime_End;		// 게임 시간을 GameTime_End로 바꾼다.
				}
			}
			break;
		case GameState_End:		
			m_GameInfo.fTime -= fElapsedTimeInSecond;			// 시간 계산

			if (m_GameInfo.fTime <= 0.f)						// 시간이 0이 되면
			{
				m_GameInfo.enumGameState = GameState_Term;		// 게임 상태를 GameState_Term으로 바꾼다.
				m_GameInfo.fTime = (float)GameTime_Term;		// 게임 시간을 GameTime_Term으로 바꾼다.

				m_iRoundCount = 1;								// 라운드 카운트를 초기화한다.
				InitScore();									// 점수를 초기화한다.
			}
			break;
		default:
			break;
		}
	}
	else														// 두 명이 로그인하지 않았다면
	{
		m_GameInfo.enumGameState = GameState_Wait;				// 게임 상태를 GameState_Wait으로 바꾼다.
		m_GameInfo.fTime = (float)GameTime_Wait;				// 게임 시간을 GameTime_Wait으로 바꾼다.

		InitFruit();											// 과일을 초기화한다.
		InitScore();											// 점수를 초기화한다.
		m_iRoundCount = 1;										// 라운드를 카운트를 초기화한다.
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

	// 모든 과일의 점에 대하여 검사
	for (iter; iter != iter_end; ++iter)
	{
		// 이미 잘려진 과일에 대해서는 검사하지 않는다.
		if (iter->FruitPacket.bSliced)
			continue;

		// 벡터 초기화
		m_vecCollisionPoint_Player1.clear();
		m_vecCollisionPoint_Player2.clear();

		// 플레이어1이 그린 선의 모든 점에 대하여 검사
		for (int j = 0; j < m_GameInfo.LinePosInfo_Player1.iLinePosCount; ++j)
		{
			// 과일과 점의 거리가 과일의 반지름보다 작거나 같을 경우 (플레이어1)
			if (Distance(iter->FruitPacket.ptPos, m_GameInfo.LinePosInfo_Player1.LinePos[j]) <= m_iRadius)
			{
				// 벡터에 저장 (플레이어1)
				m_vecCollisionPoint_Player1.push_back(m_GameInfo.LinePosInfo_Player1.LinePos[j]);
			}
		}
		// 플레이어2가 그린 선의 모든 점에 대하여 검사
		for (int k = 0; k < m_GameInfo.LinePosInfo_Player2.iLinePosCount; ++k)
		{
			// 과일과 점의 거리가 과일의 반지름보다 작거나 같을 경우 (플레이어2)
			if (Distance(iter->FruitPacket.ptPos, m_GameInfo.LinePosInfo_Player2.LinePos[k]) <= m_iRadius)
			{
				// 벡터에 저장 (플레이어2)
				m_vecCollisionPoint_Player2.push_back(m_GameInfo.LinePosInfo_Player2.LinePos[k]);
			}
		}

		// 거리의 합 초기화
		float fSumDist_Player1 = 0.f;
		float fSumDist_Player2 = 0.f;

		// 과일과 점의 거리가 과일의 반지름보다 작은 모든 점에 대해 (플레이어1)
		for (size_t i = 1; i < m_vecCollisionPoint_Player1.size(); ++i)
		{
			// 점의 거리의 합을 구함 (플레이어1)
			fSumDist_Player1 += Distance(m_vecCollisionPoint_Player1[i - 1], m_vecCollisionPoint_Player1[i]);
		}
		// 과일과 점의 거리가 과일의 반지름보다 작은 모든 점에 대해 (플레이어2)
		for (size_t i = 1; i < m_vecCollisionPoint_Player2.size(); ++i)
		{
			// 점의 거리의 합을 구함 (플레이어2)
			fSumDist_Player2 += Distance(m_vecCollisionPoint_Player2[i - 1], m_vecCollisionPoint_Player2[i]);
		}

		// 점들의 합이 과일의 반지름보다 클 경우 잘린 것으로 판정
		if (fSumDist_Player1 >= m_iRadius / 2)
		{
			// 과일의 상태를 잘린 것으로 바꾼다.
			iter->FruitPacket.bSliced = true;

			// 과일이 잘렸다면 과일의 상태(이미지)를 바꾼다.
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
			// 과일의 상태를 잘린 것으로 바꾼다.
			iter->FruitPacket.bSliced  = true;

			// 과일이 잘렸다면 과일의 상태(이미지)를 바꾼다.
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

	// 화면 밖으로 나가면 과일 삭제
	DeleteFruit();

	// 선과 과일의 충돌체크
	CollisionCheck();

	// 잘린 과일은 일정시간이 지난 후 삭제
	DeleteFruit_Sliced(fElapsedTimeInSecond);

	// 리스트에 있는 과일 정보를 클라이언트에 보내기 위해 패킷에 옮겨 담는다.
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

// 화면 밖으로 나간 과일 삭제
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

// 잘려진 과일 2초 후에 삭제
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