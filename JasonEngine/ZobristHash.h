#pragma once
#include <stdint.h>
#include <array>
#include "BasicDefinitions.h"

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

	constexpr static uint64_t GetKey(const Piece& piece, bool isWhite);
	constexpr static uint64_t GetWhiteQueenSideCastleKey();
	constexpr static uint64_t GetWhiteKingSideCastleKey();
	constexpr static uint64_t GetBlackQueenSideCastleKey();
	constexpr static uint64_t GetBlackKingSideCastleKey();
	constexpr static uint64_t GetBlackToMoveKey();

	/// <summary> Return en passant key for zobrist table /// </summary>
	/// <param name="square">en passant square idx</param>
	constexpr static uint64_t GetEnPassantKey(Square square);

private:
	static const std::array<uint64_t, 781> Table;

	static constexpr int WhitePawn = 0;
	static constexpr int WhiteKnight = 1;
	static constexpr int WhiteBishop = 2;
	static constexpr int WhiteRook = 3;
	static constexpr int WhiteQueen = 4;
	static constexpr int WhiteKing = 5;
	static constexpr int BlackPawn = 6;
	static constexpr int BlackKnight = 7;
	static constexpr int BlackBishop = 8;
	static constexpr int BlackRook = 9;
	static constexpr int BlackQueen = 10;
	static constexpr int BlackKing = 11;
	static constexpr int NumberOfPieceTypes = 12;
	//following indices are square states:
	//0 to (64 * 12 - 1)
	//0 to 11 being the states of the first square {0, 0} for each piece
	static constexpr size_t BlackToMoveIdx = 64 * 12 + 0;
	static constexpr size_t WhiteKingSideCastleIdx = 64 * 12 + 1;
	static constexpr size_t WhiteQueenSideCastleIdx = 64 * 12 + 2;
	static constexpr size_t BlackKingSideCastleIdx = 64 * 12 + 3;
	static constexpr size_t BlackQueenSideCastleIdx = 64 * 12 + 4;
	static constexpr size_t EnPassantAIdx = 64 * 12 + 5; //A to H = 64 * 12 + 5 to 64 * 12 + 12
	static constexpr size_t EnPassantHIdx = 64 * 12 + 12;
	static constexpr size_t NumberOfKeys = 781;
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

constexpr uint64_t ZobristHash::GetKey(const Piece& piece, bool isWhite)
{
	int idx = 0;
	switch (piece.m_Type)
	{
	case PieceType::Pawn:
		idx = piece.m_Square * NumberOfPieceTypes + (isWhite ? WhitePawn : BlackPawn);
		break;
	case PieceType::Knight:
		idx = piece.m_Square * NumberOfPieceTypes + (isWhite ? WhiteKnight : BlackKnight);
		break;
	case PieceType::Bishop:
		idx = piece.m_Square * NumberOfPieceTypes + (isWhite ? WhiteBishop : BlackBishop);
		break;;
	case PieceType::Rook:
		idx = piece.m_Square * NumberOfPieceTypes + (isWhite ? WhiteRook : BlackRook);
		break;
	case PieceType::Queen:
		idx = piece.m_Square * NumberOfPieceTypes + (isWhite ? WhiteQueen : BlackQueen);
		break;
	default:
	case PieceType::King:
		idx = piece.m_Square * NumberOfPieceTypes + (isWhite ? WhiteKing : BlackKing);
		break;
	}

	return Table[idx];
}