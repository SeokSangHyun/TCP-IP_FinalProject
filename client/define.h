
#ifndef Define_h__
#define Define_h__

#include "struct.h"

#define WINCX 800
#define WINCY 600
#define RESOURCEFILE_PATH "../Resource/"

extern HWND			g_hWnd;
extern void			CharToWChar(const char* pstrSrc, wchar_t pwstrDest[]);
extern int			g_iTime;
extern int			g_iSerialNum;
extern float		g_fDeleteTime;

#endif