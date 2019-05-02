#pragma once

class CFruit
{
public:
	CFruit();
	~CFruit();

private:
	POINT			m_iSize;
	TCHAR*			m_szImgPath;
	HDC				m_DC;
	HDC				m_HDC;
	HDC				m_MemDC;
	HDC				m_MemDC_Lemon;
	HDC				m_MemDC_Strawberry;
	HDC				m_MemDC_Kiwi;
	HDC				m_MemDC_Peach;
	HDC				m_MemDC_Watermelon;
	HDC				m_MemDC_Lemon_Sliced;
	HDC				m_MemDC_Strawberry_Sliced;
	HDC				m_MemDC_Kiwi_Sliced;
	HDC				m_MemDC_Peach_Sliced;
	HDC				m_MemDC_Watermelon_Sliced;
	HDC				m_MemDC_Bomb;
	HDC				m_MemDC_Bomb_Sliced;

	HBITMAP			m_hOld;
	HBITMAP			m_Bitmap;
	int				m_iFruitInfoCount;

private:
	FruitPacket		m_FruitInfo[100];
	GameInfo		m_GameInfo;

public:
	void			Initialize();
	void			Update(GameInfo gameInfo);
	void			Render(HDC _dc);
	void			Release();

public:
	void			LoadBmp();
	void			Render_Fruit_Sliced(HDC _dc);
	void			Render_Fruit(HDC _dc);
};