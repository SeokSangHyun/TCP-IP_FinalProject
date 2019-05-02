#include "stdafx.h"
#include "Stage_Play.h"
#include "Player.h"
#include "Fruit.h"
#include "UI.h"

CStage_Play::CStage_Play()
{
}


CStage_Play::~CStage_Play()
{
	Release();
}
void CStage_Play::LoadBmp(TCHAR* pPath)
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
void CStage_Play::Initialize()
{
	m_szImgPath = L"Resource/back.bmp";
	LoadBmp(m_szImgPath);

	m_pPlayer = new CPlayer();
	m_pPlayer->Initialize();

	m_pFruit = new CFruit();
	m_pFruit->Initialize();

	POINT pos;
	pos.x = 400 - 70; pos.y = 5;
	POINT size;
	size.x = 29; size.y = 32;
	m_pTimeUI = new CUI(L"Resource/clock.bmp", pos, size);
	m_pTimeUI->Initialize();
	RECT rc = { pos.x + size.x, pos.y, pos.x + 2 * size.x ,pos.y + size.y };
	m_pTimeUI->Set_StrRectSize(rc);

	m_pTimeUI->SetUIType(UI_Time);
}

void CStage_Play::Update(GameInfo gameInfo)
{
	m_pFruit->Update(gameInfo);
	m_pPlayer->Update(gameInfo);
	m_pTimeUI->Update(gameInfo);
}

void CStage_Play::Render(HDC _dc)
{

	BitBlt(_dc,
		0,
		0,
		WINCX,
		WINCY,
		m_MemDC,
		0, 0,
		SRCCOPY);

	m_pFruit->Render(_dc);
	m_pPlayer->Render(_dc);
	m_pTimeUI->Render(_dc);
}

void CStage_Play::Release()
{

}

