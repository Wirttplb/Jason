#pragma once
#include "Position.h"
#include <set>

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
	static double GetPieceValue(PieceType type);

	static int CountDoubledPawn(const Position& position, bool isWhite);

	/// <summary>
	/// Returns indices of controlled squares (0 to 64 from a1 to h8, file is first index)
	/// Flag is true if controlled by pawn
	/// </summary>
	static std::set<int> GetControlledSquares(const Position& position, const Piece& piece, bool isWhite);
	static std::set<int> GetControlledSquares(const Position& position, bool isWhite, std::set<int>& byPawn);

	static int CountCenterControlledByPawns(const std::set<int>& squares, bool isWhite);
};