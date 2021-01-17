#include "pch.h"
#include "ZobristHash.h"
#include "BasicDefinitions.h"
#include <random>
#include <unordered_set>
#include <assert.h>

//One number for each piece at each square
//One number to indicate the side to move is black
//Four numbers to indicate the castling rights, though usually 16 (2 ^ 4) are used for speed
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
constexpr int NumberOfPieces = 12;
//following indices are square states:
//0 to (64 * 12 - 1)
//0 to 11 being the states of the first square {0, 0} for each piece

constexpr size_t WhiteToMoveIdx = 64 * 12 + 0;
constexpr size_t WhiteKingSideCastleIdx = 64 * 12 + 1;
constexpr size_t WhiteQueenSideCastleIdx = 64 * 12 + 2;
constexpr size_t BlackKingSideCastleIdx = 64 * 12 + 3;
constexpr size_t BlackQueenSideCastleIdx = 64 * 12 + 4;
constexpr size_t EnPassantAIdx = 64 * 12 + 5;
constexpr size_t EnPassantBIdx = 64 * 12 + 6;
constexpr size_t EnPassantCIdx = 64 * 12 + 7;
constexpr size_t EnPassantDIdx = 64 * 12 + 8;
constexpr size_t EnPassantEIdx = 64 * 12 + 9;
constexpr size_t EnPassantFIdx = 64 * 12 + 10;
constexpr size_t EnPassantGIdx = 64 * 12 + 11;
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
	uint64_t hash = GetKey(Piece(PieceType::Rook, 0, 0), true)
		^ GetKey(Piece(PieceType::Knight, 1, 0), true)
		^ GetKey(Piece(PieceType::Bishop, 2, 0), true)
		^ GetKey(Piece(PieceType::Queen, 3, 0), true)
		^ GetKey(Piece(PieceType::King, 4, 0), true)
		^ GetKey(Piece(PieceType::Bishop, 5, 0), true)
		^ GetKey(Piece(PieceType::Knight, 6, 0), true)
		^ GetKey(Piece(PieceType::Rook, 7, 0), true)
		^ GetKey(Piece(PieceType::Pawn, 0, 1), true)
		^ GetKey(Piece(PieceType::Pawn, 1, 1), true)
		^ GetKey(Piece(PieceType::Pawn, 2, 1), true)
		^ GetKey(Piece(PieceType::Pawn, 3, 1), true)
		^ GetKey(Piece(PieceType::Pawn, 4, 1), true)
		^ GetKey(Piece(PieceType::Pawn, 5, 1), true)
		^ GetKey(Piece(PieceType::Pawn, 6, 1), true)
		^ GetKey(Piece(PieceType::Pawn, 7, 1), true)
		^ GetKey(Piece(PieceType::Rook, 0, 7), false)
		^ GetKey(Piece(PieceType::Knight, 1, 7), false)
		^ GetKey(Piece(PieceType::Bishop, 2, 7), false)
		^ GetKey(Piece(PieceType::Queen, 3, 7), false)
		^ GetKey(Piece(PieceType::King, 4, 7), false)
		^ GetKey(Piece(PieceType::Bishop, 5, 7), false)
		^ GetKey(Piece(PieceType::Knight, 6, 7), false)
		^ GetKey(Piece(PieceType::Rook, 7, 7), false)
		^ GetKey(Piece(PieceType::Pawn, 0, 6), false)
		^ GetKey(Piece(PieceType::Pawn, 1, 6), false)
		^ GetKey(Piece(PieceType::Pawn, 2, 6), false)
		^ GetKey(Piece(PieceType::Pawn, 3, 6), false)
		^ GetKey(Piece(PieceType::Pawn, 4, 6), false)
		^ GetKey(Piece(PieceType::Pawn, 5, 6), false)
		^ GetKey(Piece(PieceType::Pawn, 6, 6), false)
		^ GetKey(Piece(PieceType::Pawn, 7, 6), false);

	return hash;
}

//int ZobristHash::GetHash(const Position& position)
//{
//	for (int i = 0; i < 64; i++)
//	{
//		for (int j = 0; j < 64; j++)
//		{
//			Table[i][j] = std::rand();
//		}
//	}
//}

static int SquareToIdx(const std::array<int, 2>& square)
{
	//{0, 0} is a1, {7, 7} h8
	return square[0] + 8 * square[1];
}

uint64_t ZobristHash::GetKey(const Piece& piece, bool isWhite)
{
	int idx = 0;
	switch (piece.m_Type)
	{
	case PieceType::Pawn:
		idx = SquareToIdx(piece.m_Position) + (isWhite ? WhitePawn : BlackPawn) * NumberOfPieces;
		break;
	case PieceType::Knight:
		idx = SquareToIdx(piece.m_Position) + (isWhite ? WhiteKnight : BlackKnight) * NumberOfPieces;
		break;
	case PieceType::Bishop:
		idx = SquareToIdx(piece.m_Position) + (isWhite ? WhiteBishop : BlackBishop) * NumberOfPieces;
		break;;
	case PieceType::Rook:
		idx = SquareToIdx(piece.m_Position) + (isWhite ? WhiteRook : BlackRook) * NumberOfPieces;
		break;
	case PieceType::Queen:
		idx = SquareToIdx(piece.m_Position) + (isWhite ? WhiteQueen : BlackQueen) * NumberOfPieces;
		break;
	case PieceType::King:
		idx = SquareToIdx(piece.m_Position) + (isWhite ? WhiteKing : BlackKing) * NumberOfPieces;
		break;
	default:
		assert(false);
		break;
	}

	return Table[idx];
}

uint64_t ZobristHash::GetWhiteToMoveKey()
{
	return Table[WhiteToMoveIdx];
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

uint64_t ZobristHash::GetEnPassantKey(int fileIdx)
{
	return Table[EnPassantAIdx + fileIdx];
}
