#include "stdafx.h"
#include "Player.h"
#include "UI.h"

CPlayer::CPlayer()
{

}

CPlayer::~CPlayer()
{

}

void CPlayer::LoadBmp(TCHAR* pPath)
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

void CPlayer::Initialize()
{
	m_szImgPath = L"Resource/Alphaca.bmp";
	LoadBmp(m_szImgPath);

	m_iLinePosCount = 0;

	// 내 점수판
	POINT pos;
	POINT size;

	pos.x = 800 - 242; pos.y = 0;
	size.x = 242; size.y = 70;
	m_pScoreUI = new CUI(L"Resource/점수판.bmp", pos, size);
	m_pScoreUI->Initialize();
	
	RECT rc = { pos.x + size.x / 2, pos.y + size.y / 2, pos.x + 2 * size.x, pos.y + size.y / 2};
	m_pScoreUI->Set_StrRectSize(rc);

	// 적 점수판
	pos.x = 0; pos.y = 0;
	size.x = 242; size.y = 70;
	m_pScoreUI_Enemy = new CUI(L"Resource/점수판.bmp", pos, size);
	m_pScoreUI_Enemy->Initialize();

	rc = { pos.x + size.x / 2, pos.y + size.y / 2, pos.x + 2 * size.x, pos.y + size.y / 2};
	m_pScoreUI_Enemy->Set_StrRectSize(rc);

	// 점수판 구분
	if (g_iSerialNum == 1)
	{
		m_pScoreUI->SetUIType(UI_Score_Player1);
		m_pScoreUI_Enemy->SetUIType(UI_Score_Player2);
	}
	else
	{
		m_pScoreUI->SetUIType(UI_Score_Player2);
		m_pScoreUI_Enemy->SetUIType(UI_Score_Player1);
	}
}

void CPlayer::Update(GameInfo gameInfo)
{
	m_GameInfo = gameInfo;

	if (m_GameInfo.enumGameState == GameState_Play)
	{
		DrawSlice();
	}
	else
	{
		m_listLinePos.clear();
		m_LinePosInfo.iLinePosCount = 0;
	}

	m_pScoreUI->Update(gameInfo);
	m_pScoreUI_Enemy->Update(gameInfo);
}

void CPlayer::Render(HDC _dc)
{
	HPEN Pen, oPen;

	if (g_iSerialNum == 1)
	{
		//
		Pen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
		oPen = (HPEN)SelectObject(_dc, Pen);

		for (int i = 0; i < m_GameInfo.LinePosInfo_Player1.iLinePosCount; ++i)
		{
			if (i == 0)
				MoveToEx(_dc, int(m_GameInfo.LinePosInfo_Player1.LinePos[i].x), int(m_GameInfo.LinePosInfo_Player1.LinePos[i].y), NULL);

			LineTo(_dc, int(m_GameInfo.LinePosInfo_Player1.LinePos[i].x), int(m_GameInfo.LinePosInfo_Player1.LinePos[i].y));
		}

		SelectObject(_dc, oPen);
		DeleteObject(Pen);

		//
		Pen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
		oPen = (HPEN)SelectObject(_dc, Pen);

		for (int i = 0; i < m_GameInfo.LinePosInfo_Player2.iLinePosCount; ++i)
		{
			if (i == 0)
				MoveToEx(_dc, int(m_GameInfo.LinePosInfo_Player2.LinePos[i].x), int(m_GameInfo.LinePosInfo_Player2.LinePos[i].y), NULL);

			LineTo(_dc, int(m_GameInfo.LinePosInfo_Player2.LinePos[i].x), int(m_GameInfo.LinePosInfo_Player2.LinePos[i].y));
		}

		SelectObject(_dc, oPen);
		DeleteObject(Pen);
	}
	else
	{
		//
		Pen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
		oPen = (HPEN)SelectObject(_dc, Pen);

		for (int i = 0; i < m_GameInfo.LinePosInfo_Player1.iLinePosCount; ++i)
		{
			if (i == 0)
				MoveToEx(_dc, int(m_GameInfo.LinePosInfo_Player1.LinePos[i].x), int(m_GameInfo.LinePosInfo_Player1.LinePos[i].y), NULL);

			LineTo(_dc, int(m_GameInfo.LinePosInfo_Player1.LinePos[i].x), int(m_GameInfo.LinePosInfo_Player1.LinePos[i].y));
		}

		SelectObject(_dc, oPen);
		DeleteObject(Pen);

		//
		Pen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
		oPen = (HPEN)SelectObject(_dc, Pen);

		for (int i = 0; i < m_GameInfo.LinePosInfo_Player2.iLinePosCount; ++i)
		{
			if (i == 0)
				MoveToEx(_dc, int(m_GameInfo.LinePosInfo_Player2.LinePos[i].x), int(m_GameInfo.LinePosInfo_Player2.LinePos[i].y), NULL);

			LineTo(_dc, int(m_GameInfo.LinePosInfo_Player2.LinePos[i].x), int(m_GameInfo.LinePosInfo_Player2.LinePos[i].y));
		}

		SelectObject(_dc, oPen);
		DeleteObject(Pen);
	}

	m_pScoreUI->Render(_dc);
	m_pScoreUI_Enemy->Render(_dc);
}

void CPlayer::Release()
{

}

void CPlayer::DrawSlice()
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	PointInfo lineInfo;
	lineInfo.pos.x = (float)pt.x;
	lineInfo.pos.y = (float)pt.y;

	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		if (pt.x > 0 && pt.x < WINCX && pt.y >0 && pt.y < WINCY)
		{
			lineInfo.fDeleteTime = 0.f;
			m_listLinePos.push_back(lineInfo);
		}
	}
	else
	{
		m_listLinePos.clear();
	}

	Add_DeleteTime();
	Delete_Point();

	list<PointInfo>::iterator iter = m_listLinePos.begin();
	list<PointInfo>::iterator iter_End = m_listLinePos.end();

	iter = m_listLinePos.begin();
	iter_End = m_listLinePos.end();
	m_iLinePosCount = 0;

	for (iter; iter != iter_End; ++iter)
	{
		m_LinePosInfo.LinePos[m_iLinePosCount] = (*iter).pos;
		++m_iLinePosCount;
	}

	m_LinePosInfo.iLinePosCount = m_iLinePosCount;
}

void CPlayer::Add_DeleteTime()
{
	list<PointInfo>::iterator iter = m_listLinePos.begin();
	list<PointInfo>::iterator iter_End = m_listLinePos.end();

	for (iter; iter != iter_End; ++iter)
	{
		(*iter).fDeleteTime += g_fDeleteTime;
	}
}

void CPlayer::Delete_Point()
{
	list<PointInfo>::iterator iter = m_listLinePos.begin();
	list<PointInfo>::iterator iter_End = m_listLinePos.end();

	iter = m_listLinePos.begin();
	iter_End = m_listLinePos.end();

	for (iter; iter != iter_End;)
	{
		if ((*iter).fDeleteTime > 0.07f)
		{
			iter = m_listLinePos.erase(iter);
		}
		else
			++iter;
	}
}