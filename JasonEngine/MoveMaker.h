#pragma once
#include <optional>
#include "Position.h"
#include <string>
#include "PositionEvaluation.h"
#include "MoveSearcher.h"

class MoveMaker
{
public:
	/// <summary>
	/// Let computer make a move given a position
	/// </summary>
	/// <param>Evaluation depth</param>
	/// <param=name"score">leaf position score associated to returned best move</param>
	/// <returns>true if move was applied, false if illegal or game is already over</returns>
	bool MakeMove(Position& position, int depth, double& score);

	/// <summary>
	/// Make a move given a position
	/// </summary>
	/// <returns>true if move was applied, false if illegal or game is already over</returns>
	bool MakeMove(Position& position, Move& move);

	/// <param=name"ply">ply number to retrieve generated move list in m_MoveLists ; moves will be regenerated if < 0</param>
	void CheckGameOver(Position& position, int ply = -1);

protected: //protected for testing
	static bool MovesSorter(const Position& position, const Move& move1, const Move& move2);
	void SortMoves(const Position& position, MoveList<MaxMoves>& moves);

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

		uint64_t m_ZobristHash = 0;
		Flag m_Flag = Flag::Exact;
		int m_Depth = 0;
		double m_Score = 0.0;
		Move m_BestMove;
	};

	std::array<TranspositionTableEntry, 1000> m_TranspositionTable; //simple transposition table, store scores of positions evaluated at depth 0 to, key is Zobrist hash % size

	size_t GetTranspositionTableKey(const Position& position) const;

private:
	/// <returns>True if move found, false if StaleMate</returns>
	/// <param=name"depth">Evaluation depth</param>
	/// <param=name"score">leaf position score associated to returned best move</param>
	std::optional<Move> FindMove(Position& position, int depth, double& score);

	/// <summary>Depth limited alpha-beta negamax algorithm</summary>
	/// <remark>Works best when it happens to test the best move first at most levels, in other words when eval function is quite good</remark>
	/// <remark>Returns score for maximizing player (> 0 is a good score even if maximizing black)</remark>
	double AlphaBetaNegamax(Position& position, int depth, int ply, double alpha, double beta, bool maximizeWhite, bool allowNullMove, std::optional<Move>& bestMove);

	/// <summary>Depth limited minimax algorithm, very slow, only for testing against alpha-beta negamax</summary>
	double Minimax(Position& position, int depth, bool maximizeWhite, std::optional<Move>& bestMove);

	double QuiescentSearch(Position& position, int ply, double alpha, double beta, bool maximizeWhite);

	/// <summary>Evaluate a position at depth 0</summary>
	/// <returns>Score (>0 for white advantage, <0 for black), 100.0 is value of a pawn</returns>
	/// <param=name"ply">ply number to retrieve generated move list in m_MoveLists ; moves will be regenerated if < 0</param>
	double EvaluatePosition(Position& position, int ply = -1);

	//std::array<std::pair<uint64_t, const MoveList<MaxMoves>*>, 1000> m_LegalMovesTable; //table of legal moves generation results, key is Zobrist hash % size

	///<summary>Generated lists of moves should be statically allocated, we use one such MoveList per search depth</summary>
	std::array<MoveList<MaxMoves>, MaxPly> m_MoveLists = {};
};