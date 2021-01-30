#include "MoveSearcherTests.h"
#include "Position.h"
#include "MoveSearcher.h"
#include "TestsUtility.h"
#include <unordered_set>

#include "NotationParser.h"

static Position staleMateWhiteToPlay("8/8/8/8/8/kq6/8/K7 w - - 0 1");
static Position staleMateBlackToPlay("8/8/8/8/8/KQ6/8/k7 b - - 0 1");
static Position perftPosition2("r3k2r / p1ppqpb1 / bn2pnp1 / 3PN3 / 1p2P3 / 2N2Q1p / PPPBBPPP / R3K2R w KQkq -");
static Position perftPosition3("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -");
static Position perftPosition4("r3k2r / Pppp1ppp / 1b3nbN / nP6 / BBP1P3 / q4N2 / Pp1P2PP / R2Q1RK1 w kq - 0 1");
static Position perftPosition4bis("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1");
static Position perftPosition5("rnbq1k1r / pp1Pbppp / 2p5 / 8 / 2B5 / 8 / PPP1NnPP / RNBQK2R w KQ - 1 8");
static Position perftPosition6("r4rk1 / 1pp1qppp / p1np1n2 / 2b1p1B1 / 2B1P1b1 / P1NP1N2 / 1PP1QPPP / R4RK1 w - -0 10");

static void TestRookMoves()
{
	Position position("4k3/8/8/8/8/8/6K1/R7 w - - 0 1");
	Piece rook(PieceType::Rook, a1);
	std::vector<Move> moves = MoveSearcher::GetLegalMoves(position, rook, true);
	ASSERT(moves.size() == 14);

	moves = MoveSearcher::GetLegalMovesFromBitboards(position, rook, true);
	ASSERT(moves.size() == 14);

	position = Position("4k3/8/8/N7/8/8/4K3/R2N4 w - - 0 1");
	moves = MoveSearcher::GetLegalMovesFromBitboards(position, rook, true);
	ASSERT(moves.size() == 5);

	position = Position("4k3/8/8/n7/8/8/4K3/R2N4 w - - 0 1");
	moves = MoveSearcher::GetLegalMovesFromBitboards(position, rook, true);
	ASSERT(moves.size() == 6);

	rook = Piece(PieceType::Rook, g5);
	position = Position("4k1n1/8/8/N5Rn/8/8/4K3/8 w - - 0 1");
	moves = MoveSearcher::GetLegalMovesFromBitboards(position, rook, true);
	ASSERT(moves.size() == 13);

	rook = Piece(PieceType::Rook, b1);
	position = Position("8/8/8/8/8/8/8/nrNnnnnn b - - 0 1");
	moves = MoveSearcher::GetLegalMovesFromBitboards(position, rook, false);
	ASSERT(moves.size() == 8);
}

