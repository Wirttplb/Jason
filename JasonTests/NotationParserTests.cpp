#include "NotationParserTests.h"
#include "Position.h"
#include "TestsUtility.h"

void NotationParserTests::Run()
{
	Position startingPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	Position startingPositionEnPassant("rnbqkbnr / pppppppp / 8 / 8 / 4P3 / 8 / PPPP1PPP / RNBQKBNR b KQkq e3 0 1");
	Position position1("r1b3k1 / p1b1Bppp / 5n2 / 2n1p1N1 / 2B5 / 3P4 / 2Q2PPP / qN2R1K1 b - -5 19");
	Position perftPosition4("r3k2r / Pppp1ppp / 1b3nbN / nP6 / BBP1P3 / q4N2 / Pp1P2PP / R2Q1RK1 w kq - 0 1");
	Position perftPosition4bis("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1");
	Position enPassant1("4k3/8/8/2pP4/8/8/8/4K3 w - c6 0 1");

	ASSERT(startingPosition.GetWhitePiecesList().size() == 16);
	ASSERT(startingPosition.GetBlackPiecesList().size() == 16);
	ASSERT(startingPosition.CanWhiteCastleKingSide());
	ASSERT(startingPosition.CanBlackCastleKingSide());
	ASSERT(startingPosition.CanWhiteCastleQueenSide());
	ASSERT(startingPosition.CanBlackCastleQueenSide());
	ASSERT(!startingPosition.GetEnPassantSquare().has_value());

	ASSERT(startingPositionEnPassant.GetWhitePiecesList().size() == 16);
	ASSERT(startingPositionEnPassant.GetBlackPiecesList().size() == 16);
	ASSERT(startingPositionEnPassant.CanWhiteCastleKingSide());
	ASSERT(startingPositionEnPassant.CanBlackCastleKingSide());
	ASSERT(startingPositionEnPassant.CanWhiteCastleQueenSide());
	ASSERT(startingPositionEnPassant.CanBlackCastleQueenSide());
	ASSERT(startingPositionEnPassant.GetEnPassantSquare().has_value());
	ASSERT(*startingPositionEnPassant.GetEnPassantSquare() == e3);

	ASSERT(position1.GetWhitePiecesList().size() == 11);
	ASSERT(position1.GetBlackPiecesList().size() == 12);
	ASSERT(!position1.CanWhiteCastleKingSide());
	ASSERT(!position1.CanBlackCastleKingSide());
	ASSERT(!position1.CanWhiteCastleQueenSide());
	ASSERT(!position1.CanBlackCastleQueenSide());
	ASSERT(!position1.GetEnPassantSquare().has_value());
	ASSERT(position1.GetPiecesToPlay(PieceType::Queen).front().m_Square == a1);

	//had a bug with castling moves parsing
	Position positionA = perftPosition4;
	Move move(PieceType::King, g1, h1);
	positionA.Update(move);

	Position positionB = perftPosition4bis;
	move = Move(PieceType::King, g8, h8);
	positionB.Update(move);
	ASSERT(!positionA.IsWhiteToPlay());
	ASSERT(positionA.CanBlackCastleKingSide() && positionA.CanBlackCastleQueenSide());
	ASSERT(!positionA.CanWhiteCastleKingSide() && !positionA.CanWhiteCastleQueenSide());
	ASSERT(positionB.IsWhiteToPlay());
	ASSERT(positionB.CanWhiteCastleKingSide() && positionB.CanWhiteCastleQueenSide());
	ASSERT(!positionB.CanBlackCastleKingSide() && !positionB.CanBlackCastleQueenSide());

	ASSERT(enPassant1.GetEnPassantSquare() == c6);
}