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
private:

	/// <summary>
	/// Depth limited alpha-beta negamax algorithm
	/// </summary>
	/// <remark>Works best when it happens to test the best move first at most levels, in other words when eval function is quite good</remark>
	double AlphaBetaNegamax(Position& position, int depth, double alpha, double beta, bool maximizeWhite);

	/// <summary>
	/// Depth limited minimax algorithm
	/// </summary>
	double Minimax(Position& position, int depth, bool maximizeWhite);

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

	/// <summary>Returns true if piece is defended pieces</summary>
	static bool IsPieceDefended(const Position& position, const Piece& piece, bool isWhite);
	/// <summary>Returns true if piece is attacked</summary>
	static bool IsPieceAttacked(const Position& position, const Piece& piece, bool isWhite);
	/// <summary>Returns list of hanging pieces (at depth 0, not counting eventual tactics)</summary>
	static std::vector<const Piece*> GetHangingPieces(const Position& position);

	static int CountDoubledPawn(const Position& position, bool isWhite);

	/// <summary>
	/// Returns indices of controlled squares (0 to 64 from a1 to h8, file is first index)
	/// Flag is true if controlled by pawn
	/// </summary>
	static std::set<int> GetControlledSquares(const Position& position, const Piece& piece, bool isWhite);
	static std::set<int> GetControlledSquares(const Position& position, bool isWhite, std::set<int>& byPawn);

	static int CountCenterControlledByPawns(const std::set<int>& squares, bool isWhite);

	int m_InitialDepth = 0;

	/// <summary>
	/// Transposition table entry
	/// </summary>
	struct TranspositionTableEntry
	{
	public:
		enum class Flag
		{
			LowerBound,
			UpperBound,
			Exact
		};

		Flag m_Flag = Flag::Exact;
		int m_Depth = 0;
		double m_Score = 0.0;
	};

	std::unordered_map<Position, TranspositionTableEntry> m_TranspositionTable; //simple transposition table, store scores of positions evaluated at depth 0 to
};