static void TestBishopMoves()
{
	Position position("4k3/8/8/8/8/8/4K3/B7 w - - 0 1");
	Piece bishop(PieceType::Bishop, a1);
	std::vector<Move> moves = MoveSearcher::GetLegalMovesFromBitboards(position, bishop, true);
	ASSERT(moves.size() == 7);

	position = Position("4k3/8/8/8/8/8/1B2K3/8 w - - 0 1");
	bishop.m_Square = b2;
	moves = MoveSearcher::GetLegalMovesFromBitboards(position, bishop, true);
	ASSERT(moves.size() == 9);

	position = Position("4k3/8/8/8/3n4/N7/1B3K2/8 w - - 0 1");
	moves = MoveSearcher::GetLegalMovesFromBitboards(position, bishop, true);
	ASSERT(moves.size() == 4);

	position = Position("1n2k3/pp4pp/3B4/4N3/3n4/N5N1/5K2/8 w - - 0 1");
	bishop.m_Square = d6;
	moves = MoveSearcher::GetLegalMovesFromBitboards(position, bishop, true);
	ASSERT(moves.size() == 6);

	position = Position("1n1nk3/pp2n1pp/5nN1/6B1/8/N3N3/3nNK2/2n5 w - - 0 1");
	bishop.m_Square = g5;
	moves = MoveSearcher::GetLegalMovesFromBitboards(position, bishop, true);
	ASSERT(moves.size() == 4);

	position = Position("7B/8/8/8/8/8/8/8 w - - 0 1");
	bishop.m_Square = h8;
	moves = MoveSearcher::GetLegalMovesFromBitboards(position, bishop, true);
	ASSERT(moves.size() == 7);

	position = Position("B7/8/8/8/8/8/8/8 w - - 0 1");
	bishop.m_Square = a8;
	moves = MoveSearcher::GetLegalMovesFromBitboards(position, bishop, true);
	ASSERT(moves.size() == 7);

	position = Position("8/8/8/8/8/8/8/7B w - - 0 1");
	bishop.m_Square = h1;
	moves = MoveSearcher::GetLegalMovesFromBitboards(position, bishop, true);
	ASSERT(moves.size() == 7);

	position = Position("B7/7B/8/8/4B3/8/8/1B5B w - - 0 1");
	bishop.m_Square = e4;
	moves = MoveSearcher::GetLegalMovesFromBitboards(position, bishop, true);
	ASSERT(moves.size() == 9);

	//there was a bug on this position
	position = Position("r3k2r/pbppqpb1/1nN1pnp1/3P4/1p2P3/5Q1p/PPPBBPPP/RN2K2R b KQq - 0 1");
	bishop.m_Square = b7;
	moves = MoveSearcher::GetLegalMovesFromBitboards(position, bishop, false);
	ASSERT(moves.size() == 3);
}

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

