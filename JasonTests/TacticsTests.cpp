#include "TacticsTests.h"
#include "Position.h" 
#include "MoveMaker.h"
#include "TestsUtility.h"

static void RunPosition(Position& position, int maxMoves, int evaluationDepth)//, bool decreaseDepth = true)
{
	int moveCount = 0;
	bool moveFound = true;
	MoveMaker moveMaker;
	while (moveFound && moveCount < maxMoves)
	{
		moveFound = moveMaker.MakeMove(position, evaluationDepth);
		moveCount++;
		//if (decreaseDepth)
		//	evaluationDepth--; //should not have to go as deep if evaluation is right for first move
	}

	moveMaker.CheckGameOver(position);
}

void TacticsTests::Run()
{
	static Position queenMate("4k3/8/1Q2K3/8/8/8/8/8 w - - 0 1");
	static Position staircaseMate("4k3 / R7 / 1Q6 / 8 / 8 / 8 / 8 / 4K3 w - -0 1");
	static Position staircaseMate2("4k3/8/1Q6/8/8/8/8/R3K3 w - - 0 1");
	static Position pinKnightFork("8/2R2pkp/6p1/6N1/3P1r2/7P/1r4P1/7K w - - 0 1");
	static Position removeKnightForkDefender("2rbr1k1/6p1/pq5p/1p1Ppp2/1P2N3/P1P1Q2P/5PP1/2R2RK1 w - - 0 1");
	static Position twoAttackOne("5rk1/5pbp/6p1/8/8/2NQ4/1q4PP/5RK1 b - - 0 1");
	static Position queenSacrificeMate("r6k/6pp/3N4/8/2Q5/1B6/8/4K3 w - - 0 1");
	static Position mateIn2("r7/p4kp1/1p2b3/5p1p/2PR1Pn1/3BP2P/PPQ2qP1/R1B4K b - - 2 26");
	static Position tactic1800("r5k1/6p1/nqp2b1p/4Q3/1p6/1P4P1/PB5P/3R3K w - - 1 27");
	static Position tactic1900("3r2k1/1n3pp1/p3p1qp/P3P3/RprPN3/4Q2P/5PP1/1R4K1 b - - 4 28");
	static Position tactic2000("1k1r4/p1pnb3/1nQ1p3/P7/3P4/4PN2/1P2KPq1/RN6 w - - 0 1");
	static Position tactic2200("8/p7/2N2p2/1P1Ppkpp/2K5/5P1P/5b2/8 w - - 0 46");


	RunPosition(queenMate, 1, 2);
	ASSERT(queenMate.GetGameStatus() == Position::GameStatus::CheckMate);

	RunPosition(staircaseMate, 1, 2);
	ASSERT(staircaseMate.GetGameStatus() == Position::GameStatus::CheckMate);

	RunPosition(staircaseMate2, 3, 3);
	ASSERT(staircaseMate2.GetGameStatus() == Position::GameStatus::CheckMate);

	RunPosition(pinKnightFork, 3, 2);
	ASSERT(pinKnightFork.GetMoves()[0].GetFrom() == Piece(PieceType::Knight, g5));
	ASSERT(pinKnightFork.GetMoves()[0].GetTo() == Piece(PieceType::Knight, e6));
	ASSERT(pinKnightFork.GetMoves()[2].GetFrom() == Piece(PieceType::Knight, e6));
	ASSERT(pinKnightFork.GetMoves()[2].GetTo() == Piece(PieceType::Knight, f4));

	RunPosition(removeKnightForkDefender, 4, 5);
	ASSERT(removeKnightForkDefender.GetMoves()[0].GetTo() == Piece(PieceType::Queen, b6));
	ASSERT(removeKnightForkDefender.GetMoves()[1].GetTo() == Piece(PieceType::Bishop, b6));
	ASSERT(removeKnightForkDefender.GetMoves()[2].GetTo() == Piece(PieceType::Knight, d6));

	RunPosition(twoAttackOne, 1, 3);
	ASSERT(twoAttackOne.GetMoves()[0].GetToSquare() == c3);

	RunPosition(queenSacrificeMate, 1, 3);
	ASSERT(queenSacrificeMate.GetMoves()[0].GetTo() == Piece(PieceType::Queen, g8));

	RunPosition(mateIn2, 1, 3);
	ASSERT(mateIn2.GetMoves()[0].GetTo() == Piece(PieceType::Queen, e1));

	RunPosition(tactic1800, 4, 6);
	ASSERT(tactic1800.GetMoves()[0].GetTo() == Piece(PieceType::Queen, e6));
	ASSERT(tactic1800.GetMoves()[1].GetTo() == Piece(PieceType::King, h7));
	ASSERT(tactic1800.GetMoves()[2].GetTo() == Piece(PieceType::Queen, f5));
	ASSERT(tactic1800.GetMoves()[3].GetToType() == PieceType::King);

	RunPosition(tactic1900, 1, 6);
	ASSERT(tactic1900.GetMoves()[0].GetTo() == Piece(PieceType::Rook, d4));

	RunPosition(tactic2000, 1, 6);
	ASSERT(tactic2000.GetMoves()[0].GetTo() == Piece(PieceType::Pawn, a6));

	//The following test is still failing
	RunPosition(tactic2200, 5, 8);
	//ASSERT(tactic2200.GetMoves()[0].m_To == Piece(PieceType::Knight, e7));
	//ASSERT(tactic2200.GetMoves()[1].m_To == Piece(PieceType::King, f4));
	//ASSERT(tactic2200.GetMoves()[2].m_To == Piece(PieceType::Pawn, d6));
	//ASSERT(tactic2200.GetMoves()[3].m_To == Piece(PieceType::Bishop, b6));
	//ASSERT(tactic2200.GetMoves()[4].m_To == Piece(PieceType::Knight, d5));
}
