#pragma once

#include "enum.h"

struct Point_F 
{
	float x;
	float y;
};

struct FruitPacket
{
	Point_F	ptPos;
	int		iState;
	int		iRadian;
	int		iSort;
	bool	bSliced;
};

struct FruitInfo
{
	FruitPacket FruitPacket;
	Point_F		ptDir;			//과일 방향
	Point_F		ptSpeed;		//과일의 속도
	Point_F		ptAccel;		//과일 속도 변화량
	float		fDeleteTime;
};

struct LinePosInfo
{
	int		iLinePosCount;
	Point_F LinePos[1000];
};

struct LinePosAndSerialNumInfo
{
	int			iSerialNum;
	LinePosInfo linePosInfo;
};

struct GameInfo
{
	int			iScore_Player1;
	int			iScore_Player2;
	int			iFruitInfoCount;
	float		fTime;
	LinePosInfo LinePosInfo_Player1;
	LinePosInfo LinePosInfo_Player2;
	FruitPacket	FruitInfo[100];
	GameState	enumGameState;
};