static void TestPawnMoves()
{
	Position position("4k3/2p5/8/8/8/8/2P5/4K3 w - - 0 1");
	Piece pawn(PieceType::Pawn, c2);
	std::vector<Move> moves = MoveSearcher::GetLegalMoves(position, pawn, true);
	ASSERT(moves.size() == 2);
	ASSERT(moves.front() == Move(PieceType::Pawn, c2, c3));
	ASSERT(moves.back() == Move(PieceType::Pawn, c2, c4));

	pawn = Piece(PieceType::Pawn, c7);
	moves = MoveSearcher::GetLegalMoves(position, pawn, false);
	ASSERT(moves.size() == 2);
	ASSERT(moves.front() == Move(PieceType::Pawn, c7, c6));
	ASSERT(moves.back() == Move(PieceType::Pawn, c7, c5));

	position = Position("4k3 / 2p5 / 8 / 8 / 8 / 1n1n4 / 2P5 / 5K2 w - -0 1");
	pawn = Piece(PieceType::Pawn, c2);
	moves = MoveSearcher::GetLegalMoves(position, pawn, true);
	ASSERT(moves.size() == 4);
	ASSERT(moves[0] == Move(PieceType::Pawn, c2, c3));
	ASSERT(moves[1] == Move(PieceType::Pawn, c2, c4));
	ASSERT(moves[2] == Move(PieceType::Pawn, c2, b3));
	ASSERT(moves[3] == Move(PieceType::Pawn, c2, d3));

	position = Position("4k3 / 2p5 / 8 / 8 / 8 / 1n1n4 / 2P5 / 5K2 w - -0 1");
	pawn = Piece(PieceType::Pawn, c2);
	moves = MoveSearcher::GetLegalMoves(position, pawn, true);
	ASSERT(moves.size() == 4);
	ASSERT(moves[0] == Move(PieceType::Pawn, c2, c3));
	ASSERT(moves[1] == Move(PieceType::Pawn, c2, c4));
	ASSERT(moves[2] == Move(PieceType::Pawn, c2, b3));
	ASSERT(moves[3] == Move(PieceType::Pawn, c2, d3));

	position = Position("5k2/2p5/1N1N4/8/8/8/2P5/5K2 w - - 0 1");
	pawn = Piece(PieceType::Pawn, c7);
	moves = MoveSearcher::GetLegalMoves(position, pawn, false);
	ASSERT(moves.size() == 4);
	ASSERT(moves[0] == Move(PieceType::Pawn, c7, c6));
	ASSERT(moves[1] == Move(PieceType::Pawn, c7, c5));
	ASSERT(moves[2] == Move(PieceType::Pawn, c7, b6));
	ASSERT(moves[3] == Move(PieceType::Pawn, c7, d6));

	position = Position("5k2/2p5/1N1N4/8/8/8/2P5/5K2 w - - 0 1");
	pawn = Piece(PieceType::Pawn, c7);
	moves = MoveSearcher::GetLegalMoves(position, pawn, false);
	ASSERT(moves.size() == 4);
	ASSERT(moves[0] == Move(PieceType::Pawn, c7, c6));
	ASSERT(moves[1] == Move(PieceType::Pawn, c7, c5));
	ASSERT(moves[2] == Move(PieceType::Pawn, c7, b6));
	ASSERT(moves[3] == Move(PieceType::Pawn, c7, d6));

	position = Position("4k3/8/8/2pP4/8/8/8/4K3 w - c6 0 1");
	pawn = Piece(PieceType::Pawn, d5);
	moves = MoveSearcher::GetLegalMoves(position, pawn, true);
	ASSERT(moves.size() == 2);
	ASSERT(moves[0] == Move(PieceType::Pawn, d5, d6));
	ASSERT(moves[1] == Move(PieceType::Pawn, d5, c6));

	position = Position("4k3/8/8/3Pp3/8/8/8/4K3 w - e6 0 1");
	pawn = Piece(PieceType::Pawn, d5);
	moves = MoveSearcher::GetLegalMoves(position, pawn, true);
	ASSERT(moves.size() == 2);
	ASSERT(moves[0] == Move(PieceType::Pawn, d5, d6));
	ASSERT(moves[1] == Move(PieceType::Pawn, d5, e6));

	position = Position("4k3/8/8/8/2Pp4/8/8/4K3 b - c3 0 1");
	pawn = Piece(PieceType::Pawn, d4);
	moves = MoveSearcher::GetLegalMoves(position, pawn, false);
	ASSERT(moves.size() == 2);
	ASSERT(moves[0] == Move(PieceType::Pawn, d4, d3));
	ASSERT(moves[1] == Move(PieceType::Pawn, d4, c3));

	position = Position("4k3/8/8/8/3pP3/8/8/4K3 b - e3 0 1");
	pawn = Piece(PieceType::Pawn, d4);
	moves = MoveSearcher::GetLegalMoves(position, pawn, false);
	ASSERT(moves.size() == 2);
	ASSERT(moves[0] == Move(PieceType::Pawn, d4, d3));
	ASSERT(moves[1] == Move(PieceType::Pawn, d4, e3));

	position = Position("4k3/1P6/8/3p4/8/8/8/4K3 w - d6 0 1");
	pawn = Piece(PieceType::Pawn, b7);
	moves = MoveSearcher::GetLegalMoves(position, pawn, true);
	ASSERT(moves.size() == 4);
	ASSERT(moves[0] == Move(PieceType::Pawn, PieceType::Queen, b7, b8));
	ASSERT(moves[1] == Move(PieceType::Pawn, PieceType::Rook, b7, b8));
	ASSERT(moves[2] == Move(PieceType::Pawn, PieceType::Bishop, b7, b8));
	ASSERT(moves[3] == Move(PieceType::Pawn, PieceType::Knight, b7, b8));

	position = Position("nnn1k3/1P6/8/3p4/8/8/8/4K3 w - d6 0 1");
	pawn = Piece(PieceType::Pawn, b7);
	moves = MoveSearcher::GetLegalMoves(position, pawn, true);
	ASSERT(moves.size() == 8);
	ASSERT(moves[0] == Move(PieceType::Pawn, PieceType::Queen, b7, a8));
	ASSERT(moves[1] == Move(PieceType::Pawn, PieceType::Rook, b7, a8));
	ASSERT(moves[2] == Move(PieceType::Pawn, PieceType::Bishop, b7, a8));
	ASSERT(moves[3] == Move(PieceType::Pawn, PieceType::Knight, b7, a8));
	ASSERT(moves[4] == Move(PieceType::Pawn, PieceType::Queen, b7, c8));
	ASSERT(moves[5] == Move(PieceType::Pawn, PieceType::Rook, b7, c8));
	ASSERT(moves[6] == Move(PieceType::Pawn, PieceType::Bishop, b7, c8));
	ASSERT(moves[7] == Move(PieceType::Pawn, PieceType::Knight, b7, c8));

	position = Position("nnn1k3/1P6/8/3p4/8/8/1p6/N1N1K3 b - - 0 1");
	pawn = Piece(PieceType::Pawn, b2);
	moves = MoveSearcher::GetLegalMoves(position, pawn, false);
	ASSERT(moves.size() == 12);
	ASSERT(moves[0] == Move(PieceType::Pawn, PieceType::Queen, b2, b1));
	ASSERT(moves[4] == Move(PieceType::Pawn, PieceType::Queen, b2, a1));
	ASSERT(moves[8] == Move(PieceType::Pawn, PieceType::Queen, b2, c1));
}

