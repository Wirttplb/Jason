#pragma once
#include <stdint.h>
#include <array>
#include <string>
#include <ostream>
#include "BasicDefinitions.h"


/// <summary>Bitboard representation, Little endian rank-file (LERF)</summary>
/// 
/// Compass Rose for square indices:
/// noWe         north         noEa
///           +7 + 8 + 9
///             \  |  /
///    west - 1 < -0 -> + 1    east
///             /  |  \
///          - 9 - 8 - 7
/// soWe         south         soEa
/// 
class Bitboard
{
public:
	constexpr Bitboard() {};

	/// <summary>Constructor from list of bits, from a8-h8 to a1-h1, like a chessboard from above.</summary>
	constexpr Bitboard(const std::array<bool, 64>& bits);
	constexpr Bitboard(Square square);
	constexpr Bitboard(int square);
	constexpr Bitboard& operator=(const Bitboard& bitboard) = default;

	constexpr Bitboard operator~() const;
	constexpr Bitboard operator&(const Bitboard& bitboard) const;
	constexpr void operator&=(const Bitboard& bitboard);
	constexpr Bitboard operator|(const Bitboard& bitboard) const;
	constexpr void operator|=(const Bitboard& bitboard);
	constexpr Bitboard operator^(const Bitboard& bitboard) const;
	constexpr void operator^=(const Bitboard& bitboard);
	constexpr Bitboard operator<<(int shift) const;
	constexpr Bitboard operator>>(int shift) const;
	constexpr Bitboard operator-(const Bitboard& bitboard) const;
	inline operator uint64_t() const { return m_Value; };
	inline bool operator==(uint64_t value) const { return m_Value == value; };
	inline bool operator==(const Bitboard& bitboard) const { return (m_Value == bitboard.m_Value); }; //for hash definition

	inline Bitboard RotateLeft(int s) const;
	inline Bitboard RotateRight(int s) const;
	inline Bitboard FlipVertically() const;
	inline Bitboard FlipA1H8() const;
	inline Bitboard Rotate90Clockwise() const;
	inline Bitboard Rotate90AntiClockwise() const;
	/// <remark> Main Diagonal is mapped to 1st rank </summary>
	inline Bitboard PseudoRotate45Clockwise() const;
	inline Bitboard PseudoRotate45AntiClockwise() const;
	inline Bitboard UndoPseudoRotate45Clockwise() const;
	inline Bitboard UndoPseudoRotate45AntiClockwise() const;

	inline int CountSetBits() const;

	/// <returns>Square index ; only a single bit should be set, otherwise unknown result</returns>
	/// <remark>Row index is square / 8 (or square >> 3) ; File index is square % 8 (or square & 7)<remark>
	inline int GetSquare() const;

	std::string ToString() const;

	uint64_t m_Value = 0;
};

std::ostream& operator<<(std::ostream& os, const Bitboard& bitboard);

constexpr Bitboard::Bitboard(const std::array<bool, 64>& bits)
{
	m_Value = 0;
	size_t count = 0;
	for (int j = 7; j >= 0; j--)
	{
		for (int i = 0; i <= 7; i++)
		{
			int bitNumber = j * 8 + i;
			uint64_t mask = uint64_t(bits[count]) << bitNumber;
			m_Value |= mask;
			count++;
		}
	}
}

static constexpr std::array<uint64_t, 64> GenerateSquareToBitboardTable()
{
	std::array<uint64_t, 64> squareToBitboardTable = {};
	for (int i = 0; i < 64; i++)
	{
		uint64_t bitboard = uint64_t(1) << i;
		squareToBitboardTable[i] = bitboard;
	}

	return squareToBitboardTable;
}

static constexpr std::array<uint64_t, 64> SquareToBitboardTable = GenerateSquareToBitboardTable();

constexpr Bitboard::Bitboard(Square square)
{
	m_Value = SquareToBitboardTable[static_cast<int>(square)];
}

constexpr Bitboard::Bitboard(int square)
{
	*this = Bitboard(static_cast<Square>(square));
	m_Value = SquareToBitboardTable[square];
}

template<>
struct std::hash<Bitboard>
{
	uint64_t operator()(const Bitboard& position) const
	{
		return position.m_Value;
	}
};

constexpr Bitboard Bitboard::operator~() const
{
	Bitboard result = *this;
	result.m_Value = ~result.m_Value;
	return result;
}

constexpr Bitboard Bitboard::operator&(const Bitboard& bitboard) const
{
	Bitboard result = *this;
	result &= bitboard;
	return result;
}

