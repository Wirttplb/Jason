#include <iostream>
#include "Bitboard.h"

int main()
{
    Bitboard aFile
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

	const Bitboard a1
	({
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0
	});

	Bitboard a;
	a.m_Value = 8657043458;// 1152921504606846976;// 329728;// 391167 329744;

	Bitboard b;
	b.m_Value = 1099511758845;

	//((WhiteKingCastleInBetweenSquares & (friendlyPieces | enemyPieces)).m_Value == 0)
	Bitboard d;
	d.m_Value = 65536;

	std::cout << a << std::endl << std::endl;
	std::cout << b;
	//std::cout << std::endl << std::endl;
	//std::cout << (_f1 | _g1);
	std::cout << std::endl << std::endl;
	//std::cout << (a | b);
	std::cout << d;
	//std::cout << b;
}