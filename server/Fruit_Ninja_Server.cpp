// Fruit_Ninja_Server.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Fruit_Ninja_Server.h"
#include "MainGame.h"

DWORD WINAPI ServerMainThread(LPVOID arg);
DWORD WINAPI CommunicateThread(LPVOID arg);

// 임계 영역
CRITICAL_SECTION cs;

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
HWND g_hWnd;
HWND hEdit;

// 메인게임 클래스 생성
CMainGame* g_pMainGame = new CMainGame();

// 시간 관련 전역 변수
DWORD g_dwPrevTime = 0;
float g_fTime_Loop = 0.f;
float g_fTime_FPS = 0.f;
float g_fTime_Communicate = 0.f;

// 전역 변수
LinePosInfo g_linePosInfo_Player1;
LinePosInfo g_linePosInfo_Player2;

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
#ifdef _DEBUG
	//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")	//멀티바이트
	#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")		//유니코드
#endif

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.
	MSG msg;
	HACCEL hAccelTable;

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_FRUIT_NINJA_SERVER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FRUIT_NINJA_SERVER));

	msg.message = WM_NULL;

	// 임계영역 초기화
	InitializeCriticalSection(&cs);

	// 소켓 통신 스레드 생성
	CreateThread(NULL, 0, ServerMainThread, NULL, 0, NULL);
	
	// 메인게임 클래스 초기화
	g_pMainGame->Initialize();

	TCHAR	szFPS[128] = L"";
	int		iFPS = 0;

	while (msg.message != WM_QUIT)
	{
		DWORD dwCurrTime = timeGetTime();
		DWORD dwElapsedTime = dwCurrTime - g_dwPrevTime;
		float fElapsedTimeInSecond = dwElapsedTime / 1000.f;
		g_dwPrevTime = dwCurrTime;

		g_fTime_Loop += fElapsedTimeInSecond;
		g_fTime_FPS += fElapsedTimeInSecond;
		g_fTime_Communicate += fElapsedTimeInSecond;

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);

				DispatchMessage(&msg);
			}
		}
		else if (g_fTime_Loop >= 1.f / FPS)
		{
			//게임의 루프
			EnterCriticalSection(&cs);
			g_pMainGame->Update(fElapsedTimeInSecond);
			LeaveCriticalSection(&cs);

			++iFPS;
			g_fTime_Loop = 0.f;
		}

		if (g_fTime_FPS >= 1.f)
		{
			//FPS 표시
			wsprintf(szFPS, L"FPS : %d", iFPS);
			SetWindowText(g_hWnd, szFPS);

			iFPS = 0;
			g_fTime_FPS = 0.f;
		}
	}

	// 임계영역 삭제
	DeleteCriticalSection(&cs);

    return (int) msg.wParam;
}

//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FRUIT_NINJA_SERVER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	RECT rc = { 0, 0, WINCX, WINCY };

	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   g_hWnd = hWnd;

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다.
            switch (wmId)
            {
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

DWORD WINAPI ServerMainThread(LPVOID arg)
{
	int retval; // 바이트 변수
	HANDLE hThread; // 쓰레드

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");
	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");
	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;

	while (1)
	{
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);

		if (client_sock == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}

		cout << "[TCP 서버] 클라이언트 접속: IP 주소=" << inet_ntoa(clientaddr.sin_addr) << ", 포트 번호=" << ntohs(clientaddr.sin_port) << endl;

		hThread = CreateThread(NULL, 0, CommunicateThread, (LPVOID)client_sock, 0, NULL);

		if (hThread == NULL)
			closesocket(client_sock);
		else
			CloseHandle(hThread);
	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

DWORD WINAPI CommunicateThread(LPVOID arg)
{
	int retval;
	int addrlen;
	SOCKET client_sock = (SOCKET)arg;
	SOCKADDR_IN clientaddr;

	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR *)&clientaddr, &addrlen);

	int iSerialNum;

	if (g_pMainGame->GetLogin0() == false)			// Login0가 비어있다면 채운다.
	{
		g_pMainGame->SetLogin0(true);
		iSerialNum = 1;
	}
	else if (g_pMainGame->GetLogin1() == false)		// Login1이 비어있다면 채운다. 
	{
		g_pMainGame->SetLogin1(true);
		iSerialNum = 2;
	}

	retval = send(client_sock, (char*)&iSerialNum, sizeof(int), 0);

	if (retval == SOCKET_ERROR)
	{
		err_display("send()");
		return 0;
	}

	while (1)
	{
		// 데이터 송수신
		if (g_fTime_Communicate >= 1.f / COMMUNICATE_PER_SECOND)
		{
			// GameInfo 보내기
			GameInfo gameInfo = g_pMainGame->GetGameInfo();

			retval = send(client_sock, (char*)&gameInfo, sizeof(GameInfo), 0);

			if (retval == SOCKET_ERROR)
			{
				err_display("send()");
				break;
			}

			// LinePos와 SerialNum 받기
			LinePosAndSerialNumInfo linePosAndSerialNumInfo;

			retval = recvn(client_sock, (char*)&linePosAndSerialNumInfo, sizeof(LinePosAndSerialNumInfo), 0);

			if (retval == SOCKET_ERROR)
			{
				err_display("recv()");
				break;
			}
			else if (retval == 0)
				break;

			if (linePosAndSerialNumInfo.iSerialNum == 1)
				g_pMainGame->SetLinePosInfo_Player1(linePosAndSerialNumInfo.linePosInfo);
			else if (linePosAndSerialNumInfo.iSerialNum == 2)
				g_pMainGame->SetLinePosInfo_Player2(linePosAndSerialNumInfo.linePosInfo);

			// 시간 초기화
			g_fTime_Communicate = 0.f;
		}
	}

	closesocket(client_sock);
	cout << "[TCP 서버] 클라이언트 종료: IP 주소=" << inet_ntoa(clientaddr.sin_addr) << ", 포트 번호=" << ntohs(clientaddr.sin_port) << endl;

	if (iSerialNum == 1)
	{
		g_pMainGame->SetLogin0(false);
	}
	else if (iSerialNum == 2)
	{
		g_pMainGame->SetLogin1(false);
	}

	return 0;
}