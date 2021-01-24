#pragma once
#include <stdint.h>

class Piece;
class Position;
enum Square;

/// <summary>
/// Utility class for initializing, handling and updating a Zobrist hash of a chess position
/// </summary>
class ZobristHash
{
public:
	static uint64_t Init();

	static uint64_t GetKey(const Piece& piece, bool isWhite);
	static uint64_t GetWhiteQueenSideCastleKey();
	static uint64_t GetWhiteKingSideCastleKey();
	static uint64_t GetBlackQueenSideCastleKey();
	static uint64_t GetBlackKingSideCastleKey();
	static uint64_t GetBlackToMoveKey();

	/// <summary> Return en passant key for zobrist table /// </summary>
	/// <param name="square">en passant square idx</param>
	static uint64_t GetEnPassantKey(Square square);
};