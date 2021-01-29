#include "pch.h"
#include "MoveMaker.h"
#include "MoveSearcher.h"
#include "PositionEvaluation.h"
#include <string>
#include <assert.h>

bool MoveMaker::MakeMove(Position& position, int depth)
{
	bool moveFound = false;
	std::optional<Position> newPosition = FindMove(position, depth);

	if (newPosition.has_value())
	{
		position = *newPosition;
		moveFound = true;
	}

	return moveFound;
}

std::optional<Position> MoveMaker::FindMove(Position& position, int depth)
{
	std::optional<Position> newPosition;
	std::vector<Move> allLegalMoves = MoveSearcher::GetLegalMovesFromBitboards(position);

	if (allLegalMoves.empty())
		return newPosition; //Stalemate

	//position score is signed, > 0 is good for white ; < 0 good for black
	constexpr double alpha = std::numeric_limits<double>::lowest();
	constexpr double beta = std::numeric_limits<double>::max();
	std::optional<Move> bestMove;
	double score = AlphaBetaNegamax(position, depth, alpha, beta, position.IsWhiteToPlay(), bestMove);
	if (!bestMove.has_value())
	{
		assert(false);
		return newPosition;
	}

	////TEST CHECK
	//std::optional<Move> bestMove2;
	//double score2 = MoveMaker::Minimax(position, depth, position.IsWhiteToPlay(), bestMove2);
	//if ((abs(score - score2) > 0.0000001) || !(*bestMove == *bestMove2))
	//{
	//	bestMove = bestMove2;
	//}

	newPosition = position;
	newPosition->Update(*bestMove);

	//Check there is still a king!!
	const int n = position.GetWhiteKing().CountSetBits() + position.GetBlackKing().CountSetBits();
	assert(n == 2);

	return newPosition;
}

bool MoveMaker::MakeMove(Position& position, Move& move)
{
	//Check move is legal
	std::vector<Move> legalMoves = MoveSearcher::GetLegalMovesFromBitboards(position, move.m_From, position.IsWhiteToPlay());

	bool isLegal = false;
	for (const Move& legalMove : legalMoves)
	{
		if (legalMove == move)
		{
			isLegal = true;
			break;
		}
	}

	if (!isLegal)
		return false;

	position.Update(move);
	return true;
}

void MoveMaker::CheckGameOver(Position& position)
{
	//check for insufficient material
	if (position.IsInsufficientMaterialFromBitboards())
	{
		position.SetGameStatus(Position::GameStatus::StaleMate);
		return;
	}

	//Check all legal moves
	std::vector<Move> allLegalMoves = MoveSearcher::GetLegalMovesFromBitboards(position);
	if (allLegalMoves.empty())
	{
		if (MoveSearcher::IsKingInCheckFromBitboards(position, position.IsWhiteToPlay()))
			position.SetGameStatus(Position::GameStatus::CheckMate);
		else
			position.SetGameStatus(Position::GameStatus::StaleMate);
	}

	return;
}

double MoveMaker::AlphaBetaNegamax(Position& position, int depth, double alpha, double beta, bool maximizeWhite, std::optional<Move>& bestMove)
{
	const double originalAlpha = alpha;

	//Transposition table lookup
	if ((m_TranspositionTable.find(position.GetZobristHash()) != m_TranspositionTable.end()) && (m_TranspositionTable[position.GetZobristHash()].m_Depth >= depth))
	{
		switch (m_TranspositionTable[position.GetZobristHash()].m_Flag)
		{
		case TranspositionTableEntry::Flag::Exact:
		{
			bestMove = m_TranspositionTable[position.GetZobristHash()].m_BestMove;
			return m_TranspositionTable[position.GetZobristHash()].m_Score;
		}
		case TranspositionTableEntry::Flag::LowerBound:
			alpha = std::max(alpha, m_TranspositionTable[position.GetZobristHash()].m_Score);
			break;
		case TranspositionTableEntry::Flag::UpperBound:
			beta = std::min(beta, m_TranspositionTable[position.GetZobristHash()].m_Score);
			break;
		default:
			assert(false);
			break;
		}

		if (alpha >= beta)
		{
			bestMove = m_TranspositionTable[position.GetZobristHash()].m_BestMove;
			return m_TranspositionTable[position.GetZobristHash()].m_Score;
		}
	}

	if (depth == 0)
		return (maximizeWhite ? 1.0 : -1.0) * PositionEvaluation::EvaluatePosition(position);

	std::vector<Move> childMoves = MoveSearcher::GetLegalMovesFromBitboards(position);
	if (childMoves.empty())
		return (maximizeWhite ? 1.0 : -1.0) * PositionEvaluation::EvaluatePosition(position);

	double value = std::numeric_limits<double>::lowest();
	const int piecesCount = position.GetBlackPieces().CountSetBits() + position.GetWhitePieces().CountSetBits();
	for (Move& childMove : childMoves)
	{
		position.Update(childMove);
		const int piecesCount2 = position.GetBlackPieces().CountSetBits() + position.GetWhitePieces().CountSetBits();
		const int actualDepth = (piecesCount == piecesCount2) ? depth - 1 : depth -1; //go deeper when we sense a tactic (capture...) ~ Quiescent search
		std::optional<Move> bestMoveDummy; //only returns best move from 0 depth
		double score = -AlphaBetaNegamax(position, actualDepth, -beta, -alpha, !maximizeWhite, bestMoveDummy);
		position.Undo(childMove);

		if (score > value)
		{
			value = score;
			bestMove = childMove;
		}
		
		alpha = std::max(alpha, value);

		if (alpha >= beta)
			break;//cutoff
	}

	//Transposition Table Store
	m_TranspositionTable[position.GetZobristHash()].m_Score = value;
	if (value <= originalAlpha)
		m_TranspositionTable[position.GetZobristHash()].m_Flag = TranspositionTableEntry::Flag::UpperBound;
	else if (value >= beta)
		m_TranspositionTable[position.GetZobristHash()].m_Flag = TranspositionTableEntry::Flag::LowerBound;
	else
		m_TranspositionTable[position.GetZobristHash()].m_Flag = TranspositionTableEntry::Flag::Exact;

	m_TranspositionTable[position.GetZobristHash()].m_Depth = depth;
	m_TranspositionTable[position.GetZobristHash()].m_BestMove = *bestMove;

	return value;
}

double MoveMaker::Minimax(Position& position, int depth, bool maximizeWhite, std::optional<Move>& bestMove)
{
	if (depth == 0)
		return PositionEvaluation::EvaluatePosition(position);

	std::vector<Move> childMoves = MoveSearcher::GetLegalMovesFromBitboards(position);
	if (childMoves.empty())
		return PositionEvaluation::EvaluatePosition(position);

	double value = 0.0;
	if (maximizeWhite)
	{
		value = std::numeric_limits<double>::lowest();
		for (Move& childMove : childMoves)
		{
			position.Update(childMove);
			std::optional<Move> dummyBestMove;
			const double score = Minimax(position, depth - 1, false, dummyBestMove);
			position.Undo(childMove);

			if (score > value)
			{
				value = score;
				bestMove = childMove;
			}
		}

		return value;
	}
	else
	{
		value = std::numeric_limits<double>::max();
		for (Move& childMove : childMoves)
		{
			position.Update(childMove);
			std::optional<Move> dummyBestMove;
			const double score = std::min(value, Minimax(position, depth - 1, true, dummyBestMove));
			position.Undo(childMove);

			if (score < value)
			{
				value = score;
				bestMove = childMove;
			}
		}

		return value;
	}
}