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
	std::cout.setf(std::ios::unitbuf); //make sure that the outputs are sent straight away to the GUI

	//Initialization
	Position position;
	MoveMaker moveMaker;
	bool isGameOver = false;
	int evaluationDepth = 6;
	double score = 0.0;

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
						WriteToFile("noinvalid move string error");
						return 1;
					}
				}
			}			
		}
		else if (line.substr(0, 2) == "go")
		{
			size_t depthIdx = line.find("depth");
			if (depthIdx != std::string::npos)
			{
				depthIdx += 6;
				evaluationDepth = std::stoi(GetFirstWord(line.substr(depthIdx, std::string::npos)));
			}

			const bool moveFound = moveMaker.MakeMove(position, evaluationDepth, score);
			if (!moveFound)
			{
				std::cout << "no move found, game has already ended!" << std::endl;
				WriteToFile("no move found error");
				return 1;
			}

			const std::string moveString = NotationParser::TranslateToUciString(position.GetMoves().back());
			std::cout << "bestmove " << moveString << std::endl;
			std::cout << "info score cp " << static_cast<int>(score) << std::endl;
			WriteToFile("bestmove " + moveString);
			WriteToFile("info score cp " + std::to_string(score));
		}
		else if (line == "stop")
		{
			// nothing to do
		}
		else if (line == "quit")
		{
			std::cout << "Bye Bye" << std::endl;
			break;
		}
	}

	return 0;
}