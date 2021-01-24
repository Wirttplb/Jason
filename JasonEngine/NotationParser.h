#pragma once
#include <string>
#include "Position.h"

class NotationParser
{
public:
	/// <summary>TODO: MOVE IS AMBIGUOUS RIGHT NOW!!!
	/// Translate a move (piece and its new position) to algebraic notation
	/// </summary>
	/// <param name=="move">moved piece and its new position</param>
	/// <returns>algebraic notation</returns>
	static std::string TranslateToAlgebraic(const Move& move);

	/// <summary>
	/// Translate from algebraic notation (if valid move!), does not support all abbreviated notations
	/// </summary>
	static std::optional<Move> TranslateFromAlgebraic(const Position& position, const std::string& moveString);

	/// <summary>
	/// Generate position from fen notation string
	/// </summary>
	static void TranslateFEN(const std::string& fen, Position& position);

private:

	static std::string TranslateToAlgebraic(PieceType type);
	static std::string TranslateToAlgebraic(Square square);
};