#pragma once

class CUI;

class CPlayer
{
public:
	CPlayer();
	~CPlayer();
	
private:
	CUI*			m_pScoreUI;
	CUI*			m_pScoreUI_Enemy;

private:
	TCHAR*			m_szImgPath;
	HDC				m_DC;
	HDC				m_HDC;
	HDC				m_MemDC;
	HBITMAP			m_hOld;
	HBITMAP			m_Bitmap;

private:
	int				m_iScore;
	int				m_iLinePosCount;
	list<PointInfo>	m_listLinePos;
	GameInfo		m_GameInfo;
	LinePosInfo		m_LinePosInfo;

public:
	void		LoadBmp(TCHAR* pPath);

public:
	void		Initialize();
	void		Update(GameInfo gameInfo);
	void		Render(HDC _dc);
	void		Release();
	void		DrawSlice();
	void		Add_DeleteTime();
	void		Delete_Point();

public:
	int			GetScore() { return m_iScore; }
	LinePosInfo	GetLinePosInfo() { return m_LinePosInfo; }

public:
	void		SetScore(int iScore) { m_iScore = iScore; }
};