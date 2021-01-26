#include "pch.h"
#include "ZobristHash.h"
#include "BasicDefinitions.h"
#include <random>
#include <unordered_set>
#include <assert.h>

//One number for each piece at each square
//One number to indicate the side to move is black
//Four numbers to indicate the castling rights
//Eight numbers to indicate the file of a valid En passant square, if any

constexpr int WhitePawn = 0;
constexpr int WhiteKnight = 1;
constexpr int WhiteBishop = 2;
constexpr int WhiteRook = 3;
constexpr int WhiteQueen = 4;
constexpr int WhiteKing = 5;
constexpr int BlackPawn = 6;
constexpr int BlackKnight = 7;
constexpr int BlackBishop = 8;
constexpr int BlackRook = 9;
constexpr int BlackQueen = 10;
constexpr int BlackKing = 11;
constexpr int NumberOfPieceTypes = 12;
//following indices are square states:
//0 to (64 * 12 - 1)
//0 to 11 being the states of the first square {0, 0} for each piece

constexpr size_t BlackToMoveIdx = 64 * 12 + 0;
constexpr size_t WhiteKingSideCastleIdx = 64 * 12 + 1;
constexpr size_t WhiteQueenSideCastleIdx = 64 * 12 + 2;
constexpr size_t BlackKingSideCastleIdx = 64 * 12 + 3;
constexpr size_t BlackQueenSideCastleIdx = 64 * 12 + 4;
constexpr size_t EnPassantAIdx = 64 * 12 + 5; //A to H = 64 * 12 + 5 to 64 * 12 + 12
constexpr size_t EnPassantHIdx = 64 * 12 + 12;
constexpr size_t NumberOfKeys = 781;
static_assert(EnPassantHIdx == (NumberOfKeys - 1));

static std::array<uint64_t, NumberOfKeys> GenerateRandomKeys()
{
	std::default_random_engine generator;
	std::uniform_int_distribution<uint64_t> distribution(0, std::numeric_limits<uint64_t>::max());

	std::unordered_set<uint64_t> randSet;
	while (randSet.size() < NumberOfKeys)
	{
		uint64_t key = distribution(generator);
		randSet.insert(key);
	}

	std::array<uint64_t, NumberOfKeys> keys;
	for (std::unordered_set<uint64_t>::const_iterator randomNumber = randSet.begin(); randomNumber != randSet.end(); randomNumber++)
	{
		const size_t idx = std::distance(randSet.begin(), randomNumber);
		keys[idx] = *randomNumber;
	}

	return keys;
}

//table size = 64 * 12 + 1 + 4 + 8 = 781: 64 squares + 1 color to move + 4 castling rights + 8 en passant file
static const std::array<uint64_t, NumberOfKeys> Table = GenerateRandomKeys();

uint64_t ZobristHash::Init()
{
	uint64_t hash = GetKey(Piece(PieceType::Rook, a1), true)
		^ GetKey(Piece(PieceType::Knight, b1), true)
		^ GetKey(Piece(PieceType::Bishop, c1), true)
		^ GetKey(Piece(PieceType::Queen, d1), true)
		^ GetKey(Piece(PieceType::King, e1), true)
		^ GetKey(Piece(PieceType::Bishop, f1), true)
		^ GetKey(Piece(PieceType::Knight, g1), true)
		^ GetKey(Piece(PieceType::Rook, h1), true)
		^ GetKey(Piece(PieceType::Pawn, a2), true)
		^ GetKey(Piece(PieceType::Pawn, b2), true)
		^ GetKey(Piece(PieceType::Pawn, c2), true)
		^ GetKey(Piece(PieceType::Pawn, d2), true)
		^ GetKey(Piece(PieceType::Pawn, e2), true)
		^ GetKey(Piece(PieceType::Pawn, f2), true)
		^ GetKey(Piece(PieceType::Pawn, g2), true)
		^ GetKey(Piece(PieceType::Pawn, h2), true)
		^ GetKey(Piece(PieceType::Rook, a8), false)
		^ GetKey(Piece(PieceType::Knight, b8), false)
		^ GetKey(Piece(PieceType::Bishop, c8), false)
		^ GetKey(Piece(PieceType::Queen, d8), false)
		^ GetKey(Piece(PieceType::King, e8), false)
		^ GetKey(Piece(PieceType::Bishop, f8), false)
		^ GetKey(Piece(PieceType::Knight, g8), false)
		^ GetKey(Piece(PieceType::Rook, h8), false)
		^ GetKey(Piece(PieceType::Pawn, a7), false)
		^ GetKey(Piece(PieceType::Pawn, b7), false)
		^ GetKey(Piece(PieceType::Pawn, c7), false)
		^ GetKey(Piece(PieceType::Pawn, d7), false)
		^ GetKey(Piece(PieceType::Pawn, e7), false)
		^ GetKey(Piece(PieceType::Pawn, f7), false)
		^ GetKey(Piece(PieceType::Pawn, g7), false)
		^ GetKey(Piece(PieceType::Pawn, h7), false)
		^ GetWhiteKingSideCastleKey()
		^ GetWhiteQueenSideCastleKey()
		^ GetBlackKingSideCastleKey()
		^ GetBlackQueenSideCastleKey();

	return hash;
}

uint64_t ZobristHash::GetKey(const Piece& piece, bool isWhite)
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
	case PieceType::King:
		idx = piece.m_Square * NumberOfPieceTypes + (isWhite ? WhiteKing : BlackKing);
		break;
	default:
		assert(false);
		break;
	}

	return Table[idx];
}

uint64_t ZobristHash::GetBlackToMoveKey()
{
	return Table[BlackToMoveIdx];
}

uint64_t ZobristHash::GetWhiteQueenSideCastleKey()
{
	return Table[WhiteQueenSideCastleIdx];
}

uint64_t ZobristHash::GetWhiteKingSideCastleKey()
{
	return Table[WhiteKingSideCastleIdx];
}

uint64_t ZobristHash::GetBlackQueenSideCastleKey()
{
	return Table[BlackQueenSideCastleIdx];
}

uint64_t ZobristHash::GetBlackKingSideCastleKey()
{
	return Table[BlackKingSideCastleIdx];
}

uint64_t ZobristHash::GetEnPassantKey(Square square)
{
	return Table[EnPassantAIdx + square % 8];
}
