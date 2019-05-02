#pragma once

#include "resource.h"

#define SERVERPORT 9000
#define BUFSIZE    512
#define MAX_LOADSTRING 100
#define FPS 1000
#define COMMUNICATE_PER_SECOND 30

extern HWND hEdit;

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

	cout << "[" << msg << "]" << WCharToChar((LPTSTR)lpMsgBuf) << endl;
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

void recvToC(SOCKET s, char *buf, int len, int flags, SOCKADDR_IN csa)
{
	int temp_retvl;
	int fileSize = 0;
	int sizeCount = 0;
	len = 0;
	char temp[50] = "";

	while (1) {
		// 데이터 받기(고정 길이)
		temp_retvl = recvn(s, (char *)&len, sizeof(int), flags);

		if (temp_retvl == SOCKET_ERROR) 
		{
			err_display("recv()");
			break;
		}
		else if (temp_retvl == 0) break;

		// 데이터 받기(가변 길이)
		temp_retvl = recvn(s, buf, len, flags);
		if (temp_retvl == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (temp_retvl == 0) break;


		// 받은 데이터 출력
		buf[temp_retvl] = '\0';
		printf("[TCP/%s:%d] %s\n", inet_ntoa(csa.sin_addr), ntohs(csa.sin_port), buf);
	}

}

// 편집 컨트롤 출력 함수
void DisplayText(char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);

	char cbuf[BUFSIZE + 256];
	vsprintf(cbuf, fmt, arg);

	int nLength = GetWindowTextLength(hEdit);
	SendMessage(hEdit, EM_SETSEL, nLength, nLength);

	va_end(arg);
}