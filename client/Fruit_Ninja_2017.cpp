// Fruit_Ninja_2017.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "Fruit_Ninja_2017.h"
#include "MainGame.h"
#include "Player.h"

#define MAX_LOADSTRING 100
#define SERVERPORT 9000
#define BUFSIZE    512
#define FPS 1000

wchar_t* CharToWChar(char* str)
{
	wchar_t* pStr;
	int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);

	pStr = new WCHAR[strSize];
	MultiByteToWideChar(CP_ACP, 0, str, strlen(str) + 1, pStr, strSize);

	return pStr;
}

char* WCharToChar(wchar_t* str)
{
	char* pStr;
	int strSize = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);

	pStr = new char[strSize];
	WideCharToMultiByte(CP_ACP, 0, str, -1, pStr, strSize, 0, 0);

	return pStr;
}

void DisplayText(char *fmt, ...);
void err_quit(char *msg);
void err_display(char *msg);
int recvn(SOCKET s, char *buf, int len, int flags);
DWORD WINAPI ClientMain(LPVOID arg);

HWND hEdit2;

// �Ӱ� ����
CRITICAL_SECTION cs;

// ���� ����
HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.
HWND g_hWnd;

// ���ӿ� �ʿ��� ���� ����
GameInfo		g_GameInfo;
CMainGame*		g_pMainGame;
int				g_iSerialNum;

// ��ſ� �ʿ��� ���� ����
SOCKET			sock;
SOCKADDR_IN		serveraddr;

// �ð� ���� ���� ����
DWORD				g_dwPrevTime = 0;
float				g_fTime_Loop = 0.f;
float				g_fTime_FPS = 0.f;
int					g_iTime = 0;
UnionDataForFloat	g_fGameTime;
float				g_fDeleteTime;

// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

INT_PTR WINAPI MainDlgProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	char addrText[32] = { '0', }, *tempText;
	int size = 0, count = 0;
	switch (iMessage)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case ID_INROOM:
			GetDlgItemText(hWnd, IDC_IPADDRESS1, (LPWSTR)addrText, sizeof(addrText));

			for (int i = 0; i < sizeof(addrText); ++i)
			{
				if (addrText[i] != '\0') {
					++count;
				}
			}
			tempText = new char[count] {'\0', };
			size = count;
			count = 0;
			for (int i = 0; i < sizeof(addrText); ++i)
			{
				if (addrText[i] != '\0')
				{
					tempText[count] = addrText[i];
					++count;

					if (size == count)
					{
						tempText[count] = '\0';
						//
						ZeroMemory(&serveraddr, sizeof(serveraddr));
						serveraddr.sin_family = AF_INET;
						serveraddr.sin_port = htons(SERVERPORT);
						serveraddr.sin_addr.s_addr = inet_addr(tempText);

						EndDialog(hWnd, 0);
						return TRUE;
					}
				}
			}
			EndDialog(hWnd, 0);
			return TRUE;
		case IDCANCEL:
			exit(1);
		case ID_EXIT:
			exit(1);
		}
		return TRUE;
	}

	return FALSE;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ���⿡ �ڵ带 �Է��մϴ�.
	MSG msg;
	HACCEL hAccelTable;
    // ���� ���ڿ��� �ʱ�ȭ�մϴ�.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_FRUIT_NINJA_2017, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

	DialogBox(hInstance, MAKEINTRESOURCE(IDD_IPDLG), HWND_DESKTOP, MainDlgProc);

    // ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FRUIT_NINJA_2017));

	msg.message = WM_NULL;

	// �Ӱ迵�� �ʱ�ȭ
	InitializeCriticalSection(&cs);

	// ���� ��� ������ ����
	CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);

	g_pMainGame = new CMainGame();
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
		g_fDeleteTime = fElapsedTimeInSecond;

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);

				DispatchMessage(&msg);
			}
		}
		else  if (g_fTime_Loop >= 1.f / FPS)
		{
			// ������ ����
			EnterCriticalSection(&cs);
			g_pMainGame->Update(g_GameInfo);
			LeaveCriticalSection(&cs);

			g_pMainGame->Render();

			++iFPS;
			g_fTime_Loop = 0.f;
			//g_iTime = (int)g_fGameTime.fData;
		}

		if (g_fTime_FPS >= 1.f)
		{
			//FPS ǥ��
			wsprintf(szFPS, L"FPS : %d", iFPS);
			//wsprintf(szFPS, L"Time : %d", g_iTime);
			SetWindowText(g_hWnd, szFPS);

			iFPS = 0;
			g_fTime_FPS = 0.f;
		}
	}

	// �Ӱ迵�� ����
	DeleteCriticalSection(&cs);

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FRUIT_NINJA_2017));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = nullptr;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

// ���� ��Ʈ�� ��� �Լ�
void DisplayText(char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);

	char cbuf[BUFSIZE + 256];
	vsprintf(cbuf, fmt, arg);

	int nLength = GetWindowTextLength(hEdit2);
	SendMessage(hEdit2, EM_SETSEL, nLength, nLength);
	SendMessage(hEdit2, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

	va_end(arg);
}

// ���� �Լ� ���� ��� �� ����
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);

	MessageBox(NULL, (LPCTSTR)lpMsgBuf, CharToWChar(msg), MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// ���� �Լ� ���� ���
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	cout << "[" << msg << "]" << WCharToChar((LPTSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// ����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}

// TCP Ŭ���̾�Ʈ ���� �κ�
DWORD WINAPI ClientMain(LPVOID arg)
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	// SerialNum �ޱ�
	retval = recvn(sock, (char*)&g_iSerialNum, sizeof(int), 0);

	if (retval == SOCKET_ERROR)
	{
		err_display("recv()");
		return 0;
	}
	else if (retval == 0)
		return 0;

	// ������ ������ ���
	while (1)
	{
		// GameInfo �ޱ�
		retval = recvn(sock, (char*)&g_GameInfo, sizeof(GameInfo), 0);

		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			return 0;
		}
		else if (retval == 0)
			return 0;

		// LienPos�� SerialNum ������
		LinePosAndSerialNumInfo linePosAndSerialNumInfo;

		if (g_pMainGame->GetPlayer() == NULL)
			linePosAndSerialNumInfo.linePosInfo.iLinePosCount = 0;
		else
		{
			linePosAndSerialNumInfo.linePosInfo = g_pMainGame->GetPlayer()->GetLinePosInfo();
			linePosAndSerialNumInfo.iSerialNum = g_iSerialNum;
		}

		retval = send(sock, (char*)&linePosAndSerialNumInfo, sizeof(LinePosAndSerialNumInfo), 0);

		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			return 0;
		}
		else if (retval == 0)
			return 0;
	}

	return 0;
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

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	   CW_USEDEFAULT, 0, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);
   //x, y, width, height
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
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

    switch (message)
    {
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
		break;
    case WM_COMMAND:
        {
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
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
            hdc = BeginPaint(hWnd, &ps);
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
