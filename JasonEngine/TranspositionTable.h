#pragma once
#include <memory>
#include "BasicDefinitions.h"

/// <summary>Transposition table entry</summary>
struct TranspositionTableEntry
{
public:
	enum class Flag : uint8_t
	{
		LowerBound,
		UpperBound,
		Exact
	};

	uint64_t m_ZobristHash = 0; //64 bits
	Flag m_Flag = Flag::Exact; //2 bits =>8 bits for now
	uint8_t m_Depth = 0;//8 bits
	int16_t m_Score = 0;//16 bits
	Move m_BestMove; //64 bits
	//Total: 160... compiler makes it 192 (divisible by 64) so 24 bytes... it's huuuge
};

static_assert(TranspositionTableSizeMb % sizeof(TranspositionTableEntry) == 0);

/// <summary>Transposition table</summary>
class TranspositionTable
{
public:
	TranspositionTable() { m_Table.reset(new TT); };
	TranspositionTableEntry& operator[](size_t idx) { return (*m_Table)[idx]; };
	size_t size() const { return (*m_Table).size(); };
private:
	typedef std::array<TranspositionTableEntry, TranspositionTableSize> TT;
	std::unique_ptr <std::array<TranspositionTableEntry, TranspositionTableSize>> m_Table;
};