#pragma once
#include <stdint.h>
#include <array>
#include "BasicDefinitions.h"

class Position;

/// <summary>
/// Utility class for initializing, handling and updating a Zobrist hash of a chess position
/// </summary>
class ZobristHash
{
public:
	static uint64_t Init();

	static constexpr uint64_t GetKey(PieceType type, Square square, bool isWhite);
	static constexpr uint64_t GetWhiteQueenSideCastleKey();
	static constexpr uint64_t GetWhiteKingSideCastleKey();
	static constexpr uint64_t GetBlackQueenSideCastleKey();
	static constexpr uint64_t GetBlackKingSideCastleKey();
	static constexpr uint64_t GetBlackToMoveKey();

	/// <summary> Return en passant key for zobrist table /// </summary>
	/// <param name="square">en passant square idx</param>
	constexpr static uint64_t GetEnPassantKey(Square square);

private:
	static const std::array<uint64_t, 781> Table;

	static constexpr int NumberOfPieceTypes = 6;
	static constexpr int NumberOfPieceKeys = 12;
	static constexpr int BlackToMoveIdx = 64 * 12 + 0;
	static constexpr int WhiteKingSideCastleIdx = 64 * 12 + 1;
	static constexpr int WhiteQueenSideCastleIdx = 64 * 12 + 2;
	static constexpr int BlackKingSideCastleIdx = 64 * 12 + 3;
	static constexpr int BlackQueenSideCastleIdx = 64 * 12 + 4;
	static constexpr int EnPassantAIdx = 64 * 12 + 5; //A to H = 64 * 12 + 5 to 64 * 12 + 12
	static constexpr int EnPassantHIdx = 64 * 12 + 12;
	static constexpr int NumberOfKeys = 781;
	static_assert(EnPassantHIdx == (NumberOfKeys - 1));
};

constexpr uint64_t ZobristHash::GetBlackToMoveKey()
{
	return ZobristHash::Table[BlackToMoveIdx];
}

constexpr uint64_t ZobristHash::GetWhiteQueenSideCastleKey()
{
	return ZobristHash::Table[WhiteQueenSideCastleIdx];
}

constexpr uint64_t ZobristHash::GetWhiteKingSideCastleKey()
{
	return ZobristHash::Table[WhiteKingSideCastleIdx];
}

constexpr uint64_t ZobristHash::GetBlackQueenSideCastleKey()
{
	return ZobristHash::Table[BlackQueenSideCastleIdx];
}

constexpr uint64_t ZobristHash::GetBlackKingSideCastleKey()
{
	return ZobristHash::Table[BlackKingSideCastleIdx];
}

constexpr uint64_t ZobristHash::GetEnPassantKey(Square square)
{
	return ZobristHash::Table[EnPassantAIdx + square % 8];
}

constexpr uint64_t ZobristHash::GetKey(PieceType type, Square square, bool isWhite)
{
	const int idx = square * NumberOfPieceKeys + static_cast<int>(type) + (isWhite ? 0 : NumberOfPieceTypes);
	return Table[idx];
}