void MoveSearcherTests::Run()
{
	TestPawnMoves();
	TestRookMoves();
	TestBishopMoves();
	TestIllegalCastles();

	//simple stalemate
	staleMateWhiteToPlay.SetMaintainPiecesList(false);
	size_t positionCount = MoveSearcher::CountNodes(staleMateWhiteToPlay, 1);
	ASSERT(positionCount == 0);

	staleMateBlackToPlay.SetMaintainPiecesList(false);
	positionCount = MoveSearcher::CountNodes(staleMateBlackToPlay, 1);
	ASSERT(positionCount == 0);

	//starting position
	Position startingPosition;
	startingPosition.SetMaintainPiecesList(false);
	size_t positionsCount = MoveSearcher::CountNodes(startingPosition, 1);
	std::unordered_set<uint64_t>  uniqueCount = MoveSearcher::GetUniqueNodes(startingPosition, 1);
	ASSERT(positionsCount == 20);
	ASSERT(uniqueCount.size() == 20);

	uniqueCount = MoveSearcher::GetUniqueNodes(startingPosition, 2);
	positionsCount = MoveSearcher::CountNodes(startingPosition, 2);
	ASSERT(positionsCount == 20 * 20);
	ASSERT(uniqueCount.size() == 20 * 20);

	uniqueCount = MoveSearcher::GetUniqueNodes(startingPosition, 3);
	positionsCount = MoveSearcher::CountNodes(startingPosition, 3);
	ASSERT(uniqueCount.size() == 7602); //5362 if we dont count en passant
	ASSERT(positionsCount == 8902);

	uniqueCount = MoveSearcher::GetUniqueNodes(startingPosition, 4);
	positionsCount = MoveSearcher::CountNodes(startingPosition, 4);
	ASSERT(uniqueCount.size() == 101240); //72084 if we dont count en passant
	ASSERT(positionsCount == 197281);

	positionsCount = MoveSearcher::CountNodes(startingPosition, 5);
	ASSERT(positionsCount == 4865609);

	positionsCount = MoveSearcher::CountNodes(startingPosition, 6);
	ASSERT(positionsCount == 119060324);

	//Perft #2
	perftPosition2.SetMaintainPiecesList(false);
	positionsCount = MoveSearcher::CountNodes(perftPosition2, 1);
	ASSERT(positionsCount == 48);

	positionsCount = MoveSearcher::CountNodes(perftPosition2, 2);
	ASSERT(positionsCount == 2039);

	positionsCount = MoveSearcher::CountNodes(perftPosition2, 3);
	ASSERT(positionsCount == 97862);

	positionsCount = MoveSearcher::CountNodes(perftPosition2, 4);
	ASSERT(positionsCount == 4085603);

	positionsCount = MoveSearcher::CountNodes(perftPosition2, 5);
	ASSERT(positionsCount == 193690690);

	//Perft #3
	perftPosition3.SetMaintainPiecesList(false);
	positionsCount = MoveSearcher::CountNodes(perftPosition3, 1);
	ASSERT(positionsCount == 14);

	positionsCount = MoveSearcher::CountNodes(perftPosition3, 2);
	ASSERT(positionsCount == 191);

	positionsCount = MoveSearcher::CountNodes(perftPosition3, 3);
	ASSERT(positionsCount == 2812);

	positionsCount = MoveSearcher::CountNodes(perftPosition3, 4);
	ASSERT(positionsCount == 43238);

	positionsCount = MoveSearcher::CountNodes(perftPosition3, 5);
	ASSERT(positionsCount == 674624);

	positionsCount = MoveSearcher::CountNodes(perftPosition3, 6);
	ASSERT(positionsCount == 11030083);

	positionsCount = MoveSearcher::CountNodes(perftPosition3, 7);
	ASSERT(positionsCount == 178633661);

	//Perft #4
	perftPosition4.SetMaintainPiecesList(false);
	positionsCount = MoveSearcher::CountNodes(perftPosition4, 1);
	ASSERT(positionsCount == 6);

	positionsCount = MoveSearcher::CountNodes(perftPosition4, 2);
	ASSERT(positionsCount == 264);

	positionsCount = MoveSearcher::CountNodes(perftPosition4, 3);
	ASSERT(positionsCount == 9467);

	positionsCount = MoveSearcher::CountNodes(perftPosition4, 4);
	ASSERT(positionsCount == 422333);

	positionsCount = MoveSearcher::CountNodes(perftPosition4, 5);
	ASSERT(positionsCount == 15833292);

	//Perft #4 bis
	perftPosition4bis.SetMaintainPiecesList(false);
	positionsCount = MoveSearcher::CountNodes(perftPosition4bis, 1);
	ASSERT(positionsCount == 6);

	positionsCount = MoveSearcher::CountNodes(perftPosition4bis, 2);
	ASSERT(positionsCount == 264);

	positionsCount = MoveSearcher::CountNodes(perftPosition4bis, 3);
	ASSERT(positionsCount == 9467);

	positionsCount = MoveSearcher::CountNodes(perftPosition4bis, 4);
	ASSERT(positionsCount == 422333);

	positionsCount = MoveSearcher::CountNodes(perftPosition4bis, 5);
	ASSERT(positionsCount == 15833292);

	//Perft #5
	perftPosition5.SetMaintainPiecesList(false);
	positionsCount = MoveSearcher::CountNodes(perftPosition5, 1);
	ASSERT(positionsCount == 44);

	positionsCount = MoveSearcher::CountNodes(perftPosition5, 2);
	ASSERT(positionsCount == 1486);

	positionsCount = MoveSearcher::CountNodes(perftPosition5, 3);
	ASSERT(positionsCount == 62379);

	positionsCount = MoveSearcher::CountNodes(perftPosition5, 4);
	ASSERT(positionsCount == 2103487);

	positionsCount = MoveSearcher::CountNodes(perftPosition5, 5);
	ASSERT(positionsCount == 89941194);

	//Perft #6
	perftPosition6.SetMaintainPiecesList(false);
	positionsCount = MoveSearcher::CountNodes(perftPosition6, 1);
	ASSERT(positionsCount == 46);

	positionsCount = MoveSearcher::CountNodes(perftPosition6, 2);
	ASSERT(positionsCount == 2079);

	positionsCount = MoveSearcher::CountNodes(perftPosition6, 3);
	ASSERT(positionsCount == 89890);

	positionsCount = MoveSearcher::CountNodes(perftPosition6, 4);
	ASSERT(positionsCount == 3894594);

	positionsCount = MoveSearcher::CountNodes(perftPosition6, 5);
	ASSERT(positionsCount == 164075551);
}