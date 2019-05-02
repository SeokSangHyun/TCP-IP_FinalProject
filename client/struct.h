#ifndef Struct_h__
#define Struct_h__

#include "enum.h"

struct Point_F 
{
	float x;
	float y;
};

struct PointInfo
{
	Point_F	pos;
	float	fDeleteTime;
};

struct FruitPacket
{
	Point_F	ptFruitPos;
	int		iState;
	int		iRadian;
	int		iSort;
	bool	bSliced;
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

#endif // Struct_h__