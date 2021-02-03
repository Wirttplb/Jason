#include "pch.h"
#include "ZobristHash.h"
#include <random>
#include <unordered_set>

//One number for each piece at each square
//One number to indicate the side to move is black
//Four numbers to indicate the castling rights
//Eight numbers to indicate the file of a valid En passant square, if any

static std::array<uint64_t, 781> GenerateRandomKeys()
{
	std::default_random_engine generator;
	std::uniform_int_distribution<uint64_t> distribution(0, std::numeric_limits<uint64_t>::max());

	std::unordered_set<uint64_t> randSet;
	while (randSet.size() < 781)
	{
		uint64_t key = distribution(generator);
		randSet.insert(key);
	}

	std::array<uint64_t, 781> keys = {};
	for (std::unordered_set<uint64_t>::const_iterator randomNumber = randSet.begin(); randomNumber != randSet.end(); randomNumber++)
	{
		const size_t idx = std::distance(randSet.begin(), randomNumber);
		keys[idx] = *randomNumber;
	}

	return keys;
}

//table size = 64 * 12 + 1 + 4 + 8 = 781: 64 squares + 1 color to move + 4 castling rights + 8 en passant file
const std::array<uint64_t, 781> ZobristHash::Table = GenerateRandomKeys();

uint64_t ZobristHash::Init()
{
	uint64_t hash = GetKey(PieceType::Rook, a1, true)
		^ GetKey(PieceType::Knight, b1, true)
		^ GetKey(PieceType::Bishop, c1, true)
		^ GetKey(PieceType::Queen, d1, true)
		^ GetKey(PieceType::King, e1, true)
		^ GetKey(PieceType::Bishop, f1, true)
		^ GetKey(PieceType::Knight, g1, true)
		^ GetKey(PieceType::Rook, h1, true)
		^ GetKey(PieceType::Pawn, a2, true)
		^ GetKey(PieceType::Pawn, b2, true)
		^ GetKey(PieceType::Pawn, c2, true)
		^ GetKey(PieceType::Pawn, d2, true)
		^ GetKey(PieceType::Pawn, e2, true)
		^ GetKey(PieceType::Pawn, f2, true)
		^ GetKey(PieceType::Pawn, g2, true)
		^ GetKey(PieceType::Pawn, h2, true)
		^ GetKey(PieceType::Rook, a8, false)
		^ GetKey(PieceType::Knight, b8, false)
		^ GetKey(PieceType::Bishop, c8, false)
		^ GetKey(PieceType::Queen, d8, false)
		^ GetKey(PieceType::King, e8, false)
		^ GetKey(PieceType::Bishop, f8, false)
		^ GetKey(PieceType::Knight, g8, false)
		^ GetKey(PieceType::Rook, h8, false)
		^ GetKey(PieceType::Pawn, a7, false)
		^ GetKey(PieceType::Pawn, b7, false)
		^ GetKey(PieceType::Pawn, c7, false)
		^ GetKey(PieceType::Pawn, d7, false)
		^ GetKey(PieceType::Pawn, e7, false)
		^ GetKey(PieceType::Pawn, f7, false)
		^ GetKey(PieceType::Pawn, g7, false)
		^ GetKey(PieceType::Pawn, h7, false)
		^ GetWhiteKingSideCastleKey()
		^ GetWhiteQueenSideCastleKey()
		^ GetBlackKingSideCastleKey()
		^ GetBlackQueenSideCastleKey();

	return hash;
}



