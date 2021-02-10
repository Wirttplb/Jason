#pragma once
#include "Position.h"
#include <iostream>
#include <fstream>

/// <summary>
/// Algebraic notation writer for pgn import (very basic, only unabbreviated moves)
/// </summary>
class ANWriter
{
public:
	static void Write(const Position& position)
	{
		std::ofstream myfile;
		myfile.open("D:\\work\\Jason\\Jason\\Debug\\ANTest.txt");

		std::vector<Move> moves = position.GetMovesVector();
		for (const Move& move : moves)
		{
			myfile << NotationParser::TranslateToAlgebraic(move) << " ";
		}

		myfile.close();
	}
};