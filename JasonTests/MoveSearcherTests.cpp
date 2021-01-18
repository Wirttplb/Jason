#include "MoveSearcherTests.h"
#include "Position.h"
#include "MoveSearcher.h"
#include "TestsUtility.h"
#include <unordered_set>

#include "NotationParser.h"

static const Position staleMateWhiteToPlay("8/8/8/8/8/kq6/8/K7 w - - 0 1");
static const Position staleMateBlackToPlay("8/8/8/8/8/KQ6/8/k7 b - - 0 1");
static const Position perftPosition2("r3k2r / p1ppqpb1 / bn2pnp1 / 3PN3 / 1p2P3 / 2N2Q1p / PPPBBPPP / R3K2R w KQkq -");
static const Position perftPosition3("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -");
static const Position perftPosition4("r3k2r / Pppp1ppp / 1b3nbN / nP6 / BBP1P3 / q4N2 / Pp1P2PP / R2Q1RK1 w kq - 0 1");
static const Position perftPosition4bis("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1");
static const Position perftPosition5("rnbq1k1r / pp1Pbppp / 2p5 / 8 / 2B5 / 8 / PPP1NnPP / RNBQK2R w KQ - 1 8");
static const Position perftPosition6("r4rk1 / 1pp1qppp / p1np1n2 / 2b1p1B1 / 2B1P1b1 / P1NP1N2 / 1PP1QPPP / R4RK1 w - -0 10");

//For debug
//int capture = 0;
//int castles = 0;
//int checks = 0;
//int checkmates = 0;
//for (const Position& p : positions)
//{
//	if (p.GetMoves().back().IsCastling())
//		castles++;
//	if (MoveSearcher::IsKingInCheck(p, p.IsWhiteToPlay()))
//	{
//		checks++;
//		if (MoveSearcher::GetLegalMoves(p).empty())
//			checkmates++;
//	}
//}


static void TestIllegalCastles()
{
	//legal castles
	Position position("4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1");
	std::vector<Position> positions = MoveSearcher::GetAllPossiblePositions(position, 1);
	int castles = 0;
	for (const Position& p : positions)
	{
		if (p.GetMoves().back().IsCastling())
			castles++;
	}

	ASSERT(castles == 2);

	//pawn controls square
	position = Position("4k3/8/8/8/8/8/2p3p1/R3K2R w KQ - 0 1");
	positions = MoveSearcher::GetAllPossiblePositions(position, 1);
	castles = 0;
	for (const Position& p : positions)
	{
		if (p.GetMoves().back().IsCastling())
			castles++;
	}

	ASSERT(castles == 0);

	//king in check
	position = Position("4k3/4r3/8/8/8/8/8/R3K2R w KQ - 0 1");
	positions = MoveSearcher::GetAllPossiblePositions(position, 1);
	castles = 0;
	for (const Position& p : positions)
	{
		if (p.GetMoves().back().IsCastling())
			castles++;
	}

	ASSERT(castles == 0);

	//enemy piece blocks
	position = Position("4k3/8/8/8/8/8/8/R1n1K1nR w KQ - 0 1");
	positions = MoveSearcher::GetAllPossiblePositions(position, 1);
	castles = 0;
	for (const Position& p : positions)
	{
		if (p.GetMoves().back().IsCastling())
			castles++;
	}

	ASSERT(castles == 0);

	//enemy piece blocks #2
	position = Position("4k3/8/8/8/8/8/8/R2nKn1R w KQ - 0 1");
	positions = MoveSearcher::GetAllPossiblePositions(position, 1);
	castles = 0;
	for (const Position& p : positions)
	{
		if (p.GetMoves().back().IsCastling())
			castles++;
	}

	ASSERT(castles == 0);

	//enemy piece blocks #3
	position = Position("4k3/8/8/8/8/6n1/8/Rn2K2R w KQ - 0 1");
	positions = MoveSearcher::GetAllPossiblePositions(position, 1);
	castles = 0;
	for (const Position& p : positions)
	{
		if (p.GetMoves().back().IsCastling())
			castles++;
	}

	ASSERT(castles == 0);

	//friend piece blocks
	position = Position("4k3/8/8/8/8/8/8/R1N1KN1R w KQ - 0 1");
	positions = MoveSearcher::GetAllPossiblePositions(position, 1);
	castles = 0;
	for (const Position& p : positions)
	{
		if (p.GetMoves().back().IsCastling())
			castles++;
	}

	ASSERT(castles == 0);
}

