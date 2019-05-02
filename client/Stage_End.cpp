#include "stdafx.h"
#include "Stage_End.h"
#include "Player.h"
#include "Fruit.h"
#include "UI.h"

CStage_End::CStage_End()
{
}


CStage_End::~CStage_End()
{
	Release();
}


void CStage_End::LoadBmp(TCHAR* pPath1, TCHAR* pPath2)
{
	//
	m_HDC = GetDC(g_hWnd);
	m_WinDC = CreateCompatibleDC(m_HDC);
	m_hBitmap = (HBITMAP)LoadImage(NULL, pPath1, IMAGE_BITMAP,
		0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	if (!m_hBitmap)
		MessageBox(g_hWnd, pPath1, L"Image Load Failed", MB_OK);
	m_hOld = (HBITMAP)SelectObject(m_WinDC, m_hBitmap);
	ReleaseDC(g_hWnd, m_HDC);

	m_HDC = GetDC(g_hWnd);
	m_LoseDC = CreateCompatibleDC(m_HDC);
	m_hBitmap = (HBITMAP)LoadImage(NULL, pPath2, IMAGE_BITMAP,
		0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	if (!m_hBitmap)
		MessageBox(g_hWnd, pPath2, L"Image Load Failed", MB_OK);
	m_hOld = (HBITMAP)SelectObject(m_LoseDC, m_hBitmap);
	ReleaseDC(g_hWnd, m_HDC);
	//

}


void CStage_End::Initialize()
{
	m_szWinImgPath = L"Resource/Win.bmp";
	m_szLoseImgPath = L"Resource/Lose.bmp";

	LoadBmp(m_szWinImgPath, m_szLoseImgPath);
}


void CStage_End::Update(GameInfo gameInfo)
{
	if (g_iSerialNum == 1)
	{
		m_iMyScore = gameInfo.iScore_Player1;
		m_iEnemyScore = gameInfo.iScore_Player2;
	}
	else
	{
		m_iMyScore = gameInfo.iScore_Player2;
		m_iEnemyScore = gameInfo.iScore_Player1;
	}
}


void CStage_End::Render(HDC _dc)
{
	if (m_iMyScore >= m_iEnemyScore)
	{
		m_MemDC = m_WinDC;
	}
	else
	{
		m_MemDC = m_LoseDC;
	}

	BitBlt(_dc,
		0,
		0,
		WINCX,
		WINCY,
		m_MemDC,
		0, 0,
		SRCCOPY);
}

void CStage_End::Release()
{

}
