#pragma once
#include <optional>
#include "Position.h"
#include <string>

class MoveMaker
{
public:
	/// <summary>
	/// Let computer make a move given a position
	/// </summary>
	/// <returns>true if move was applied, false if illegal or game is already over</returns>
	static bool MakeMove(Position& position);

	/// <summary>
	/// Make a move given a position
	/// </summary>
	/// <returns>true if move was applied, false if illegal or game is already over</returns>
	static bool MakeMove(Position& position, Position::Move& move);

	static void CheckGameOver(Position& position);

	/// <returns>Update position with given move (NO check for legal move!)</returns>
	/// <remark>white to play flag not updated, only pieces positions</remark>
	static void UpdatePosition(Position& position, const Position::Move& move);

	/// <summary>TODO: MOVE IS AMBIGUOUS RIGHT NOW!!!
	/// Translate a move (piece and its new position) to algebraic notation
	/// </summary>
	/// <param name=="move">moved piece and its new position</param>
	/// <returns>algebraic notation</returns>
	static std::string TranslateToAlgebraic(const Position::Move& move);

	/// <summary>
	/// Translate from algebraic notation (if valid move!), does not support all abbreviated notations
	/// </summary>
	static std::optional<Position::Move> TranslateFromAlgebraic(const Position& position, const std::string& moveString);

private:

	/// <returns>Best move found (a new position), invalid if StaleMate</returns>
	static std::optional<Position> FindMove(Position& position);

	static std::string TranslateToAlgebraic(Position::PieceType type);
	static std::string TranslateToAlgebraic(const std::array<int, 2>& square);
};