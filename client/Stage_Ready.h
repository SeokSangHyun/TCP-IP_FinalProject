#pragma once
#include "Scene.h"

class CStage_Ready :
	public CScene
{
public:
	CStage_Ready();
	virtual ~CStage_Ready();

private:
	bool	m_bSceneChange;
	TCHAR*	m_szImgPath;

private:
	HDC			m_DC;
	HDC			m_HDC;
	HDC			m_MemDC;
	HBITMAP		m_hOld;
	HBITMAP		m_Bitmap;

public:
	void	Initialize();
	void	Update(GameInfo gameInfo);
	void	Render(HDC _dc);
	void	Release();

public:
	void	LoadBmp(TCHAR* pPath);
	bool	Get_m_bSceneChange() { return m_bSceneChange; }
};

