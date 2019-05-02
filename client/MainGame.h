#pragma once

class CScene;
class CPlayer;

class CMainGame
{
public:
	CMainGame();
	~CMainGame();

private:
	bool		m_bSceneChange;
	bool		m_bEndScene;
	int			m_iNowState;
	CScene*		m_pScene;
	TCHAR*		m_szImgPath;

private:
	HDC			m_DC;
	HDC			m_HDC;
	HDC			m_MemDC;
	HBITMAP		m_hOld;
	HBITMAP		m_Bitmap;

public:
	void		LoadBmp(TCHAR* pPath);
	CPlayer*	GetPlayer();

public:
	void		Initialize();
	void		Update(GameInfo gameInfo);
	void		Render();
	void		Release();
};