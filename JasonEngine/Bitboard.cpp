#include "pch.h"
#include "Bitboard.h"

Bitboard::Bitboard()
{
}

Bitboard::Bitboard(const std::array<bool, 64>& bits)
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

Bitboard::Bitboard(int square)
{
	*this = Bitboard(static_cast<Square>(square));
	m_Value = SquareToBitboardTable[square];
}

Bitboard Bitboard::operator~() const
{
	Bitboard result = *this;
	result.m_Value = ~result.m_Value;
	return result;
}

Bitboard Bitboard::operator&(const Bitboard& bitboard) const
{
	Bitboard result = *this;
	result &= bitboard;
	return result;
}

void Bitboard::operator&=(const Bitboard& bitboard)
{
	this->m_Value &= bitboard.m_Value;
}

Bitboard Bitboard::operator|(const Bitboard& bitboard) const
{
	Bitboard result = *this;
	result |= bitboard;
	return result;
}

void Bitboard::operator|=(const Bitboard& bitboard)
{
	this->m_Value |= bitboard.m_Value;
}

Bitboard Bitboard::operator^(const Bitboard& bitboard) const
{
	Bitboard result = *this;
	result ^= bitboard;
	return result;
}

void Bitboard::operator^=(const Bitboard& bitboard)
{
	this->m_Value ^= bitboard.m_Value;
}

Bitboard Bitboard::operator<<(int shift) const
{
	Bitboard result = *this;
	result.m_Value <<= shift;
	return result;
}

Bitboard Bitboard::operator>>(int shift) const
{
	Bitboard result = *this;
	result.m_Value >>= shift;
	return result;
}

Bitboard Bitboard::operator-(const Bitboard& bitboard) const
{
	Bitboard result = *this;
	result.m_Value -= bitboard.m_Value;
	return result;
}

Bitboard Bitboard::RotateLeft(int s) const
{
	Bitboard bitboard = *this;
	bitboard.m_Value = (bitboard.m_Value << s) | (bitboard.m_Value >> (64 - s));
	return  bitboard;
}

Bitboard Bitboard::RotateRight(int s) const
{
	Bitboard bitboard = *this;
	bitboard.m_Value = (bitboard.m_Value >> s) | (bitboard.m_Value << (64 - s));
	return  bitboard;
}

Bitboard Bitboard::FlipVertically() const
{
	//https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating
	Bitboard flippedBitboard;
	flippedBitboard.m_Value = _byteswap_uint64(this->m_Value);
	return flippedBitboard;
}

Bitboard Bitboard::FlipA1H8() const
{
	//https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating
	Bitboard flippedBitboard = *this;
	uint64_t t;
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

Bitboard Bitboard::Rotate90Clockwise() const
{
	Bitboard rotatedBitboard = this->FlipA1H8();
	rotatedBitboard = rotatedBitboard.FlipVertically();
	return rotatedBitboard;
}

Bitboard Bitboard::Rotate90AntiClockwise() const
{
	Bitboard rotatedBitboard = this->FlipVertically();
	rotatedBitboard = rotatedBitboard.FlipA1H8();
	return rotatedBitboard;
}

Bitboard Bitboard::PseudoRotate45Clockwise() const
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

Bitboard Bitboard::PseudoRotate45AntiClockwise() const
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

Bitboard Bitboard::UndoPseudoRotate45Clockwise() const
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

Bitboard Bitboard::UndoPseudoRotate45AntiClockwise() const
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

int Bitboard::CountSetBits() const
{
	// https://en.wikipedia.org/wiki/Hamming_weight
	// This is better when most bits in x are 0
	// This is algorithm works the same for all data sizes.
	// This algorithm uses 3 arithmetic operations and 1 comparison/branch per "1" bit in x.
	int count;
	uint64_t x = m_Value;
	for (count = 0; x; count++)
	{
		x &= x - 1;
	}

	return count;
}

int Bitboard::GetSquare() const
{
	return static_cast<int>(_tzcnt_u64(m_Value));// __builtin_ctzll(m_Value);
}

std::string Bitboard::ToString() const
{
	std::string string;

	for (int j = 7; j >= 0; j--)
	{
		for (int i = 0; i <= 7; i++)
		{
			int bitNumber = j * 8 + i;
			uint64_t mask = uint64_t(1) << bitNumber;
			uint64_t bit = (m_Value & mask) >> bitNumber;
			string += std::to_string(bit);
		}

		string += "\n";
	}

	return string;
}

std::ostream& operator<<(std::ostream& os, const Bitboard& bitboard)
{
	return os << bitboard.ToString();
}