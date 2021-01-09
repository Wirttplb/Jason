#include "NotationParserTests.h"
#include "Position.h"
#include <assert.h>

Position startingPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

void NotationParserTests::Run()
{
	assert(startingPosition.GetWhitePieces().size() == 16);
	assert(startingPosition.GetBlackPieces().size() == 16);
	assert(startingPosition.CanWhiteCastleKingSide());
	assert(startingPosition.CanBlackCastleKingSide());
	assert(startingPosition.CanWhiteCastleQueenSide());
	assert(startingPosition.CanBlackCastleQueenSide());
	assert(!startingPosition.GetEnPassantSquare().has_value());
}