constexpr void Bitboard::operator&=(const Bitboard& bitboard)
{
	this->m_Value &= bitboard.m_Value;
}

constexpr Bitboard Bitboard::operator|(const Bitboard& bitboard) const
{
	Bitboard result = *this;
	result |= bitboard;
	return result;
}

constexpr void Bitboard::operator|=(const Bitboard& bitboard)
{
	this->m_Value |= bitboard.m_Value;
}

constexpr Bitboard Bitboard::operator^(const Bitboard& bitboard) const
{
	Bitboard result = *this;
	result ^= bitboard;
	return result;
}

constexpr void Bitboard::operator^=(const Bitboard& bitboard)
{
	this->m_Value ^= bitboard.m_Value;
}

constexpr Bitboard Bitboard::operator<<(int shift) const
{
	Bitboard result = *this;
	result.m_Value <<= shift;
	return result;
}

constexpr Bitboard Bitboard::operator>>(int shift) const
{
	Bitboard result = *this;
	result.m_Value >>= shift;
	return result;
}

constexpr Bitboard Bitboard::operator-(const Bitboard& bitboard) const
{
	Bitboard result = *this;
	result.m_Value -= bitboard.m_Value;
	return result;
}

inline Bitboard Bitboard::RotateLeft(int s) const
{
	Bitboard bitboard = *this;
	bitboard.m_Value = (bitboard.m_Value << s) | (bitboard.m_Value >> (64 - s));
	return  bitboard;
}

inline Bitboard Bitboard::RotateRight(int s) const
{
	Bitboard bitboard = *this;
	bitboard.m_Value = (bitboard.m_Value >> s) | (bitboard.m_Value << (64 - s));
	return  bitboard;
}

inline Bitboard Bitboard::FlipVertically() const
{
	//https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating
	Bitboard flippedBitboard;
	flippedBitboard.m_Value = _byteswap_uint64(this->m_Value);
	return flippedBitboard;
}

inline Bitboard Bitboard::FlipA1H8() const
{
	//https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating
	Bitboard flippedBitboard = *this;
	uint64_t t = 0;
	const uint64_t k1 = (0x5500550055005500);
	const uint64_t k2 = (0x3333000033330000);
	const uint64_t k4 = (0x0f0f0f0f00000000);
	t = k4 & (flippedBitboard.m_Value ^ (flippedBitboard.m_Value << 28));
	flippedBitboard.m_Value ^= t ^ (t >> 28);
	t = k2 & (flippedBitboard.m_Value ^ (flippedBitboard.m_Value << 14));
	flippedBitboard.m_Value ^= t ^ (t >> 14);
	t = k1 & (flippedBitboard.m_Value ^ (flippedBitboard.m_Value << 7));
	flippedBitboard.m_Value ^= t ^ (t >> 7);

	return flippedBitboard;
}

inline Bitboard Bitboard::Rotate90Clockwise() const
{
	Bitboard rotatedBitboard = this->FlipA1H8();
	rotatedBitboard = rotatedBitboard.FlipVertically();
	return rotatedBitboard;
}

inline Bitboard Bitboard::Rotate90AntiClockwise() const
{
	Bitboard rotatedBitboard = this->FlipVertically();
	rotatedBitboard = rotatedBitboard.FlipA1H8();
	return rotatedBitboard;
}

inline Bitboard Bitboard::PseudoRotate45Clockwise() const
{
	//https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating
	Bitboard rotatedBitboard = *this;
	const uint64_t k1 = (0xAAAAAAAAAAAAAAAA);
	const uint64_t k2 = (0xCCCCCCCCCCCCCCCC);
	const uint64_t k4 = (0xF0F0F0F0F0F0F0F0);
	rotatedBitboard.m_Value ^= k1 & (rotatedBitboard.m_Value ^ rotatedBitboard.RotateRight(8));
	rotatedBitboard.m_Value ^= k2 & (rotatedBitboard.m_Value ^ rotatedBitboard.RotateRight(16));
	rotatedBitboard.m_Value ^= k4 & (rotatedBitboard.m_Value ^ rotatedBitboard.RotateRight(32));
	return rotatedBitboard;
}

