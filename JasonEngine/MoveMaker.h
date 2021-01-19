#pragma once
#include <optional>
#include "Position.h"
#include <string>
#include "PositionEvaluation.h"

class MoveMaker
{
public:
	/// <summary>
	/// Let computer make a move given a position
	/// </summary>
	/// <param>Evaluation depth</param>
	/// <returns>true if move was applied, false if illegal or game is already over</returns>
	bool MakeMove(Position& position, int depth);

	/// <summary>
	/// Make a move given a position
	/// </summary>
	/// <returns>true if move was applied, false if illegal or game is already over</returns>
	bool MakeMove(Position& position, Move& move);

	static void CheckGameOver(Position& position);
private:

	/// <returns>Best move found (a new position), invalid if StaleMate</returns>
	/// <param>Evaluation depth</param>
	std::optional<Position> FindMove(Position& position, int depth);

	PositionEvaluation m_PositionEvaluator; //will store transposition table
};