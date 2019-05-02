#include "stdafx.h"
#include "MainGame.h"
#include "Stage_Ready.h"
#include "Stage_Play.h"
#include "Stage_End.h"

CMainGame::CMainGame()
	:m_szImgPath(NULL)
{
	m_DC = GetDC(g_hWnd);

	m_bEndScene = false;

	m_pScene = new CStage_Ready();
	m_pScene->Initialize();
	m_iNowState = GameState_Wait;
}

CMainGame::~CMainGame()
{
}

void CMainGame::LoadBmp(TCHAR* pPath)
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
void CMainGame::Initialize()
{
	m_bSceneChange = false;

	m_szImgPath = L"Resource/BackBuffer.bmp";

	LoadBmp(m_szImgPath);
}

void CMainGame::Update(GameInfo gameInfo)
{
	if (gameInfo.enumGameState == GameState_Wait)
	{
		if (m_iNowState != GameState_Wait)
		{
			if (m_pScene != NULL)
			{
				delete m_pScene;
				m_pScene = NULL;
			}

			m_pScene = new CStage_Ready;
			m_pScene->Initialize();
			m_iNowState = GameState_Wait;
		}
	}
	else if (gameInfo.enumGameState == GameState_Term)
	{
		if (m_iNowState != GameState_Term)
		{
			if (m_pScene != NULL)
			{
				delete m_pScene;
				m_pScene = NULL;
			}

			m_pScene = new CStage_Play;
			m_pScene->Initialize();
			m_iNowState = GameState_Term;
		}
	}
	else if (gameInfo.enumGameState == GameState_End)
	{
		if (m_iNowState != GameState_End)
		{
			if (m_pScene != NULL)
			{
				delete m_pScene;
				m_pScene = NULL;
			}

			m_pScene = new CStage_End;
			m_pScene->Initialize();
			m_iNowState = GameState_End;
		}
	}

	m_pScene->Update(gameInfo);
}

void CMainGame::Render()
{	
	BitBlt(m_DC,
		0,	//x위치
		0,	//y위치
		WINCX,//가로길이
		WINCY, //세로길이
		m_MemDC, 0, 0, SRCCOPY);

	m_pScene->Render(m_MemDC);
}

void CMainGame::Release()
{

}

CPlayer* CMainGame::GetPlayer()
{
	if (m_iNowState == GameState_Term)
		return dynamic_cast<CStage_Play*>(m_pScene)->GetPlayer();
	else
		return NULL;
}