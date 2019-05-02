#pragma once
#include "Scene.h"


class CPlayer;
class CFruit;
class CUI;

class CStage_End : public CScene
{
public:
	CStage_End();
	virtual ~CStage_End();

private:
	CUI*		m_pTimeUI;

private:
	int			m_iMyScore;
	int			m_iEnemyScore;
	TCHAR*		m_szWinImgPath;
	TCHAR*		m_szLoseImgPath;

	HDC			m_DC;
	HDC			m_HDC;

	HDC			m_MemDC;
	HDC			m_WinDC;
	HDC			m_LoseDC;

	HBITMAP		m_hOld;
	HBITMAP		m_hBitmap;

public:
	void		LoadBmp(TCHAR* pPath1, TCHAR* pPath2);

public:
	void Initialize();
	void Update(GameInfo gameInfo);
	void Render(HDC _dc);
	void Release();
};

