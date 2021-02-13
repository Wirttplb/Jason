#pragma once
#include <stdint.h>
#include "Bitboard.h"

/// <summary>Loop over set bits from right to left, callback is called with index of set bit</summary>
static void LoopOverSetBits(const Bitboard& bitboard, void callback(int idx))
{
	uint64_t bitset = bitboard;
	while (bitset != 0)
	{
		const uint64_t t = bitset & (~bitset + 1);
		const int idx = static_cast<int>(_tzcnt_u64(bitset));
		callback(idx);
		bitset ^= t;
	}
}

static void LoopOverSetBits(const uint8_t& byte, void callback(int idx))
{
	uint8_t bitset = byte;
	while (bitset != 0)
	{
		const uint8_t t = bitset & (~bitset + 1);
		const int idx = _tzcnt_u32(bitset);
		callback(idx);
		bitset ^= t;
	}
}