void MoveSearcherTests::Run()
{
	TestIllegalCastles();

	//COMMENTED LINES ARE CURRENT FAILURES

	//simple stalemate
	std::vector<Position> positions = MoveSearcher::GetAllPossiblePositions(staleMateWhiteToPlay);
	std::unordered_set<Position> uniquePositions;
	ASSERT(positions.empty());

	positions = MoveSearcher::GetAllPossiblePositions(staleMateBlackToPlay);
	ASSERT(positions.empty());

	//starting position
	Position startingPosition;
	positions = MoveSearcher::GetAllPossiblePositions(startingPosition);
	ASSERT(positions.size() == 20);

	positions = MoveSearcher::GetAllPossiblePositions(startingPosition, 1);
	uniquePositions = MoveSearcher::GetAllUniquePositions(startingPosition, 1);
	ASSERT(positions.size() == 20);
	ASSERT(uniquePositions.size() == 20);

	positions = MoveSearcher::GetAllPossiblePositions(startingPosition, 2);
	uniquePositions = MoveSearcher::GetAllUniquePositions(startingPosition, 2);
	ASSERT(positions.size() == 20 * 20);
	ASSERT(uniquePositions.size() == 20 * 20);

	positions = MoveSearcher::GetAllPossiblePositions(startingPosition, 3);
	uniquePositions = MoveSearcher::GetAllUniquePositions(startingPosition, 3);
	ASSERT(positions.size() == 8902);
	ASSERT(uniquePositions.size() == 7602); //5362 if we dont count en passant

	positions = MoveSearcher::GetAllPossiblePositions(startingPosition, 4);
	uniquePositions = MoveSearcher::GetAllUniquePositions(startingPosition, 4);
	ASSERT(positions.size() == 197281);
	ASSERT(uniquePositions.size() == 101240); //72084 if we dont count en passant

	positions = MoveSearcher::GetAllPossiblePositions(startingPosition, 4);
	ASSERT(positions.size() == 197281);

	positions = MoveSearcher::GetAllPossiblePositions(startingPosition, 5);
	ASSERT(positions.size() == 4865609);

	//Perft #2
	positions = MoveSearcher::GetAllPossiblePositions(perftPosition2, 1);
	ASSERT(positions.size() == 48);

	positions = MoveSearcher::GetAllPossiblePositions(perftPosition2, 2);
	ASSERT(positions.size() == 2039);

	positions = MoveSearcher::GetAllPossiblePositions(perftPosition2, 3);
	ASSERT(positions.size() == 97862);

	positions = MoveSearcher::GetAllPossiblePositions(perftPosition2, 4);
	ASSERT(positions.size() == 4085603);

	//Perft #3
	positions = MoveSearcher::GetAllPossiblePositions(perftPosition3, 1);
	ASSERT(positions.size() == 14);

	positions = MoveSearcher::GetAllPossiblePositions(perftPosition3, 2);
	ASSERT(positions.size() == 191);

	positions = MoveSearcher::GetAllPossiblePositions(perftPosition3, 3);
	ASSERT(positions.size() == 2812);

	positions = MoveSearcher::GetAllPossiblePositions(perftPosition3, 4);
	ASSERT(positions.size() == 43238);

	positions = MoveSearcher::GetAllPossiblePositions(perftPosition3, 5);
	ASSERT(positions.size() == 674624);

	positions = MoveSearcher::GetAllPossiblePositions(perftPosition3, 6);
	ASSERT(positions.size() == 11030083);

	//Perft #4
	positions = MoveSearcher::GetAllPossiblePositions(perftPosition4, 1);
	ASSERT(positions.size() == 6);

	positions = MoveSearcher::GetAllPossiblePositions(perftPosition4, 2);
	ASSERT(positions.size() == 264);

	positions = MoveSearcher::GetAllPossiblePositions(perftPosition4, 3);
	ASSERT(positions.size() == 9467);

	positions = MoveSearcher::GetAllPossiblePositions(perftPosition4, 4);
	ASSERT(positions.size() == 422333);

	//Perft #4 bis
	positions = MoveSearcher::GetAllPossiblePositions(perftPosition4bis, 1);
	ASSERT(positions.size() == 6);

	positions = MoveSearcher::GetAllPossiblePositions(perftPosition4bis, 2);
	ASSERT(positions.size() == 264);

	positions = MoveSearcher::GetAllPossiblePositions(perftPosition4bis, 3);
	ASSERT(positions.size() == 9467);

	positions = MoveSearcher::GetAllPossiblePositions(perftPosition4bis, 4);
	ASSERT(positions.size() == 422333);

	//Perft #5
	positions = MoveSearcher::GetAllPossiblePositions(perftPosition5, 1);
	ASSERT(positions.size() == 44);

	positions = MoveSearcher::GetAllPossiblePositions(perftPosition5, 2);
	ASSERT(positions.size() == 1486);

	positions = MoveSearcher::GetAllPossiblePositions(perftPosition5, 3);
	ASSERT(positions.size() == 62379);

	positions = MoveSearcher::GetAllPossiblePositions(perftPosition5, 4);
	ASSERT(positions.size() == 2103487);

	//Perft #6
	positions = MoveSearcher::GetAllPossiblePositions(perftPosition6, 1);
	ASSERT(positions.size() == 46);

	positions = MoveSearcher::GetAllPossiblePositions(perftPosition6, 2);
	ASSERT(positions.size() == 2079);

	positions = MoveSearcher::GetAllPossiblePositions(perftPosition6, 3);
	ASSERT(positions.size() == 89890);

	positions = MoveSearcher::GetAllPossiblePositions(perftPosition6, 4);
	ASSERT(positions.size() == 3894594);
}