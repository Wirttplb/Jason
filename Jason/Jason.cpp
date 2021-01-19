#include <iostream>
#include <conio.h>
#include "Position.h"
#include "MoveMaker.h"
#include "NotationParser.h"
#include <assert.h>
#include <ctime>
#include "ANWriter.h"
#include "CommandLineUtility.h"

int main()
{
    std::string jasonColorString;
    std::cout << "Jason Chess Program 1.0\n\n";
    std::cout << "Jason plays Black, White or Both? (B/W/any key)...\n\n";
    std::cin >> jasonColorString;
    std::cout << "\n";

    bool isJasonWhite = false;
    bool isJasonBlack = false;
    if (jasonColorString == "W" || jasonColorString == "w")
    {
        isJasonWhite = true;
        std::cout << "Jason plays White.\n\n";
    }
    else if (jasonColorString == "B" || jasonColorString == "b")
    {
        isJasonBlack = true;
        std::cout << "Jason plays Black.\n\n";
    }
    else
    {
        isJasonWhite = true;
        isJasonBlack = true;
        std::cout << "Jason plays both sides.\n\n";
    }

    //Initiate position
    Position position; //starting position
    MoveMaker moveMaker;
    bool isGameOver = false;
    size_t turnCount = 0;

    int evaluationDepth = 3;

    while (!isGameOver)//&& turnCount < 200)
    {
        //turnCount = (position.GetMoves().size() + 1) / 2;

        ANWriter::Write(position);

        if ((isJasonWhite && position.IsWhiteToPlay()) || (isJasonBlack && !position.IsWhiteToPlay()))
        {
            bool moveFound = moveMaker.MakeMove(position, evaluationDepth);
            assert(moveFound);
        }
        else
        {
            //ask user for move
            std::optional<Move> move;
            bool moveIsLegal = false;

            while (!move.has_value() || !moveIsLegal)
            {
                std::string inputMove;
                std::cin >> inputMove;
                move = NotationParser::TranslateFromAlgebraic(position, inputMove);
                if (move.has_value())
                {
                    moveIsLegal = moveMaker.MakeMove(position, *move);
                }

                if (!moveIsLegal)
                    EraseLastLine();
            }

            EraseLastLine();
        }

        MoveMaker::CheckGameOver(position);
        isGameOver = (position.GetGameStatus() != Position::GameStatus::Running);
        PrintLastMove(position);
    }

    switch (position.GetGameStatus())
    {
        case Position::GameStatus::StaleMate:
            std::cout << "1/2 - 1/2\n\n\n";
            break;
        case Position::GameStatus::CheckMate:
            if (position.IsWhiteToPlay())
                std::cout << "0 - 1\n\n\n";
            else
                std::cout << "1 - 0\n\n\n";
            break;
        default:
            std::cout << "ERROR! Game is still running!\n\n\n";
            break;
    }

    //ANWriter::Write(position);

    return 0;
}