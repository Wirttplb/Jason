#pragma once
#include <optional>
#include "Position.h"
#include <string>
#include "PositionEvaluation.h"
#include "MoveSearcher.h"
#include "TranspositionTable.h"
#include "TimeManager.h"

class MoveMaker
{
public:
	/// <summary>Let computer make a move given a position</summary>
	/// <param name="time">max time to use for move, in seconds</param>
	/// <param name="isMoveTime">if true, time is time for move, otherwise it is the remaining total time for the game</param>
	/// <param name="increment">time increment per move, for time management</param>
	/// <param name="maxDepth">Maximum evaluation depth</param>
	/// <param name="score">leaf position score associated to returned best move</param>
	/// <returns>true if move was applied, false if illegal or game is already over</returns>
	bool MakeMove(double moveTime, bool isMoveTime, double increment, Position& position, int maxDepth, int& score, int& searchDepth);
	bool MakeMove(Position& position, int maxDepth, int& score);

	/// <summary>Make a move given a position</summary>
	/// <returns>true if move was applied, false if illegal or game is already over</returns>
	bool MakeMove(Position& position, Move& move);

	/// <param=name"ply">ply number to retrieve generated move list in m_MoveLists ; moves will be regenerated if < 0</param>
	void CheckGameOver(Position& position, int ply = -1);

protected: //protected for testing
	bool MovesSorter(const Position& position, int ply, const Move& move1, const Move& move2);
	void SortMoves(const Position& position, int ply, MoveList<MaxMoves>& moves);
	
	TranspositionTable m_TranspositionTable; //simple transposition table, store scores of positions evaluated at depth 0 to, key is Zobrist hash % size

	size_t GetTranspositionTableKey(const Position& position) const;

private:
	/// <returns>True if move found, false if StaleMate</returns>
	/// <param=name"maxDepth">max evaluation depth</param>
	/// <param=name"score">leaf position score associated to returned best move</param>
	std::optional<Move> FindMove(Position& position, int maxDepth, int& score, int& searchDepth);

	/// <summary>Depth limited alpha-beta negamax algorithm</summary>
	/// <remark>Works best when it happens to test the best move first at most levels, in other words when eval function is quite good</remark>
	/// <remark>Returns subjective score for maximizing player (> 0 is a good score even if maximizing black)</remark>
	int Search(Position& position, int depth, int ply, int alpha, int beta, bool maximizeWhite, bool allowNullMove, std::optional<Move>& bestMove);

	/// <summary>Depth limited minimax algorithm, very slow, only for testing against alpha-beta negamax</summary>
	int Minimax(Position& position, int depth, bool maximizeWhite, std::optional<Move>& bestMove);

	int QuiescentSearch(Position& position, int ply, int alpha, int beta, bool maximizeWhite);

	/// <summary>Static evaluation of a position at depth 0</summary>
	/// <returns>Score (>0 for white advantage, <0 for black), in centipawns</returns>
	/// <param=name"ply">ply number to retrieve generated move list in m_MoveLists ; moves will be regenerated if < 0</param>
	int EvaluatePosition(Position& position, int ply = -1);

	std::array<std::array<Move, NbOfKillerMoves>, MaxPly> m_KillerMoves = {};

	///<summary>Generated lists of moves should be statically allocated, we use one such MoveList per search depth</summary>
	std::array<MoveList<MaxMoves>, MaxPly> m_MoveLists = {};

	TimeManager m_TimeManager;
};