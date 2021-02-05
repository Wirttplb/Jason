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
	static double EvaluatePosition(Position& position);

	static bool IsPositionQuiet(const Position& position);

private:
	static double CountMaterial(const Position& position, bool isWhite);

	/// <summary> Returns value in points of a given piece </summary>
	static constexpr double GetPieceValue(PieceType type);

	static int CountDoubledPawns(const Position& position, bool isWhite);
	static int CountCenterPawns(const Position& position, bool isWhite);
	static int CountIsolatedPawns(const Position& position, bool isWhite);
	static double GeAdvancedPawnsBonus(const Position& position, bool isWhite);
	static int CountBlockedEorDPawns(const Position& position, bool isWhite);

	static Bitboard GetControlledSquares(Position& position, bool isWhite);

	/// <param name="attackedSquares">attacked squares by opposing color</param>
	static Bitboard GetAttackedSquaresAroundKing(const Position& position, const Bitboard& attackedSquares, bool isWhiteKing);

	/// <returns>Return number of rooks on open files and semi open files</returns>
	static std::pair<int, int> CountRooksOnOpenFiles(const Position& position, bool isWhite);
};