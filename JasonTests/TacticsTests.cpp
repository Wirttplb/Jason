#include "TacticsTests.h"
#include "Position.h" 
#include "MoveMaker.h"
#include "TestsUtility.h"

Position queenMate("4k3/8/1Q2K3/8/8/8/8/8 w - - 0 1");
Position staircaseMate("4k3 / R7 / 1Q6 / 8 / 8 / 8 / 8 / 4K3 w - -0 1");
Position staircaseMate2("4k3/8/1Q6/8/8/8/8/R3K3 w - - 0 1");
Position pinKnightFork("8/2R2pkp/6p1/6N1/3P1r2/7P/1r4P1/7K w - - 0 1");
Position removeKnightForkDefender("2rbr1k1/6p1/pq5p/1p1Ppp2/1P2N3/P1P1Q2P/5PP1/2R2RK1 w - - 0 1");
Position twoAttackOne("5rk1/5pbp/6p1/8/8/2NQ4/1q4PP/5RK1 b - - 0 1");
Position queenSacrificeMate("r6k/6pp/3N4/8/2Q5/1B6/8/4K3 w - - 0 1");

static void RunPosition(Position& position, int maxMoves, int evaluationDepth, bool decreaseDepth = true)
{
	int moveCount = 0;
	bool moveFound = true;
	MoveMaker moveMaker;
	while (moveFound && moveCount < maxMoves)
	{
		moveFound = moveMaker.MakeMove(position, evaluationDepth);
		moveCount++;
		if (decreaseDepth)
			evaluationDepth--; //should not have to go as deep if evaluation is right for first move
	}

	MoveMaker::CheckGameOver(position);
}

void TacticsTests::Run()
{
	RunPosition(queenMate, 1, 2);
	ASSERT(queenMate.GetGameStatus() == Position::GameStatus::CheckMate);

	RunPosition(staircaseMate, 1, 2);
	ASSERT(staircaseMate.GetGameStatus() == Position::GameStatus::CheckMate);

	RunPosition(staircaseMate2, 3, 2);
	ASSERT(staircaseMate2.GetGameStatus() == Position::GameStatus::CheckMate);

	RunPosition(pinKnightFork, 3, 2);
	ASSERT(pinKnightFork.GetMoves()[0].m_From == Piece(PieceType::Knight, g5));
	ASSERT(pinKnightFork.GetMoves()[0].m_To == Piece(PieceType::Knight, e6));
	ASSERT(pinKnightFork.GetMoves()[2].m_From == Piece(PieceType::Knight, e6));
	ASSERT(pinKnightFork.GetMoves()[2].m_To == Piece(PieceType::Knight, f4));

	//5 IS TOO DEEP!
	//RunPosition(removeKnightForkDefender, 4, 5);
	//ASSERT(removeKnightForkDefender.GetMoves()[0].m_To == Piece(PieceType::Queen, 1, 5));
	//ASSERT(removeKnightForkDefender.GetMoves()[1].m_To == Piece(PieceType::Bishop, 1, 5));
	//ASSERT(removeKnightForkDefender.GetMoves()[2].m_To == Piece(PieceType::Knight, 3, 5));

	/*Move move;
	move.m_From = Piece(PieceType::Queen, e3);
	move.m_To = Piece(PieceType::Queen, b6);
	const double score = PositionEvaluation::EvaluateMove(removeKnightForkDefender, move, 4);
	ASSERT(score > 0);*/

	RunPosition(twoAttackOne, 1, 3);
	ASSERT(twoAttackOne.GetMoves()[0].m_To.m_Square == c3);

	RunPosition(queenSacrificeMate, 1, 3);
	ASSERT(queenSacrificeMate.GetMoves()[0].m_To == Piece(PieceType::Queen, g8));
}
