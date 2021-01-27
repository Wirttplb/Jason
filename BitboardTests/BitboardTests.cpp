#include <iostream>
#include "Bitboard.h"

int main()
{
	//const Bitboard a
	//({
	//	0, 0, 0, 0, 0, 0, 0, 0,
	//	0, 0, 0, 0, 0, 0, 0, 0,
	//	0, 0, 0, 0, 0, 0, 0, 0,
	//	0, 0, 0, 0, 0, 0, 0, 0,
	//	0, 0, 0, 0, 0, 0, 0, 0,
	//	1, 0, 1, 0, 0, 0, 0, 0,
	//	1, 0, 1, 0, 0, 0, 0, 0,
	//	1, 0, 1, 0, 0, 0, 0, 0
	//});

	//Bitboard flippedA = a.FlipVertically();
	//Bitboard rotatedA = a.Rotate90Clockwise();

	//std::cout<< rotatedA.ToString();

	Bitboard a;
	a.m_Value = 4629770786759000319;
	std::cout<< a << std::endl;

	a.m_Value = 279172878336;
	std::cout << a;
}