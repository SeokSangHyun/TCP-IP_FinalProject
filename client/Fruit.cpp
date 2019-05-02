#include "stdafx.h"
#include "Fruit.h"

CFruit::CFruit()
{

}

CFruit::~CFruit()
{

}

void CFruit::Initialize()
{
	LoadBmp();
	m_iFruitInfoCount = 0;
}

void CFruit::Update(GameInfo gameInfo)
{
	m_GameInfo = gameInfo;

	m_iFruitInfoCount = gameInfo.iFruitInfoCount;

	for(int i = 0; i < m_iFruitInfoCount; ++i)
		m_FruitInfo[i] = gameInfo.FruitInfo[i];
}

void CFruit::Render(HDC _dc)
{
	Render_Fruit_Sliced(_dc);
	Render_Fruit(_dc);
}

void CFruit::Render_Fruit_Sliced(HDC _dc)
{
	for (int i = 0; i < m_iFruitInfoCount; ++i)
	{
		if (m_FruitInfo[i].bSliced == false)
			continue;

		switch (m_FruitInfo[i].iSort)
		{
		case Lemon_Sliced:
			m_MemDC = m_MemDC_Lemon_Sliced;
			m_iSize.x = 60;
			m_iSize.y = 60;
			break;
		case Strawberry_Sliced:
			m_MemDC = m_MemDC_Strawberry_Sliced;
			m_iSize.x = 60;
			m_iSize.y = 60;
			break;
		case Kiwi_Sliced:
			m_MemDC = m_MemDC_Kiwi_Sliced;
			m_iSize.x = 60;
			m_iSize.y = 60;
			break;
		case Peach_Sliced:
			m_MemDC = m_MemDC_Peach_Sliced;
			m_iSize.x = 60;
			m_iSize.y = 60;
			break;
		case Watermelon_Sliced:
			m_MemDC = m_MemDC_Watermelon_Sliced;
			m_iSize.x = 60;
			m_iSize.y = 60;
			break;
		case Bomb_Sliced:
			m_MemDC = m_MemDC_Bomb_Sliced;
			m_iSize.x = 120;
			m_iSize.y = 60;
			break;
		default:
			break;
		}

		TransparentBlt(_dc,
			(int)m_FruitInfo[i].ptFruitPos.x - 30.f,
			(int)m_FruitInfo[i].ptFruitPos.y - 30.f,
			m_iSize.x,
			m_iSize.y,
			m_MemDC,
			0,
			0,
			m_iSize.x,
			m_iSize.y,
			RGB(0, 0, 0));
	}
}

void CFruit::Render_Fruit(HDC _dc)
{
	for (int i = 0; i < m_iFruitInfoCount; ++i)
	{
		if (m_FruitInfo[i].bSliced)
			continue;

		switch (m_FruitInfo[i].iSort)
		{
		case Lemon:
			m_MemDC = m_MemDC_Lemon;
			m_iSize.x = 50;
			m_iSize.y = 50;
			break;
		case Strawberry:
			m_MemDC = m_MemDC_Strawberry;
			m_iSize.x = 50;
			m_iSize.y = 50;
			break;
		case Kiwi:
			m_MemDC = m_MemDC_Kiwi;
			m_iSize.x = 50;
			m_iSize.y = 50;
			break;
		case Peach:
			m_MemDC = m_MemDC_Peach;
			m_iSize.x = 50;
			m_iSize.y = 50;
			break;
		case Watermelon:
			m_MemDC = m_MemDC_Watermelon;
			m_iSize.x = 50;
			m_iSize.y = 50;
			break;
		case Bomb:
			m_MemDC = m_MemDC_Bomb;
			m_iSize.x = 70;
			m_iSize.y = 70;
			break;
		default:
			break;
		}

		TransparentBlt(_dc,
			(int)m_FruitInfo[i].ptFruitPos.x - 25.f,
			(int)m_FruitInfo[i].ptFruitPos.y - 25.f,
			m_iSize.x,
			m_iSize.y,
			m_MemDC,
			0,
			0,
			m_iSize.x,
			m_iSize.y,
			RGB(0, 0, 0));
	}
}

