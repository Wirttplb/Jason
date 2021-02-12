#include "MoveSearcherTests.h"
#include "Position.h"
#include "MoveSearcher.h"
#include "TestsUtility.h"
#include <unordered_set>

static MoveList<MaxMoves> moves;

static void TestRookMoves()
{
	moves.clear();
	Position position("4k3/8/8/8/8/8/6K1/R7 w - - 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, PieceType::Rook, a1, true, moves);
	ASSERT(moves.size() == 14);

	moves.clear();
	MoveSearcher::GetLegalMovesFromBitboards(position, PieceType::Rook, a1, true, moves);
	ASSERT(moves.size() == 14);

	moves.clear();
	position = Position("4k3/8/8/N7/8/8/4K3/R2N4 w - - 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, PieceType::Rook, a1, true, moves);
	ASSERT(moves.size() == 5);

	moves.clear();
	position = Position("4k3/8/8/n7/8/8/4K3/R2N4 w - - 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, PieceType::Rook, a1, true, moves);
	ASSERT(moves.size() == 6);

	moves.clear();
	position = Position("4k1n1/8/8/N5Rn/8/8/4K3/8 w - - 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, PieceType::Rook, g5, true, moves);
	ASSERT(moves.size() == 13);

	moves.clear();
	position = Position("8/8/8/8/8/8/8/nrNnnnnn b - - 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, PieceType::Rook, b1, false, moves);
	ASSERT(moves.size() == 8);
}

static void TestBishopMoves()
{
	moves.clear();
	Position position("4k3/8/8/8/8/8/4K3/B7 w - - 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, PieceType::Bishop, a1, true, moves);
	ASSERT(moves.size() == 7);

	moves.clear();
	position = Position("4k3/8/8/8/8/8/1B2K3/8 w - - 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, PieceType::Bishop, b2, true, moves);
	ASSERT(moves.size() == 9);

	moves.clear();
	position = Position("4k3/8/8/8/3n4/N7/1B3K2/8 w - - 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, PieceType::Bishop, b2, true, moves);
	ASSERT(moves.size() == 4);

	moves.clear();
	position = Position("1n2k3/pp4pp/3B4/4N3/3n4/N5N1/5K2/8 w - - 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, PieceType::Bishop, d6, true, moves);
	ASSERT(moves.size() == 6);

	moves.clear();
	position = Position("1n1nk3/pp2n1pp/5nN1/6B1/8/N3N3/3nNK2/2n5 w - - 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, PieceType::Bishop, g5, true, moves);
	ASSERT(moves.size() == 4);

	moves.clear();
	position = Position("7B/8/8/8/8/8/8/8 w - - 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, PieceType::Bishop, h8, true, moves);
	ASSERT(moves.size() == 7);

	moves.clear();
	position = Position("B7/8/8/8/8/8/8/8 w - - 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, PieceType::Bishop, a8, true, moves);
	ASSERT(moves.size() == 7);

	moves.clear();
	position = Position("8/8/8/8/8/8/8/7B w - - 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, PieceType::Bishop, h1, true, moves);
	ASSERT(moves.size() == 7);

	moves.clear();
	position = Position("B7/7B/8/8/4B3/8/8/1B5B w - - 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, PieceType::Bishop, e4, true, moves);
	ASSERT(moves.size() == 9);

	//there was a bug on this position
	moves.clear();
	position = Position("r3k2r/pbppqpb1/1nN1pnp1/3P4/1p2P3/5Q1p/PPPBBPPP/RN2K2R b KQq - 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, PieceType::Bishop, b7, false, moves);
	ASSERT(moves.size() == 3);
}

static void TestIllegalCastles()
{
	//legal castles
	Position position("4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, moves);
	int castles = 0;
	for (const Move& m : moves)
	{
		if (m.IsCastling())
			castles++;
	}

	ASSERT(castles == 2);

	//pawn controls square
	position = Position("4k3/8/8/8/8/8/2p3p1/R3K2R w KQ - 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, moves);
	castles = 0;
	for (const Move& m : moves)
	{
		if (m.IsCastling())
			castles++;
	}

	ASSERT(castles == 0);

	//king in check
	position = Position("4k3/4r3/8/8/8/8/8/R3K2R w KQ - 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, moves);
	castles = 0;
	for (const Move& m : moves)
	{
		if (m.IsCastling())
			castles++;
	}

	ASSERT(castles == 0);

	//enemy piece blocks
	position = Position("4k3/8/8/8/8/8/8/R1n1K1nR w KQ - 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, moves);
	castles = 0;
	for (const Move& m : moves)
	{
		if (m.IsCastling())
			castles++;
	}

	ASSERT(castles == 0);

	//enemy piece blocks #2
	position = Position("4k3/8/8/8/8/8/8/R2nKn1R w KQ - 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, moves);
	castles = 0;
	for (const Move& m : moves)
	{
		if (m.IsCastling())
			castles++;
	}

	ASSERT(castles == 0);

	//enemy piece blocks #3
	position = Position("4k3/8/8/8/8/6n1/8/Rn2K2R w KQ - 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, moves);
	castles = 0;
	for (const Move& m : moves)
	{
		if (m.IsCastling())
			castles++;
	}

	ASSERT(castles == 0);

	//friend piece blocks
	position = Position("4k3/8/8/8/8/8/8/R1N1KN1R w KQ - 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, moves);
	castles = 0;
	for (const Move& m : moves)
	{
		if (m.IsCastling())
			castles++;
	}

	ASSERT(castles == 0);
}

static void TestPawnMoves()
{
	moves.clear();
	Position position("4k3/2p5/8/8/8/8/2P5/4K3 w - - 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, PieceType::Pawn, c2, true, moves);
	ASSERT(moves.size() == 2);
	ASSERT(moves.front() == Move(PieceType::Pawn, c2, c3));
	ASSERT(moves.back() == Move(PieceType::Pawn, c2, c4));

	moves.clear();
	MoveSearcher::GetLegalMovesFromBitboards(position, PieceType::Pawn, c7, false, moves);
	ASSERT(moves.size() == 2);
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, c7, c5)) != moves.end());
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, c7, c6)) != moves.end());

	moves.clear();
	position = Position("4k3/2p5/8/8/8/1n1n4/2P5/5K2 w - - 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, PieceType::Pawn, c2, true, moves);
	ASSERT(moves.size() == 4);
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, c2, c3)) != moves.end());
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, c2, c4)) != moves.end());
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, c2, b3)) != moves.end());
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, c2, d3)) != moves.end());

	moves.clear();
	position = Position("5k2/2p5/1N1N4/8/8/8/2P5/5K2 w - - 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, PieceType::Pawn, c7, false, moves);
	ASSERT(moves.size() == 4);
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, c7, c6)) != moves.end());
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, c7, c5)) != moves.end());
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, c7, b6)) != moves.end());
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, c7, d6)) != moves.end());

	moves.clear();
	position = Position("4k3/8/8/2pP4/8/8/8/4K3 w - c6 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, PieceType::Pawn, d5, true, moves);
	ASSERT(moves.size() == 2);
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, d5, d6)) != moves.end());
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, d5, c6)) != moves.end());

	moves.clear();
	position = Position("4k3/8/8/3Pp3/8/8/8/4K3 w - e6 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, PieceType::Pawn, d5, true, moves);
	ASSERT(moves.size() == 2);
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, d5, d6)) != moves.end());
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, d5, e6)) != moves.end());

	moves.clear();
	position = Position("4k3/8/8/8/2Pp4/8/8/4K3 b - c3 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, PieceType::Pawn, d4, false, moves);
	ASSERT(moves.size() == 2);
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, d4, d3)) != moves.end());
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, d4, c3)) != moves.end());

	moves.clear();
	position = Position("4k3/8/8/8/3pP3/8/8/4K3 b - e3 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, PieceType::Pawn, d4, false, moves);
	ASSERT(moves.size() == 2);
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, d4, d3)) != moves.end());
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, d4, e3)) != moves.end());

	moves.clear();
	position = Position("4k3/1P6/8/3p4/8/8/8/4K3 w - d6 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, PieceType::Pawn, b7, true, moves);
	ASSERT(moves.size() == 4);
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, PieceType::Queen, b7, b8)) != moves.end());
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, PieceType::Rook, b7, b8)) != moves.end());
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, PieceType::Bishop, b7, b8)) != moves.end());
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, PieceType::Knight, b7, b8)) != moves.end());

	moves.clear();
	position = Position("nnn1k3/1P6/8/3p4/8/8/8/4K3 w - d6 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, PieceType::Pawn, b7, true, moves);
	ASSERT(moves.size() == 8);
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, PieceType::Queen, b7, a8)) != moves.end());
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, PieceType::Queen, b7, c8)) != moves.end());
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, PieceType::Rook, b7, a8)) != moves.end());
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, PieceType::Bishop, b7, a8)) != moves.end());
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, PieceType::Knight, b7, a8)) != moves.end());
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, PieceType::Rook, b7, c8)) != moves.end());
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, PieceType::Bishop, b7, c8)) != moves.end());
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, PieceType::Knight, b7, c8)) != moves.end());

	moves.clear();
	position = Position("nnn1k3/1P6/8/3p4/8/8/1p6/N1N1K3 b - - 0 1");
	MoveSearcher::GetLegalMovesFromBitboards(position, PieceType::Pawn, b2, false, moves);
	ASSERT(moves.size() == 12);
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, PieceType::Queen, b2, b1)) != moves.end());
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, PieceType::Queen, b2, a1)) != moves.end());
	ASSERT(std::find(moves.begin(), moves.end(), Move(PieceType::Pawn, PieceType::Queen, b2, c1)) != moves.end());
}

