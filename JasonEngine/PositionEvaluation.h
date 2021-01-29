#pragma once
#include "Position.h"
#include <unordered_map>
#include <set>
#include <variant>


class PositionEvaluation
{
public:
	/// <summary>
	/// Evaluate a position, with given depth
	/// </summary>
	/// <returns>Score (>0 for white advantage, <0 for black)</returns>
	/// <remark>score will be "worst" for odd depth and "best" for even depth from player's point of view,
	/// following maximin decision rule</remark>
	/// <remark>Search and evaluation is done directly on position which is constantly updated (no copy)</remark>
	double EvaluateMove(Position& position, Move& move, int depth);

	/// <summary>
	/// Evaluate a position at depth 0, tactics won't be taken into account
	/// </summary>
	/// <returns>Score (>0 for white advantage, <0 for black)</returns>
	static double EvaluatePosition(const Position& position);
private:

	/// <summary>
	/// Depth limited minimax algorithm
	/// </summary>
	double Minimax(Position& position, int depth, bool maximizeWhite);

	static double CountMaterial(const Position& position, bool isWhite);

	/// <summary>
	/// Returns value in points of a given piece
	/// </summary>
	/// <returns></returns>
	static double GetPieceValue(PieceType type);

	static int CountDoubledPawns(const Position& position, bool isWhite);
	static int CountCenterPawns(const Position& position, bool isWhite);

	/// <summary>
	/// Returns indices of controlled squares (0 to 64 from a1 to h8, file is first index)
	/// Flag is true if controlled by pawn
	/// </summary>
	static std::set<int> GetControlledSquares(const Position& position, const Piece& piece, bool isWhite);
	static std::set<int> GetControlledSquares(const Position& position, bool isWhite, std::set<int>& byPawn);
	static Bitboard GetControlledSquares(const Position& position, bool isWhite);

	static int CountCenterControlledByPawns(const std::set<int>& squares, bool isWhite);
};