void CFruit::Release()
{

}

void CFruit::LoadBmp()
{
	// 레몬
	m_szImgPath = L"Resource/Lemon.bmp";

	m_HDC = GetDC(g_hWnd);
	m_MemDC_Lemon = CreateCompatibleDC(m_HDC);
	m_Bitmap = (HBITMAP)LoadImage(NULL, m_szImgPath, IMAGE_BITMAP, 0, 0,
		LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	if (!m_Bitmap)
		MessageBox(g_hWnd, m_szImgPath, L"Image Load Failed", MB_OK);

	m_hOld = (HBITMAP)SelectObject(m_MemDC_Lemon, m_Bitmap);
	ReleaseDC(g_hWnd, m_HDC);

	// 잘려진 레몬
	m_szImgPath = L"Resource/Lemon_Sliced.bmp";

	m_HDC = GetDC(g_hWnd);
	m_MemDC_Lemon_Sliced = CreateCompatibleDC(m_HDC);
	m_Bitmap = (HBITMAP)LoadImage(NULL, m_szImgPath, IMAGE_BITMAP, 0, 0,
		LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	if (!m_Bitmap)
		MessageBox(g_hWnd, m_szImgPath, L"Image Load Failed", MB_OK);

	m_hOld = (HBITMAP)SelectObject(m_MemDC_Lemon_Sliced, m_Bitmap);
	ReleaseDC(g_hWnd, m_HDC);

	// 키위
	m_szImgPath = L"Resource/kiwi.bmp";

	m_HDC = GetDC(g_hWnd);
	m_MemDC_Kiwi = CreateCompatibleDC(m_HDC);
	m_Bitmap = (HBITMAP)LoadImage(NULL, m_szImgPath, IMAGE_BITMAP, 0, 0,
		LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	if (!m_Bitmap)
		MessageBox(g_hWnd, m_szImgPath, L"Image Load Failed", MB_OK);

	m_hOld = (HBITMAP)SelectObject(m_MemDC_Kiwi, m_Bitmap);
	ReleaseDC(g_hWnd, m_HDC);
	
	// 잘려진 키위
	m_szImgPath = L"Resource/kiwi_Sliced.bmp";

	m_HDC = GetDC(g_hWnd);
	m_MemDC_Kiwi_Sliced = CreateCompatibleDC(m_HDC);
	m_Bitmap = (HBITMAP)LoadImage(NULL, m_szImgPath, IMAGE_BITMAP, 0, 0,
		LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	if (!m_Bitmap)
		MessageBox(g_hWnd, m_szImgPath, L"Image Load Failed", MB_OK);

	m_hOld = (HBITMAP)SelectObject(m_MemDC_Kiwi_Sliced, m_Bitmap);
	ReleaseDC(g_hWnd, m_HDC);

	// 딸기
	m_szImgPath = L"Resource/strawberry.bmp";

	m_HDC = GetDC(g_hWnd);
	m_MemDC_Strawberry = CreateCompatibleDC(m_HDC);
	m_Bitmap = (HBITMAP)LoadImage(NULL, m_szImgPath, IMAGE_BITMAP, 0, 0,
		LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	if (!m_Bitmap)
		MessageBox(g_hWnd, m_szImgPath, L"Image Load Failed", MB_OK);

	m_hOld = (HBITMAP)SelectObject(m_MemDC_Strawberry, m_Bitmap);
	ReleaseDC(g_hWnd, m_HDC);

	// 잘려진 딸기
	m_szImgPath = L"Resource/strawberry_Sliced.bmp";

	m_HDC = GetDC(g_hWnd);
	m_MemDC_Strawberry_Sliced = CreateCompatibleDC(m_HDC);
	m_Bitmap = (HBITMAP)LoadImage(NULL, m_szImgPath, IMAGE_BITMAP, 0, 0,
		LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	if (!m_Bitmap)
		MessageBox(g_hWnd, m_szImgPath, L"Image Load Failed", MB_OK);

	m_hOld = (HBITMAP)SelectObject(m_MemDC_Strawberry_Sliced, m_Bitmap);
	ReleaseDC(g_hWnd, m_HDC);
	 
	// 복숭아
	m_szImgPath = L"Resource/peach.bmp";

	m_HDC = GetDC(g_hWnd);
	m_MemDC_Peach = CreateCompatibleDC(m_HDC);
	m_Bitmap = (HBITMAP)LoadImage(NULL, m_szImgPath, IMAGE_BITMAP, 0, 0,
		LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	if (!m_Bitmap)
		MessageBox(g_hWnd, m_szImgPath, L"Image Load Failed", MB_OK);

	m_hOld = (HBITMAP)SelectObject(m_MemDC_Peach, m_Bitmap);
	ReleaseDC(g_hWnd, m_HDC);
	
	// 잘려진 복숭아
	m_szImgPath = L"Resource/peach_Sliced.bmp";

	m_HDC = GetDC(g_hWnd);
	m_MemDC_Peach_Sliced = CreateCompatibleDC(m_HDC);
	m_Bitmap = (HBITMAP)LoadImage(NULL, m_szImgPath, IMAGE_BITMAP, 0, 0,
		LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	if (!m_Bitmap)
		MessageBox(g_hWnd, m_szImgPath, L"Image Load Failed", MB_OK);

	m_hOld = (HBITMAP)SelectObject(m_MemDC_Peach_Sliced, m_Bitmap);
	ReleaseDC(g_hWnd, m_HDC);

	// 수박
	m_szImgPath = L"Resource/watermelon.bmp";

	m_HDC = GetDC(g_hWnd);
	m_MemDC_Watermelon = CreateCompatibleDC(m_HDC);
	m_Bitmap = (HBITMAP)LoadImage(NULL, m_szImgPath, IMAGE_BITMAP, 0, 0,
		LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	if (!m_Bitmap)
		MessageBox(g_hWnd, m_szImgPath, L"Image Load Failed", MB_OK);

	m_hOld = (HBITMAP)SelectObject(m_MemDC_Watermelon, m_Bitmap);
	ReleaseDC(g_hWnd, m_HDC);
	
	// 잘려진 수박
	m_szImgPath = L"Resource/watermelon_Sliced.bmp";

	m_HDC = GetDC(g_hWnd);
	m_MemDC_Watermelon_Sliced = CreateCompatibleDC(m_HDC);
	m_Bitmap = (HBITMAP)LoadImage(NULL, m_szImgPath, IMAGE_BITMAP, 0, 0,
		LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	if (!m_Bitmap)
		MessageBox(g_hWnd, m_szImgPath, L"Image Load Failed", MB_OK);

	m_hOld = (HBITMAP)SelectObject(m_MemDC_Watermelon_Sliced, m_Bitmap);
	ReleaseDC(g_hWnd, m_HDC);

	// 폭탄
	m_szImgPath = L"Resource/Bomb.bmp";

	m_HDC = GetDC(g_hWnd);
	m_MemDC_Bomb = CreateCompatibleDC(m_HDC);
	m_Bitmap = (HBITMAP)LoadImage(NULL, m_szImgPath, IMAGE_BITMAP, 0, 0,
		LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	if (!m_Bitmap)
		MessageBox(g_hWnd, m_szImgPath, L"Image Load Failed", MB_OK);

	m_hOld = (HBITMAP)SelectObject(m_MemDC_Bomb, m_Bitmap);
	ReleaseDC(g_hWnd, m_HDC);

	/////
	m_szImgPath = L"Resource/Bomb_Sliced.bmp";

	m_HDC = GetDC(g_hWnd);
	m_MemDC_Bomb_Sliced = CreateCompatibleDC(m_HDC);
	m_Bitmap = (HBITMAP)LoadImage(NULL, m_szImgPath, IMAGE_BITMAP, 0, 0,
		LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	if (!m_Bitmap)
		MessageBox(g_hWnd, m_szImgPath, L"Image Load Failed", MB_OK);

	m_hOld = (HBITMAP)SelectObject(m_MemDC_Bomb_Sliced, m_Bitmap);
	ReleaseDC(g_hWnd, m_HDC);
}