inline Bitboard Bitboard::PseudoRotate45AntiClockwise() const
{
	//https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating
	Bitboard rotatedBitboard = *this;
	constexpr uint64_t k1 = (0x5555555555555555);
	constexpr uint64_t k2 = (0x3333333333333333);
	constexpr uint64_t k4 = (0x0f0f0f0f0f0f0f0f);
	rotatedBitboard.m_Value ^= k1 & (rotatedBitboard.m_Value ^ rotatedBitboard.RotateRight(8));
	rotatedBitboard.m_Value ^= k2 & (rotatedBitboard.m_Value ^ rotatedBitboard.RotateRight(16));
	rotatedBitboard.m_Value ^= k4 & (rotatedBitboard.m_Value ^ rotatedBitboard.RotateRight(32));
	return rotatedBitboard;
}

inline Bitboard Bitboard::UndoPseudoRotate45Clockwise() const
{
	Bitboard rotatedBitboard = *this;
	const uint64_t k1 = (0xAAAAAAAAAAAAAAAA);
	const uint64_t k2 = (0xCCCCCCCCCCCCCCCC);
	const uint64_t k4 = (0xF0F0F0F0F0F0F0F0);
	rotatedBitboard.m_Value ^= k1 & (rotatedBitboard.m_Value ^ rotatedBitboard.RotateLeft(8));
	rotatedBitboard.m_Value ^= k2 & (rotatedBitboard.m_Value ^ rotatedBitboard.RotateLeft(16));
	rotatedBitboard.m_Value ^= k4 & (rotatedBitboard.m_Value ^ rotatedBitboard.RotateLeft(32));
	return rotatedBitboard;
}

inline Bitboard Bitboard::UndoPseudoRotate45AntiClockwise() const
{
	Bitboard rotatedBitboard = *this;
	constexpr uint64_t k1 = (0x5555555555555555);
	constexpr uint64_t k2 = (0x3333333333333333);
	constexpr uint64_t k4 = (0x0f0f0f0f0f0f0f0f);
	rotatedBitboard.m_Value ^= k1 & (rotatedBitboard.m_Value ^ rotatedBitboard.RotateLeft(8));
	rotatedBitboard.m_Value ^= k2 & (rotatedBitboard.m_Value ^ rotatedBitboard.RotateLeft(16));
	rotatedBitboard.m_Value ^= k4 & (rotatedBitboard.m_Value ^ rotatedBitboard.RotateLeft(32));
	return rotatedBitboard;
}

inline int Bitboard::CountSetBits() const
{
	// https://en.wikipedia.org/wiki/Hamming_weight
	// This is better when most bits in x are 0
	// This algorithm works the same for all data sizes.
	// This algorithm uses 3 arithmetic operations and 1 comparison/branch per "1" bit in x.
	int count;
	uint64_t x = m_Value;
	for (count = 0; x; count++)
	{
		x &= x - 1;
	}

	return count;
}

inline int Bitboard::GetSquare() const
{
	return static_cast<int>(_tzcnt_u64(m_Value));
}

static constexpr Bitboard _a1(Square::a1);
static constexpr Bitboard _a2(Square::a2);
static constexpr Bitboard _a3(Square::a3);
static constexpr Bitboard _a4(Square::a4);
static constexpr Bitboard _a5(Square::a5);
static constexpr Bitboard _a6(Square::a6);
static constexpr Bitboard _a7(Square::a7);
static constexpr Bitboard _a8(Square::a8);

static constexpr Bitboard _b1(Square::b1);
static constexpr Bitboard _b2(Square::b2);
static constexpr Bitboard _b3(Square::b3);
static constexpr Bitboard _b4(Square::b4);
static constexpr Bitboard _b5(Square::b5);
static constexpr Bitboard _b6(Square::b6);
static constexpr Bitboard _b7(Square::b7);
static constexpr Bitboard _b8(Square::b8);

static constexpr Bitboard _c1(Square::c1);
static constexpr Bitboard _c2(Square::c2);
static constexpr Bitboard _c3(Square::c3);
static constexpr Bitboard _c4(Square::c4);
static constexpr Bitboard _c5(Square::c5);
static constexpr Bitboard _c6(Square::c6);
static constexpr Bitboard _c7(Square::c7);
static constexpr Bitboard _c8(Square::c8);

static constexpr Bitboard _d1(Square::d1);
static constexpr Bitboard _d2(Square::d2);
static constexpr Bitboard _d3(Square::d3);
static constexpr Bitboard _d4(Square::d4);
static constexpr Bitboard _d5(Square::d5);
static constexpr Bitboard _d6(Square::d6);
static constexpr Bitboard _d7(Square::d7);
static constexpr Bitboard _d8(Square::d8);

