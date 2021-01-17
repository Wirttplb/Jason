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
	static bool MakeMove(Position& position, Move& move);

	static void CheckGameOver(Position& position);

	/// <returns>Update position with given move (NO check for legal move!)</returns>
	/// <remark>white to play flag not updated, only pieces positions</remark>
	static void UpdatePosition(Position& position, const Move& move);

private:

	/// <returns>Best move found (a new position), invalid if StaleMate</returns>
	static std::optional<Position> FindMove(Position& position);
};