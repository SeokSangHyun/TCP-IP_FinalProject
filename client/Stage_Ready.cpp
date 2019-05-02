#include "stdafx.h"
#include "Stage_Ready.h"

CStage_Ready::CStage_Ready()
{
}

CStage_Ready::~CStage_Ready()
{
	Release();
}

void CStage_Ready::Initialize()
{
	m_bSceneChange = false;

	m_szImgPath = L"Resource/Logo.bmp";

	LoadBmp(m_szImgPath);
}

void CStage_Ready::LoadBmp(TCHAR* pPath)
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

void CStage_Ready::Update(GameInfo gameInfo)
{
}

void CStage_Ready::Render(HDC _dc)
{
	BitBlt(_dc,
		0,	//x위치
		0,	//y위치
		WINCX,//가로길이
		WINCY, //세로길이
		m_MemDC, 0, 0, SRCCOPY);
}

void CStage_Ready::Release()
{

}
