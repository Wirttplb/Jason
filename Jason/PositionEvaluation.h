#pragma once
#include "Position.h"

class PositionEvaluation
{
public:
	/// <summary>
	/// Evaluate a position, with given depth
	/// </summary>
	/// <param name="position"></param>
	/// <returns>Score (>0 for white advantage, <0 for black)</returns>
	static double EvaluatePosition(const Position& position, int depth);
private:
	/// <summary>
	/// Evaluate a position, depth 0
	/// </summary>
	/// <param name="position"></param>
	/// <returns>Score (>0 for white advantage, <0 for black)</returns>
	static double EvaluatePosition(const Position& position);

	/// <summary>
	/// Returns value in points of a given piece
	/// </summary>
	/// <returns></returns>
	static double GetPieceValue(Position::PieceType type);

	static int CountDoubledPawn(const Position& position, bool isWhite);
};