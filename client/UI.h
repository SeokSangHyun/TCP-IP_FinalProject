#pragma once

class CUI
{
public:
	CUI(TCHAR* imgPath, POINT pos, POINT size);
	~CUI();

private:
	TCHAR*		m_szImgPath;
	HDC			m_DC;
	HDC			m_HDC;
	HDC			m_MemDC;
	HBITMAP		m_hOld;
	HBITMAP		m_Bitmap;

private:
	TCHAR		m_szStr[10];
	POINT		m_ptPos;
	POINT		m_ptSize;
	RECT		m_rectSize;
	HFONT		m_hFont;
	HFONT		m_hOldFont;

private:
	UIType		m_UIType;

public:
	void	Set_StrRectSize(RECT rSize) { m_rectSize = rSize; }
	void	Set_StringUI(char* str);
	void	SetUIType(UIType UIType) { m_UIType = UIType; }

public:
	void	LoadBmp(TCHAR* pPath);

public:
	void	Initialize();
	void	Update(GameInfo gameInfo);
	void	Render(HDC _dc);
	void	Release();
};

