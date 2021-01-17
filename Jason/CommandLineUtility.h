#pragma once

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
    const std::vector<Move>& moves = position.GetMoves();
    if (moves.empty())
    {
        assert(position.GetMoves().empty());
        return;
    }

    if (!position.IsWhiteToPlay())
    {
        const std::string whiteMoveString = NotationParser::TranslateToAlgebraic(moves.back());
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
        const std::string whiteMoveString = NotationParser::TranslateToAlgebraic(*(moves.end() - 2));
        const std::string blackMoveString = NotationParser::TranslateToAlgebraic(*(moves.end() - 1));
        std::cout << (moves.size() + 1) / 2 << ". " + whiteMoveString;
        std::cout << ", " + blackMoveString;
        if (position.GetGameStatus() == Position::GameStatus::CheckMate)
            std::cout << "#";
        std::cout << "\n";
    }
}
