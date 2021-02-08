#pragma once
#include <string>
#include "Position.h"

class NotationParser
{
public:
	/// <summary>
	/// Translate a move to algebraic notation
	/// </summary>
	/// <returns>algebraic notation</returns>
	static std::string TranslateToAlgebraic(const Move& move);

	/// <summary>
	/// Translate a move to uci move notation
	/// </summary>
	/// <returns>algebraic notation</returns>
	static std::string TranslateToUciString(const Move& move);

	/// <summary>
	/// Translate from algebraic notation (if valid move!), does not support all abbreviated notations
	/// </summary>
	/// <param name="isUciString">if true, string will be treated as a uci move (size 4 or 5 string)</param>
	static std::optional<Move> TranslateFromAlgebraic(const Position& position, const std::string& moveString, bool isUciString = false);

	/// <summary>
	/// Translate from uci move notation (if valid move!)
	/// </summary>
	static std::optional<Move> TranslateFromUci(const Position& position, const std::string& moveString);

	/// <summary>
	/// Generate position from fen notation string
	/// </summary>
	static void TranslateFEN(const std::string& fen, Position& position);

private:

	static std::string TranslateToAlgebraic(PieceType type);
	static std::string TranslateToAlgebraic(Square square);
};