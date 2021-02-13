#include <iostream>
#include <string>
#include "MoveMaker.h"
#include "NotationParser.h"
#include <fstream>

static std::string GetLastWord(const std::string& s)
{
	size_t index = s.find_last_of(' ');
	return s.substr(++index);
}

static std::string GetFirstWord(const std::string& s)
{
	size_t index = s.find_first_of(' ');
	return s.substr(0, index);
}

//Log everything for debugging
static void WriteToFile(const std::string& s)
{
	std::ofstream myfile;
	myfile.open("C:\\SOURCE\\uciTest.txt", std::ios_base::app);
	myfile << s << std::endl;

	myfile.close();
}

std::vector<std::string> SplitString(const std::string& str, const std::string& delim)
{
	std::vector<std::string> tokens;
	size_t prev = 0, pos = 0;
	do
	{
		pos = str.find(delim, prev);
		if (pos == std::string::npos)
			pos = str.length();

		std::string token = str.substr(prev, pos - prev);
		if (!token.empty())
			tokens.push_back(token);

		prev = pos + delim.length();
	} while (pos < str.length() && prev < str.length());
	
	return tokens;
}

int main()
{
	std::string line;
	std::cout.setf(std::ios::unitbuf); //make sure that the outputs are sent straight away

	//Initialization
	Position position;
	MoveMaker moveMaker;
	bool isGameOver = false;
	int evaluationDepth = 6;
	int score = 0;

	while (std::getline(std::cin, line))
	{
		WriteToFile(line);

		if (line == "uci")
		{
			std::cout << "id name Jason" << std::endl;
			std::cout << "id Romain Fournet" << std::endl;
			std::cout << "uciok" << std::endl;
		}
		else if (line == "isready")
		{
			std::cout << "readyok" << std::endl;
		}
		else if (line == "ucinewgame")
		{
			position = Position();
		}
		else if (line.substr(0, 8) == "position")
		{
			if (line.substr(0, 12) == "position fen")
			{
				const std::string fenString = line.substr(12, std::string::npos);
				position = Position(fenString);
			}
			else if (line.substr(0, 17) == "position startpos")
			{
				position = Position();
			}

			size_t movesIdx = line.find("moves");
			if (movesIdx != std::string::npos)
			{
				movesIdx += 6;
				std::vector<std::string> moves = SplitString(line.substr(movesIdx, std::string::npos), " ");

				for (const std::string& moveString : moves)
				{
					std::optional<Move> move = NotationParser::TranslateFromUci(position, moveString);
					if (move.has_value() && !move->IsNullMove())
						position.Update(*move);
					else
					{
						std::cout << "invalid move string!" << std::endl;
						return 1;
					}
				}
			}			
		}
		else if (line.substr(0, 2) == "go")
		{
			double wtime = 3600.0;
			size_t wtimeIdx = line.find("wtime");
			if (wtimeIdx != std::string::npos)
			{
				wtimeIdx += 6;
				wtime = std::stod(GetFirstWord(line.substr(wtimeIdx, std::string::npos))) * 0.001; //ms to s
			}

			double btime = 3600.0;
			size_t btimeIdx = line.find("btime");
			if (btimeIdx != std::string::npos)
			{
				btimeIdx += 6;
				btime = std::stod(GetFirstWord(line.substr(btimeIdx, std::string::npos))) * 0.001;
			}
			
			const double maxTime = position.IsWhiteToPlay() ? wtime : btime;
			const bool moveFound = moveMaker.MakeMove(maxTime, position, evaluationDepth, score);
			if (!moveFound)
			{
				std::cout << "no move found, game has already ended!" << std::endl;
				return 1;
			}

			const std::string moveString = NotationParser::TranslateToUciString(position.GetMoves().back());
			std::cout << "bestmove " << moveString << std::endl;

			std::cout << "info score cp " << static_cast<int>(score);
			std::optional<int> mateCount = PositionEvaluation::GetMovesToMate(score);
			if (mateCount.has_value())
				std::cout << " mate " << *mateCount;
			std::cout << std::endl;
		}
		else if (line == "stop")
		{
			// nothing to do
		}
		else if (line == "quit")
		{
			std::cout << "Bye" << std::endl;
			break;
		}
	}

	return 0;
}