static std::array<MoveList<MaxMoves>, PerftMaxDepth> perftMoveLists;
void MoveSearcherTests::Run()
{
	time_t start;
	time(&start);

	TestPawnMoves();
	TestRookMoves();
	TestBishopMoves();
	TestIllegalCastles();

	Position staleMateWhiteToPlay("8/8/8/8/8/kq6/8/K7 w - - 0 1");
	Position staleMateBlackToPlay("8/8/8/8/8/KQ6/8/k7 b - - 0 1");
	Position perftPosition2("r3k2r / p1ppqpb1 / bn2pnp1 / 3PN3 / 1p2P3 / 2N2Q1p / PPPBBPPP / R3K2R w KQkq -");
	Position perftPosition3("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -");
	Position perftPosition4("r3k2r / Pppp1ppp / 1b3nbN / nP6 / BBP1P3 / q4N2 / Pp1P2PP / R2Q1RK1 w kq - 0 1");
	Position perftPosition4bis("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1");
	Position perftPosition5("rnbq1k1r / pp1Pbppp / 2p5 / 8 / 2B5 / 8 / PPP1NnPP / RNBQK2R w KQ - 1 8");
	Position perftPosition6("r4rk1 / 1pp1qppp / p1np1n2 / 2b1p1B1 / 2B1P1b1 / P1NP1N2 / 1PP1QPPP / R4RK1 w - -0 10");

	//simple stalemate
	size_t positionCount = MoveSearcher::Perft(staleMateWhiteToPlay, 1, perftMoveLists);
	ASSERT(positionCount == 0);

	positionCount = MoveSearcher::Perft(staleMateBlackToPlay, 1, perftMoveLists);
	ASSERT(positionCount == 0);

	//starting position
	Position startingPosition;
	size_t positionsCount = MoveSearcher::Perft(startingPosition, 1, perftMoveLists);
	std::unordered_set<uint64_t>  uniqueCount = MoveSearcher::UniquePerft(startingPosition, 1, perftMoveLists);
	ASSERT(positionsCount == 20);
	ASSERT(uniqueCount.size() == 20);

	uniqueCount = MoveSearcher::UniquePerft(startingPosition, 2, perftMoveLists);
	positionsCount = MoveSearcher::Perft(startingPosition, 2, perftMoveLists);
	ASSERT(positionsCount == 20 * 20);
	ASSERT(uniqueCount.size() == 20 * 20);

	uniqueCount = MoveSearcher::UniquePerft(startingPosition, 3, perftMoveLists);
	positionsCount = MoveSearcher::Perft(startingPosition, 3, perftMoveLists);
	ASSERT(uniqueCount.size() == 7602); //5362 if we dont count en passant
	ASSERT(positionsCount == 8902);

	uniqueCount = MoveSearcher::UniquePerft(startingPosition, 4, perftMoveLists);
	positionsCount = MoveSearcher::Perft(startingPosition, 4, perftMoveLists);
	ASSERT(uniqueCount.size() == 101240); //72084 if we dont count en passant
	ASSERT(positionsCount == 197281);

	positionsCount = MoveSearcher::Perft(startingPosition, 5, perftMoveLists);
	ASSERT(positionsCount == 4865609);

	positionsCount = MoveSearcher::Perft(startingPosition, 6, perftMoveLists);
	ASSERT(positionsCount == 119060324);

	//Perft #2
	positionsCount = MoveSearcher::Perft(perftPosition2, 1, perftMoveLists);
	ASSERT(positionsCount == 48);

	positionsCount = MoveSearcher::Perft(perftPosition2, 2, perftMoveLists);
	ASSERT(positionsCount == 2039);

	positionsCount = MoveSearcher::Perft(perftPosition2, 3, perftMoveLists);
	ASSERT(positionsCount == 97862);

	positionsCount = MoveSearcher::Perft(perftPosition2, 4, perftMoveLists);
	ASSERT(positionsCount == 4085603);

	positionsCount = MoveSearcher::Perft(perftPosition2, 5, perftMoveLists);
	ASSERT(positionsCount == 193690690);

	//Perft #3
	positionsCount = MoveSearcher::Perft(perftPosition3, 1, perftMoveLists);
	ASSERT(positionsCount == 14);

	positionsCount = MoveSearcher::Perft(perftPosition3, 2, perftMoveLists);
	ASSERT(positionsCount == 191);

	positionsCount = MoveSearcher::Perft(perftPosition3, 3, perftMoveLists);
	ASSERT(positionsCount == 2812);

	positionsCount = MoveSearcher::Perft(perftPosition3, 4, perftMoveLists);
	ASSERT(positionsCount == 43238);

	positionsCount = MoveSearcher::Perft(perftPosition3, 5, perftMoveLists);
	ASSERT(positionsCount == 674624);

	positionsCount = MoveSearcher::Perft(perftPosition3, 6, perftMoveLists);
	ASSERT(positionsCount == 11030083);

	positionsCount = MoveSearcher::Perft(perftPosition3, 7, perftMoveLists);
	ASSERT(positionsCount == 178633661);

	//Perft #4
	positionsCount = MoveSearcher::Perft(perftPosition4, 1, perftMoveLists);
	ASSERT(positionsCount == 6);

	positionsCount = MoveSearcher::Perft(perftPosition4, 2, perftMoveLists);
	ASSERT(positionsCount == 264);

	positionsCount = MoveSearcher::Perft(perftPosition4, 3, perftMoveLists);
	ASSERT(positionsCount == 9467);

	positionsCount = MoveSearcher::Perft(perftPosition4, 4, perftMoveLists);
	ASSERT(positionsCount == 422333);

	positionsCount = MoveSearcher::Perft(perftPosition4, 5, perftMoveLists);
	ASSERT(positionsCount == 15833292);

	//Perft #4 bis
	positionsCount = MoveSearcher::Perft(perftPosition4bis, 1, perftMoveLists);
	ASSERT(positionsCount == 6);

	positionsCount = MoveSearcher::Perft(perftPosition4bis, 2, perftMoveLists);
	ASSERT(positionsCount == 264);

	positionsCount = MoveSearcher::Perft(perftPosition4bis, 3, perftMoveLists);
	ASSERT(positionsCount == 9467);

	positionsCount = MoveSearcher::Perft(perftPosition4bis, 4, perftMoveLists);
	ASSERT(positionsCount == 422333);

	positionsCount = MoveSearcher::Perft(perftPosition4bis, 5, perftMoveLists);
	ASSERT(positionsCount == 15833292);

	//Perft #5
	positionsCount = MoveSearcher::Perft(perftPosition5, 1, perftMoveLists);
	ASSERT(positionsCount == 44);

	positionsCount = MoveSearcher::Perft(perftPosition5, 2, perftMoveLists);
	ASSERT(positionsCount == 1486);

	positionsCount = MoveSearcher::Perft(perftPosition5, 3, perftMoveLists);
	ASSERT(positionsCount == 62379);

	positionsCount = MoveSearcher::Perft(perftPosition5, 4, perftMoveLists);
	ASSERT(positionsCount == 2103487);

	positionsCount = MoveSearcher::Perft(perftPosition5, 5, perftMoveLists);
	ASSERT(positionsCount == 89941194);

	//Perft #6
	positionsCount = MoveSearcher::Perft(perftPosition6, 1, perftMoveLists);
	ASSERT(positionsCount == 46);

	positionsCount = MoveSearcher::Perft(perftPosition6, 2, perftMoveLists);
	ASSERT(positionsCount == 2079);

	positionsCount = MoveSearcher::Perft(perftPosition6, 3, perftMoveLists);
	ASSERT(positionsCount == 89890);

	positionsCount = MoveSearcher::Perft(perftPosition6, 4, perftMoveLists);
	ASSERT(positionsCount == 3894594);

	positionsCount = MoveSearcher::Perft(perftPosition6, 5, perftMoveLists);
	ASSERT(positionsCount == 164075551);

	time_t end;
	time(&end);
	PrintTestDuration(start, end, "MoveSearcherTests: %.2lf seconds.");
	ASSERT(difftime(end, start) < 110.0);
}