#include "NotationParserTests.h"
#include "Position.h"
#include <assert.h>

Position startingPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
Position startingPositionEnPassant("rnbqkbnr / pppppppp / 8 / 8 / 4P3 / 8 / PPPP1PPP / RNBQKBNR b KQkq e3 0 1");
Position position1("r1b3k1 / p1b1Bppp / 5n2 / 2n1p1N1 / 2B5 / 3P4 / 2Q2PPP / qN2R1K1 b - -5 19");

void NotationParserTests::Run()
{
	assert(startingPosition.GetWhitePieces().size() == 16);
	assert(startingPosition.GetBlackPieces().size() == 16);
	assert(startingPosition.CanWhiteCastleKingSide());
	assert(startingPosition.CanBlackCastleKingSide());
	assert(startingPosition.CanWhiteCastleQueenSide());
	assert(startingPosition.CanBlackCastleQueenSide());
	assert(!startingPosition.GetEnPassantSquare().has_value());

	assert(startingPositionEnPassant.GetWhitePieces().size() == 16);
	assert(startingPositionEnPassant.GetBlackPieces().size() == 16);
	assert(startingPositionEnPassant.CanWhiteCastleKingSide());
	assert(startingPositionEnPassant.CanBlackCastleKingSide());
	assert(startingPositionEnPassant.CanWhiteCastleQueenSide());
	assert(startingPositionEnPassant.CanBlackCastleQueenSide());
	assert(startingPositionEnPassant.GetEnPassantSquare().has_value());
	std::array<int, 2> square{4, 2};
	assert(*startingPositionEnPassant.GetEnPassantSquare() == square);

	assert(position1.GetWhitePieces().size() == 11);
	assert(position1.GetBlackPieces().size() == 12);
	assert(!position1.CanWhiteCastleKingSide());
	assert(!position1.CanBlackCastleKingSide());
	assert(!position1.CanWhiteCastleQueenSide());
	assert(!position1.CanBlackCastleQueenSide());
	assert(!position1.GetEnPassantSquare().has_value());
	square = {0, 0};
	assert(position1.GetPiecesToPlay(PieceType::Queen).front()->m_Position == square);
}