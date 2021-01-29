#pragma once
#include "Position.h"
#include <unordered_map>
#include <set>
#include <variant>


class PositionEvaluation
{
public:
	/// <summary>
	/// Evaluate a position at depth 0, tactics won't be taken into account
	/// </summary>
	/// <returns>Score (>0 for white advantage, <0 for black), 100.0 is value of a pawn</returns>
	static double EvaluatePosition(const Position& position);

private:
	static double CountMaterial(const Position& position, bool isWhite);

	/// <summary> Returns value in points of a given piece </summary>
	static constexpr double GetPieceValue(PieceType type);

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

	/// <param name="attackedSquares">attacked squares by opposing color</param>
	static Bitboard GetAttackedSquaresAroundKing(const Position& position, const Bitboard& attackedSquares, bool isWhiteKing);

	/// <returns>Return number of rooks on open files and semi open files</returns>
	static std::pair<int, int> CountRooksOnOpenFiles(const Position& position, bool isWhite);
};