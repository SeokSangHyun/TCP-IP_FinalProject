#pragma once

#include <iostream>
#include <stdio.h>
using namespace std;

#include "Define.h"
#include "struct.h"
#include "union.h"

extern HWND g_hWnd;
extern CRITICAL_SECTION cs;
extern float g_fTime_Wait;
extern float g_fTime_Term;
extern float g_fTime_Play;