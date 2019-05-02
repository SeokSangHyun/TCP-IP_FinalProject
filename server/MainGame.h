#pragma once

class CMainGame
{
public:
	CMainGame();
	~CMainGame();

private:
	bool				m_bLogin[2];
	int					m_iRoundCount;
	int					m_iPlayerNum;
	int					m_iRadius;
	float				m_fTime_CreateFruit;
	list<FruitInfo>		m_listFruitPacket;
	GameInfo			m_GameInfo;
	UnionDataForFloat	m_fGameTime;
	size_t				m_iPackCount;

private:
	vector<Point_F>		m_vecCollisionPoint_Player1;
	vector<Point_F>		m_vecCollisionPoint_Player2;

public:
	void		Initialize(void);
	void		Update(float fElapsedTimeInSecond);
	float		Distance(Point_F A, Point_F B);
	void		CollisionCheck();
	void		CreateFruit(float fElapsedTimeInSecond);
	void		DeleteFruit();
	void		DeleteFruit_Sliced(float fElapsedTimeInSecond);
	void		InitFruit();
	void		InitScore();
	void		AddPos(float fElapsedTimeInSecond);

public:
	bool		GetLogin0() { return m_bLogin[0]; }
	bool		GetLogin1() { return m_bLogin[1]; }
	GameInfo	GetGameInfo() { return m_GameInfo; }
	char*		GetTimeData() { return m_fGameTime.szBuf; }
	GameState	GetGameState() { return m_GameInfo.enumGameState; }
	int			GetFruitCount() { return m_GameInfo.iFruitInfoCount; }

public:
	void		SetLogin0(bool bLogin0) { m_bLogin[0] = bLogin0; }
	void		SetLogin1(bool bLogin1) { m_bLogin[1] = bLogin1; }
	void		SetTimeData(float fTime) { m_fGameTime.fData = fTime; }
	void		SetGameState(GameState enumGameState) { m_GameInfo.enumGameState = enumGameState; }
	void		SetLinePosInfo_Player1(LinePosInfo linePosInfo_Player1) { m_GameInfo.LinePosInfo_Player1 = linePosInfo_Player1; }
	void		SetLinePosInfo_Player2(LinePosInfo linePosInfo_Player2) { m_GameInfo.LinePosInfo_Player2 = linePosInfo_Player2; }
};

