// Fruit_Ninja_Server.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "Fruit_Ninja_Server.h"
#include "MainGame.h"

DWORD WINAPI ServerMainThread(LPVOID arg);
DWORD WINAPI CommunicateThread(LPVOID arg);

// �Ӱ� ����
CRITICAL_SECTION cs;

// ���� ����:
HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.
HWND g_hWnd;
HWND hEdit;

// ���ΰ��� Ŭ���� ����
CMainGame* g_pMainGame = new CMainGame();

// �ð� ���� ���� ����
DWORD g_dwPrevTime = 0;
float g_fTime_Loop = 0.f;
float g_fTime_FPS = 0.f;
float g_fTime_Communicate = 0.f;

// ���� ����
LinePosInfo g_linePosInfo_Player1;
LinePosInfo g_linePosInfo_Player2;

// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
#ifdef _DEBUG
	//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")	//��Ƽ����Ʈ
	#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")		//�����ڵ�
#endif

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ���⿡ �ڵ带 �Է��մϴ�.
	MSG msg;
	HACCEL hAccelTable;

    // ���� ���ڿ��� �ʱ�ȭ�մϴ�.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_FRUIT_NINJA_SERVER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FRUIT_NINJA_SERVER));

	msg.message = WM_NULL;

	// �Ӱ迵�� �ʱ�ȭ
	InitializeCriticalSection(&cs);

	// ���� ��� ������ ����
	CreateThread(NULL, 0, ServerMainThread, NULL, 0, NULL);
	
	// ���ΰ��� Ŭ���� �ʱ�ȭ
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
			//������ ����
			EnterCriticalSection(&cs);
			g_pMainGame->Update(fElapsedTimeInSecond);
			LeaveCriticalSection(&cs);

			++iFPS;
			g_fTime_Loop = 0.f;
		}

		if (g_fTime_FPS >= 1.f)
		{
			//FPS ǥ��
			wsprintf(szFPS, L"FPS : %d", iFPS);
			SetWindowText(g_hWnd, szFPS);

			iFPS = 0;
			g_fTime_FPS = 0.f;
		}
	}

	// �Ӱ迵�� ����
	DeleteCriticalSection(&cs);

    return (int) msg.wParam;
}

//
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
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
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

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
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����:  �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND  - ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT    - �� â�� �׸��ϴ�.
//  WM_DESTROY  - ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // �޴� ������ ���� �м��մϴ�.
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
            // TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.
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
	int retval; // ����Ʈ ����
	HANDLE hThread; // ������

	// ���� �ʱ�ȭ
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

	// ������ ��ſ� ����� ����
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

		cout << "[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=" << inet_ntoa(clientaddr.sin_addr) << ", ��Ʈ ��ȣ=" << ntohs(clientaddr.sin_port) << endl;

		hThread = CreateThread(NULL, 0, CommunicateThread, (LPVOID)client_sock, 0, NULL);

		if (hThread == NULL)
			closesocket(client_sock);
		else
			CloseHandle(hThread);
	}

	// closesocket()
	closesocket(listen_sock);

	// ���� ����
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

	if (g_pMainGame->GetLogin0() == false)			// Login0�� ����ִٸ� ä���.
	{
		g_pMainGame->SetLogin0(true);
		iSerialNum = 1;
	}
	else if (g_pMainGame->GetLogin1() == false)		// Login1�� ����ִٸ� ä���. 
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
		// ������ �ۼ���
		if (g_fTime_Communicate >= 1.f / COMMUNICATE_PER_SECOND)
		{
			// GameInfo ������
			GameInfo gameInfo = g_pMainGame->GetGameInfo();

			retval = send(client_sock, (char*)&gameInfo, sizeof(GameInfo), 0);

			if (retval == SOCKET_ERROR)
			{
				err_display("send()");
				break;
			}

			// LinePos�� SerialNum �ޱ�
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

			// �ð� �ʱ�ȭ
			g_fTime_Communicate = 0.f;
		}
	}

	closesocket(client_sock);
	cout << "[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=" << inet_ntoa(clientaddr.sin_addr) << ", ��Ʈ ��ȣ=" << ntohs(clientaddr.sin_port) << endl;

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