static constexpr Bitboard _e1(Square::e1);
static constexpr Bitboard _e2(Square::e2);
static constexpr Bitboard _e3(Square::e3);
static constexpr Bitboard _e4(Square::e4);
static constexpr Bitboard _e5(Square::e5);
static constexpr Bitboard _e6(Square::e6);
static constexpr Bitboard _e7(Square::e7);
static constexpr Bitboard _e8(Square::e8);

static constexpr Bitboard _f1(Square::f1);
static constexpr Bitboard _f2(Square::f2);
static constexpr Bitboard _f3(Square::f3);
static constexpr Bitboard _f4(Square::f4);
static constexpr Bitboard _f5(Square::f5);
static constexpr Bitboard _f6(Square::f6);
static constexpr Bitboard _f7(Square::f7);
static constexpr Bitboard _f8(Square::f8);

static constexpr Bitboard _g1(Square::g1);
static constexpr Bitboard _g2(Square::g2);
static constexpr Bitboard _g3(Square::g3);
static constexpr Bitboard _g4(Square::g4);
static constexpr Bitboard _g5(Square::g5);
static constexpr Bitboard _g6(Square::g6);
static constexpr Bitboard _g7(Square::g7);
static constexpr Bitboard _g8(Square::g8);

static constexpr Bitboard _h1(Square::h1);
static constexpr Bitboard _h2(Square::h2);
static constexpr Bitboard _h3(Square::h3);
static constexpr Bitboard _h4(Square::h4);
static constexpr Bitboard _h5(Square::h5);
static constexpr Bitboard _h6(Square::h6);
static constexpr Bitboard _h7(Square::h7);
static constexpr Bitboard _h8(Square::h8);

static constexpr Bitboard _a
({
	1, 0, 0, 0, 0, 0, 0, 0,
	1, 0, 0, 0, 0, 0, 0, 0,
	1, 0, 0, 0, 0, 0, 0, 0,
	1, 0, 0, 0, 0, 0, 0, 0,
	1, 0, 0, 0, 0, 0, 0, 0,
	1, 0, 0, 0, 0, 0, 0, 0,
	1, 0, 0, 0, 0, 0, 0, 0,
	1, 0, 0, 0, 0, 0, 0, 0
});

static constexpr Bitboard _b
({
	0, 1, 0, 0, 0, 0, 0, 0,
	0, 1, 0, 0, 0, 0, 0, 0,
	0, 1, 0, 0, 0, 0, 0, 0,
	0, 1, 0, 0, 0, 0, 0, 0,
	0, 1, 0, 0, 0, 0, 0, 0,
	0, 1, 0, 0, 0, 0, 0, 0,
	0, 1, 0, 0, 0, 0, 0, 0,
	0, 1, 0, 0, 0, 0, 0, 0
});

static constexpr Bitboard _c
({
	0, 0, 1, 0, 0, 0, 0, 0,
	0, 0, 1, 0, 0, 0, 0, 0,
	0, 0, 1, 0, 0, 0, 0, 0,
	0, 0, 1, 0, 0, 0, 0, 0,
	0, 0, 1, 0, 0, 0, 0, 0,
	0, 0, 1, 0, 0, 0, 0, 0,
	0, 0, 1, 0, 0, 0, 0, 0,
	0, 0, 1, 0, 0, 0, 0, 0
});

static constexpr Bitboard _d
({
	0, 0, 0, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 0, 0, 0, 0
});

static constexpr Bitboard _e
({
	0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 0, 1, 0, 0, 0
});

static constexpr Bitboard _f
({
	0, 0, 0, 0, 0, 1, 0, 0,
	0, 0, 0, 0, 0, 1, 0, 0,
	0, 0, 0, 0, 0, 1, 0, 0,
	0, 0, 0, 0, 0, 1, 0, 0,
	0, 0, 0, 0, 0, 1, 0, 0,
	0, 0, 0, 0, 0, 1, 0, 0,
	0, 0, 0, 0, 0, 1, 0, 0,
	0, 0, 0, 0, 0, 1, 0, 0
});

static constexpr Bitboard _g
({
	0, 0, 0, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, 0, 1, 0
});

static constexpr Bitboard _h
({
	0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 1
});

static constexpr Bitboard _8
({
	1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
});

static constexpr Bitboard _7
({
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
});

static constexpr Bitboard _6
({
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
});

static constexpr Bitboard _5
({
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
});

static constexpr Bitboard _4
({
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
});

static constexpr Bitboard _3
({
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
});

