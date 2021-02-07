#include "pch.h"
#include "Bitboard.h"

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