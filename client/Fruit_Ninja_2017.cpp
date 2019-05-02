// Fruit_Ninja_2017.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
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

// 임계 영역
CRITICAL_SECTION cs;

// 전역 변수
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
HWND g_hWnd;

// 게임에 필요한 전역 변수
GameInfo		g_GameInfo;
CMainGame*		g_pMainGame;
int				g_iSerialNum;

// 통신에 필요한 전역 변수
SOCKET			sock;
SOCKADDR_IN		serveraddr;

// 시간 관련 전역 변수
DWORD				g_dwPrevTime = 0;
float				g_fTime_Loop = 0.f;
float				g_fTime_FPS = 0.f;
int					g_iTime = 0;
UnionDataForFloat	g_fGameTime;
float				g_fDeleteTime;

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
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

    // TODO: 여기에 코드를 입력합니다.
	MSG msg;
	HACCEL hAccelTable;
    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_FRUIT_NINJA_2017, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

	DialogBox(hInstance, MAKEINTRESOURCE(IDD_IPDLG), HWND_DESKTOP, MainDlgProc);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FRUIT_NINJA_2017));

	msg.message = WM_NULL;

	// 임계영역 초기화
	InitializeCriticalSection(&cs);

	// 소켓 통신 스레드 생성
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
			// 게임의 루프
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
			//FPS 표시
			wsprintf(szFPS, L"FPS : %d", iFPS);
			//wsprintf(szFPS, L"Time : %d", g_iTime);
			SetWindowText(g_hWnd, szFPS);

			iFPS = 0;
			g_fTime_FPS = 0.f;
		}
	}

	// 임계영역 삭제
	DeleteCriticalSection(&cs);

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FRUIT_NINJA_2017));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = nullptr;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

// 편집 컨트롤 출력 함수
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

// 소켓 함수 오류 출력 후 종료
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

// 소켓 함수 오류 출력
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

// 사용자 정의 데이터 수신 함수
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

// TCP 클라이언트 시작 부분
DWORD WINAPI ClientMain(LPVOID arg)
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	// SerialNum 받기
	retval = recvn(sock, (char*)&g_iSerialNum, sizeof(int), 0);

	if (retval == SOCKET_ERROR)
	{
		err_display("recv()");
		return 0;
	}
	else if (retval == 0)
		return 0;

	// 서버와 데이터 통신
	while (1)
	{
		// GameInfo 받기
		retval = recvn(sock, (char*)&g_GameInfo, sizeof(GameInfo), 0);

		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			return 0;
		}
		else if (retval == 0)
			return 0;

		// LienPos와 SerialNum 보내기
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
            hdc = BeginPaint(hWnd, &ps);
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
