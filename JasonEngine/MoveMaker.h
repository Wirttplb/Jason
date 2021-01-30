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

	/// <param=name"isTerminalNode">if true, won't check for no legal modes</param>
	static void CheckGameOver(Position& position, bool isTerminalNode);
private:

	/// <returns>True if move found, false if StaleMate</returns>
	/// <param=name"depth">Evaluation depth</param>
	std::optional<Move> FindMove(Position& position, int depth);

	/// <summary>Depth limited alpha-beta negamax algorithm</summary>
	/// <remark>Works best when it happens to test the best move first at most levels, in other words when eval function is quite good</remark>
	double AlphaBetaNegamax(Position& position, int depth, double alpha, double beta, bool maximizeWhite, std::optional<Move>& bestMove);

	/// <summary>Depth limited minimax algorithm</summary>
	double Minimax(Position& position, int depth, bool maximizeWhite, std::optional<Move>& bestMove);

	/// <summary>Transposition table entry</summary>
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
		Move m_BestMove;
	};

	std::unordered_map<uint64_t, TranspositionTableEntry> m_TranspositionTable; //simple transposition table, store scores of positions evaluated at depth 0 to, key is Zobrist hash
};