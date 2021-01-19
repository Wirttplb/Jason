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
	double EvaluateMove(const Position& position, const Move& move, int depth);
private:

	/// <summary>
	/// Depth limited alpha-beta minimax algorithm
	/// </summary>
	double AlphaBeta(const Position& position, int depth, double alpha, double beta, bool maximizeWhite);

	/// <summary>
	/// Depth limited minimax algorithm
	/// </summary>
	double Minimax(const Position& position, int depth, bool maximizeWhite);

	/// <summary>
	/// Get evaluation score at depth 0, tactics won't be taken into account
	/// </summary>
	double GetScore(const Position& position);

	/// <summary>
	/// Evaluate a position at depth 0, tactics won't be taken into account
	/// </summary>
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

	std::unordered_map<Position, double> m_TranspositionTable; //simple transposition table, store scores of positions evaluated at depth 0 to
};