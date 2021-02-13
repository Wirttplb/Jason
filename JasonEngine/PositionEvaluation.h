#pragma once
#include "Position.h"

class PositionEvaluation
{
public:
	/// <summary>
	/// Evaluate a position at depth 0, tactics won't be taken into account
	/// </summary>
	/// <returns>Score (>0 for white advantage, <0 for black), 100.0 is value of a pawn</returns>
	static int EvaluatePosition(Position& position, int ply);

	static bool IsPositionQuiet(const Position& position);

	/// <returns>Number of moves (not plies) to mate</returns>
	static std::optional<int> GetMovesToMate(int score);

private:
	static int CountMaterial(const Position& position, bool isWhite);

	/// <summary> Returns value in points of a given piece </summary>
	static constexpr int GetPieceValue(PieceType type);

	/// <returns>Punishment based on pieces dwelling on starting squares ; should not be applied during endgame</returns>
	static int GetUndevelopedPiecesPunishment(const Position& position, bool isWhite);

	static int CountDoubledPawns(const Position& position, bool isWhite);
	static int CountCenterPawns(const Position& position, bool isWhite);
	static int CountIsolatedPawns(const Position& position, bool isWhite);
	static int CountBackwardsPawns(const Position& position, bool isWhite);
	static int GeAdvancedPawnsBonus(const Position& position, bool isWhite);
	static int CountBlockedEorDPawns(const Position& position, bool isWhite);

	static Bitboard GetAttackedSquares(Position& position, bool isWhite);

	/// <param name="attackedSquares">attacked squares by opposing color</param>
	static Bitboard GetAttackedSquaresAroundKing(const Position& position, const Bitboard& attackedSquares, bool isWhiteKing);

	/// <returns>Return number of rooks on open files and semi open files</returns>
	static std::pair<int, int> CountRooksOnOpenFiles(const Position& position, bool isWhite);

	friend class PositionEvaluationTests;
};