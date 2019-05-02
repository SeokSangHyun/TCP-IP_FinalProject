#pragma once
#include "Scene.h"

class CPlayer;
class CFruit;
class CUI;

class CStage_Play :
	public CScene
{
public:
	CStage_Play();
	virtual ~CStage_Play();

private:
	CUI*		m_pTimeUI;

private:
	TCHAR*		m_szImgPath;
	HDC			m_DC;
	HDC			m_HDC;
	HDC			m_MemDC;
	HBITMAP		m_hOld;
	HBITMAP		m_Bitmap;
	CPlayer*	m_pPlayer;
	CFruit*		m_pFruit;
	bool		m_bSceneChange;

public:
	void		LoadBmp(TCHAR* pPath);
	CPlayer*	GetPlayer() { return m_pPlayer; }

public:
	void Initialize();
	void Update(GameInfo gameInfo);
	void Render(HDC _dc);
	void Release();

public:
	bool	Get_m_bSceneChange() { return m_bSceneChange; }

};

