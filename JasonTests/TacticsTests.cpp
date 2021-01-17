#include "TacticsTests.h"
#include "Position.h" 
#include "MoveMaker.h"
#include <assert.h>

Position queenMate("4k3/8/1Q2K3/8/8/8/8/8 w - - 0 1");
Position staircaseMate("4k3 / R7 / 1Q6 / 8 / 8 / 8 / 8 / 4K3 w - -0 1");
Position staircaseMate2("4k3/8/1Q6/8/8/8/8/R3K3 w - - 0 1");

static void RunPosition(Position& position, int maxMoves)
{
	int moveCount = 0;
	bool moveFound = true;
	while (moveFound && moveCount < maxMoves)
	{
		moveFound = MoveMaker::MakeMove(position);
		moveCount++;
	}

	MoveMaker::CheckGameOver(position);
}

void TacticsTests::Run()
{
	RunPosition(queenMate, 1);
	assert(queenMate.GetGameStatus() == Position::GameStatus::CheckMate);

	RunPosition(staircaseMate, 1);
	assert(staircaseMate.GetGameStatus() == Position::GameStatus::CheckMate);

	RunPosition(staircaseMate2, 3);
	assert(staircaseMate2.GetGameStatus() == Position::GameStatus::CheckMate);
}
