#include "stdafx.h"
#include "UI.h"


CUI::CUI(TCHAR* imgPath, POINT pos, POINT size)
	:m_szImgPath(imgPath)
	, m_ptPos(pos)
	, m_ptSize(size)
{

}

CUI::~CUI()
{
	Release();
}

void CUI::LoadBmp(TCHAR* pPath)
{
	m_HDC = GetDC(g_hWnd);
	m_MemDC = CreateCompatibleDC(m_HDC);
	m_Bitmap = (HBITMAP)LoadImage(NULL, pPath, IMAGE_BITMAP, 0, 0,
		LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	if (!m_Bitmap)
		MessageBox(g_hWnd, pPath, L"Image Load Failed", MB_OK);

	m_hOld = (HBITMAP)SelectObject(m_MemDC, m_Bitmap);
	ReleaseDC(g_hWnd, m_HDC);
}

void CUI::Initialize()
{
	LoadBmp(m_szImgPath);
	
}

void CUI::Update(GameInfo gameInfo)
{
	if (m_UIType == UI_Time)
	{
		_stprintf_s(m_szStr, _countof(m_szStr), _T("%d"), int(gameInfo.fTime));
	}
	else if (m_UIType == UI_Score_Player1)
	{
		_stprintf_s(m_szStr, _countof(m_szStr), _T("%d"), gameInfo.iScore_Player1);
	}
	else if (m_UIType == UI_Score_Player2)
	{
		_stprintf_s(m_szStr, _countof(m_szStr), _T("%d"), gameInfo.iScore_Player2);
	}
}

void CUI::Render(HDC _dc)
{
	TransparentBlt(_dc,
		m_ptPos.x,
		m_ptPos.y,
		m_ptSize.x,
		m_ptSize.y,
		m_MemDC,
		0, 0,
		m_ptSize.x,
		m_ptSize.y,
		RGB(0, 0, 255));

	m_hFont = CreateFont(
		20,
		0,
		0,
		0,
		FW_SEMIBOLD, //µÎ²²
		FALSE,
		FALSE,
		0,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		L"±Ã¼­");
	m_hOldFont = (HFONT)SelectObject(_dc, m_hFont);

	SetBkMode(_dc, TRANSPARENT);

	if (g_iSerialNum == 1)
	{
		if (m_UIType == UI_Time)
			SetTextColor(_dc, RGB(255, 0, 255));
		else if (m_UIType == UI_Score_Player1)
			SetTextColor(_dc, RGB(0, 0, 255));
		else if (m_UIType == UI_Score_Player2)
			SetTextColor(_dc, RGB(255, 0, 0));
	}
	else
	{
		if (m_UIType == UI_Time)
			SetTextColor(_dc, RGB(255, 0, 255));
		else if (m_UIType == UI_Score_Player1)
			SetTextColor(_dc, RGB(255, 0, 0));
		else if (m_UIType == UI_Score_Player2)
			SetTextColor(_dc, RGB(0, 0, 255));
	}

	TextOut(_dc, m_rectSize.left, (m_rectSize.top + m_rectSize.bottom) / 4, m_szStr, _tcslen(m_szStr));

	SelectObject(_dc, m_hOldFont);
	DeleteObject(m_hFont);
}

void CUI::Release()
{

}

void CUI::Set_StringUI(char* str)
{
	//	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str, strlen(str), m_szStr, 4);
	//*m_szStr = (TCHAR)(*str);
}