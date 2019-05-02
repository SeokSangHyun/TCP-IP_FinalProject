#pragma once

enum GameState
{
	GameState_Wait,
	GameState_Term,
	GameState_Play,
	GameState_End
};

enum FruitState 
{ 
	rise = 1, 
	down 
};

enum FruitSort
{
	Lemon,
	Strawberry,
	Kiwi,
	Peach,
	Watermelon,
	Bomb,
	Lemon_Sliced,
	Strawberry_Sliced,
	Kiwi_Sliced,
	Peach_Sliced,
	Watermelon_Sliced,
	Bomb_Sliced,
	end
};
 
enum GameTime
{
	GameTime_Wait = 0,
	GameTime_Term = 5,
	GameTime_Play = 30,
	GameTime_End = 10
};