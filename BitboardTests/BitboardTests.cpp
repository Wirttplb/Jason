#include <iostream>
#include "Bitboard.h"

int main()
{
	const Bitboard a
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

	Bitboard b
	({
		0, 1, 1, 1, 1, 1, 1, 1,
		0, 0, 1, 1, 1, 1, 1, 1,
		0, 0, 0, 1, 1, 1, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		//1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		//1, 1, 1, 1, 1, 1, 1, 0,
		1, 1, 1, 1, 1, 1, 1, 1,
	});

	//Bitboard flippedA = a.FlipVertically();
	//Bitboard rotatedA = a.PseudoRotate45Clockwise();

	//Bitboard unRotatedB = b;
	////unRotatedB.m_Value >>= 1;
	//unRotatedB = unRotatedB.UndoPseudoRotate45Clockwise();
	//unRotatedB.m_Value <<= 1;
	//unRotatedB.m_Value |= (1 & b);

	//std::cout<< unRotatedB.ToString() << std::endl;

	Bitboard c;
	c.m_Value = 588089187320725504;//pawns
	std::cout<< c << std::endl;

	//c.m_Value = 2310346626054553664;//2nd diag a8h1
	//std::cout << c << std::endl;

	Bitboard d
	({
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		1, 1, 1, 1, 1, 1, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0
	});

	//Bitboard unrotatedD = d.UndoPseudoRotate45Clockwise();
	//std::cout << unrotatedD.ToString() << std::endl;

	//a.m_Value = 279172878336;
	//std::cout << a;
}