static constexpr Bitboard _2
({
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0
});

static constexpr Bitboard _1
({
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1
});

static constexpr Bitboard LightSquares
({
	1, 0, 1, 0, 1, 0, 1, 0,
	0, 1, 0, 1, 0, 1, 0, 1,
	1, 0, 1, 0, 1, 0, 1, 0,
	0, 1, 0, 1, 0, 1, 0, 1,
	1, 0, 1, 0, 1, 0, 1, 0,
	0, 1, 0, 1, 0, 1, 0, 1,
	1, 0, 1, 0, 1, 0, 1, 0,
	0, 1, 0, 1, 0, 1, 0, 1
});

static constexpr Bitboard DarkSquares
({
	0, 1, 0, 1, 0, 1, 0, 1,
	1, 0, 1, 0, 1, 0, 1, 0,
	0, 1, 0, 1, 0, 1, 0, 1,
	1, 0, 1, 0, 1, 0, 1, 0,
	0, 1, 0, 1, 0, 1, 0, 1,
	1, 0, 1, 0, 1, 0, 1, 0,
	0, 1, 0, 1, 0, 1, 0, 1,
	1, 0, 1, 0, 1, 0, 1, 0
});

static constexpr Bitboard a1h8
({
	0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, 1, 0, 0,
	0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 1, 0, 0, 0, 0,
	0, 0, 1, 0, 0, 0, 0, 0,
	0, 1, 0, 0, 0, 0, 0, 0,
	1, 0, 0, 0, 0, 0, 0, 0
});

static constexpr Bitboard b1h7
({
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, 1, 0, 0,
	0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 1, 0, 0, 0, 0,
	0, 0, 1, 0, 0, 0, 0, 0,
	0, 1, 0, 0, 0, 0, 0, 0
});

static constexpr Bitboard c1h6
({
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, 1, 0, 0,
	0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 1, 0, 0, 0, 0,
	0, 0, 1, 0, 0, 0, 0, 0
});

static constexpr std::array<Bitboard, 8> _rows = { _1, _2, _3, _4, _5, _6, _7, _8 };
static constexpr std::array<Bitboard, 8> _files = { _a, _b, _c, _d, _e, _f, _g, _h };

///<remark>Includes the r row</summary>
static constexpr Bitboard GetRowsAbove(int r)
{
	Bitboard rows;
	for (int i = r; i < 8; i++)
	{
		rows |= _rows[i];
	}

	return rows;
}

static constexpr Bitboard GetRowsUnder(int r)
{
	Bitboard rows;
	for (int i = r; i >= 0; i--)
	{
		rows |= _rows[i];
	}

	return rows;
}

///<remark>Includes the f file</summary>
static constexpr Bitboard GetFilesOnRight(int f)
{
	Bitboard files;
	for (int i = f; i < 8; i++)
	{
		files |= _files[i];
	}

	return files;
}

static constexpr Bitboard GetFilesOnLeft(int f)
{
	Bitboard files;
	for (int i = f; i >= 0; i--)
	{
		files |= _files[i];
	}

	return files;
}

static constexpr std::array<Bitboard, 8> GenerateFilesOnRightOf()
{
	std::array<Bitboard, 8> result;
	for (int f = 0; f < 8; f++)
	{
		result[f] = GetFilesOnRight(f);
	}

	return result;
}

static constexpr std::array<Bitboard, 8> GenerateFilesOnLeftOf()
{
	std::array<Bitboard, 8> result;
	for (int f = 0; f < 8; f++)
	{
		result[f] = GetFilesOnLeft(f);
	}

	return result;
}

static constexpr std::array<Bitboard, 8> GenerateRowsUnderOf()
{
	std::array<Bitboard, 8> result;
	for (int r = 0; r < 8; r++)
	{
		result[r] = GetRowsUnder(r);
	}

	return result;
}

static constexpr std::array<Bitboard, 8> GenerateRowsAboveOf()
{
	std::array<Bitboard, 8> result;
	for (int r = 0; r < 8; r++)
	{
		result[r] = GetRowsAbove(r);
	}

	return result;
}

static constexpr std::array<Bitboard, 8> FilesOnRight = GenerateFilesOnRightOf();
static constexpr std::array<Bitboard, 8> FilesOnLeft = GenerateFilesOnLeftOf();
static constexpr std::array<Bitboard, 8> RowsUnder = GenerateRowsUnderOf();
static constexpr std::array<Bitboard, 8> RowsAbove = GenerateRowsAboveOf();