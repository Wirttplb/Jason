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

constexpr Bitboard::Bitboard(Square square)
{
	uint64_t mask = uint64_t(1) << static_cast<int>(square);
	m_Value |= mask;
}

Bitboard::Bitboard(int square)
{
	*this = Bitboard(static_cast<Square>(square));
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

int Bitboard::GetSquare() const
{
	return __builtin_ctzll(m_Value);
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