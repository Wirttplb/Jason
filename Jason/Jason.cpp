#include <iostream>
#include <conio.h>
#include "Position.h"
#include "MoveMaker.h"
#include <assert.h>
#include <ctime>
#include "ANWriter.h"

static void EraseLastLine()
{
    printf("\033[A"); //move cursor up to last written line
    printf("\33[2K"); //erase line
    std::cout << "\r"; //carriage return
}

static void MoveCursorUp()
{
    printf("\033[A");
}

static void PrintLastMove(const Position& position)
{
    const std::vector<Position::Move>& moves = position.GetMoves();
    if (moves.empty())
    {
        assert(position.GetMoves().empty());
        return;
    }

    if (!position.IsWhiteToPlay())
    {
        const std::string whiteMoveString = MoveMaker::TranslateToAlgebraic(moves.back());
        std::cout << (moves.size() + 1) / 2 << ". " + whiteMoveString;
        if (position.GetGameStatus() == Position::GameStatus::CheckMate)
            std::cout << "#";
        std::cout << "\n";
    }
    else
    {
        if (moves.size() < 2)
        {
            assert(false);
            return;
        }

        EraseLastLine();
        const std::string whiteMoveString = MoveMaker::TranslateToAlgebraic(*(moves.end() - 2));
        const std::string blackMoveString = MoveMaker::TranslateToAlgebraic(*(moves.end() - 1));
        std::cout << (moves.size() + 1) / 2 << ". " + whiteMoveString;
        std::cout << ", " + blackMoveString;
        if (position.GetGameStatus() == Position::GameStatus::CheckMate)
            std::cout << "#";
        std::cout << "\n";
    }
}

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
    bool isGameOver = false;
    srand(2);

    while (!isGameOver)//&& turnCount < 200)
    {
        int turnCount = (position.GetMoves().size() + 1) / 2;
        int b = 5;
        if (turnCount == 152)
        {
            int a = 1;
            a;
            b += a;
            //ahah;
        }

        ANWriter::Write(position);

        if ((isJasonWhite && position.IsWhiteToPlay()) || (isJasonBlack && !position.IsWhiteToPlay()))
        {
            bool moveFound = MoveMaker::MakeMove(position);
            assert(moveFound);
        }
        else
        {
            //ask user for move
            std::optional<Position::Move> move;
            bool moveIsLegal = false;

            while (!move.has_value() || !moveIsLegal)
            {
                std::string inputMove;
                std::cin >> inputMove;
                move = MoveMaker::TranslateFromAlgebraic(position, inputMove);
                if (move.has_value())
                {
                    moveIsLegal = MoveMaker::MakeMove(position, *move);
                }

                if (!moveIsLegal)
                    EraseLastLine();
            }

            EraseLastLine();
        }

        isGameOver = MoveMaker::CheckGameOver(position);
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