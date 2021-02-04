#include "pch.h"
#include "MoveMaker.h"
#include "PositionEvaluation.h"
#include <string>
#include <assert.h>
#include <algorithm> 

bool MoveMaker::MakeMove(Position& position, int depth)
{
	std::optional<Move> bestMove = FindMove(position, depth);

	if (bestMove.has_value())
		position.Update(*bestMove);

	return bestMove.has_value();
}

std::optional<Move> MoveMaker::FindMove(Position& position, int depth)
{
	constexpr double alpha = std::numeric_limits<double>::lowest();
	constexpr double beta = std::numeric_limits<double>::max();
	std::optional<Move> bestMove;

	//Minimax(position, depth, position.IsWhiteToPlay(), bestMove);
	//m_TranspositionTable.clear();
	double score = AlphaBetaNegamax(position, depth, depth, alpha, beta, position.IsWhiteToPlay(), bestMove);
	score;

	return bestMove;
}

bool MoveMaker::MakeMove(Position& position, Move& move)
{
	//Check move is legal
	std::vector<Move> legalMoves = MoveSearcher::GetLegalMovesFromBitboards(position, move.GetFromType(), move.GetFromSquare(), position.IsWhiteToPlay());

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

	//Get and check legal moves > 0
	const size_t tableKey = MoveMaker::GetTranspositionTableKey(position);
	if (m_LegalMovesTable[tableKey].first != position.GetZobristHash())
	{
		m_LegalMovesTable[tableKey].first = position.GetZobristHash();
		m_LegalMovesTable[tableKey].second = MoveSearcher::GetLegalMovesFromBitboards(position);
	}

	std::vector<Move> allLegalMoves = m_LegalMovesTable[tableKey].second;
	if (allLegalMoves.empty())
	{
		if (MoveSearcher::IsKingInCheckFromBitboards(position, position.IsWhiteToPlay()))
			position.SetGameStatus(Position::GameStatus::CheckMate);
		else
			position.SetGameStatus(Position::GameStatus::StaleMate);
	}
}

double MoveMaker::AlphaBetaNegamax(Position& position, int initialDepth, int depth, double alpha, double beta, bool maximizeWhite, std::optional<Move>& bestMove)
{
	const double originalAlpha = alpha;

	//Transposition table lookup
	size_t transpositionTableKey = GetTranspositionTableKey(position);
	if ((m_TranspositionTable[transpositionTableKey].m_ZobristHash == position.GetZobristHash()) &&
		(m_TranspositionTable[transpositionTableKey].m_Depth >= depth))
	{
		switch (m_TranspositionTable[transpositionTableKey].m_Flag)
		{
		case TranspositionTableEntry::Flag::Exact:
		{
			//if (initialDepth == depth)
			//{
			bestMove = m_TranspositionTable[transpositionTableKey].m_BestMove;
			return m_TranspositionTable[transpositionTableKey].m_Score;
			//}
		}
		case TranspositionTableEntry::Flag::LowerBound:
			alpha = std::max(alpha, m_TranspositionTable[transpositionTableKey].m_Score);
			break;
		case TranspositionTableEntry::Flag::UpperBound:
			beta = std::min(beta, m_TranspositionTable[transpositionTableKey].m_Score);
			break;
		default:
			assert(false);
			break;
		}

		if (alpha >= beta)
		{
			bestMove = m_TranspositionTable[transpositionTableKey].m_BestMove;
			return m_TranspositionTable[transpositionTableKey].m_Score;
		}
	}

	if (depth == 0)
	{
		//Last 2 depth as max depth
		//int quiescentSearchMaxDepth = 0;
		//return QuiescentSearch(position, quiescentSearchMaxDepth, alpha, beta, maximizeWhite);
		return (maximizeWhite ? 1.0 : -1.0) * EvaluatePosition(position);
	}

	//Don't search valid moves again if it has been done in last iteration
	//There should be no collision otherwise childMoves may become invalid after next iterations
	if (m_LegalMovesTable[transpositionTableKey].first != position.GetZobristHash())
	{
		m_LegalMovesTable[transpositionTableKey].first = position.GetZobristHash();
		m_LegalMovesTable[transpositionTableKey].second = MoveSearcher::GetLegalMovesFromBitboards(position);
	}
	std::vector<Move> childMoves = m_LegalMovesTable[transpositionTableKey].second;

	if (childMoves.empty())
		return (maximizeWhite ? 1.0 : -1.0) * EvaluatePosition(position);

	//Sort moves
	SortMoves(position, childMoves);

	//Search child nodes
	double value = std::numeric_limits<double>::lowest();
	for (Move& childMove : childMoves)
	{
		position.Update(childMove);
		std::optional<Move> bestMoveDummy; //only returns best move from 0 depth
		double score = -AlphaBetaNegamax(position, initialDepth, depth - 1, -beta, -alpha, !maximizeWhite, bestMoveDummy);
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
	m_TranspositionTable[transpositionTableKey].m_ZobristHash = position.GetZobristHash();
	m_TranspositionTable[transpositionTableKey].m_Score = value;
	m_TranspositionTable[transpositionTableKey].m_Depth = depth;
	m_TranspositionTable[transpositionTableKey].m_BestMove = *bestMove;
	if (value <= originalAlpha)
		m_TranspositionTable[transpositionTableKey].m_Flag = TranspositionTableEntry::Flag::UpperBound;
	else if (value >= beta)
		m_TranspositionTable[transpositionTableKey].m_Flag = TranspositionTableEntry::Flag::LowerBound;
	else
		m_TranspositionTable[transpositionTableKey].m_Flag = TranspositionTableEntry::Flag::Exact;

	return value;
}

double MoveMaker::Minimax(Position& position, int depth, bool maximizeWhite, std::optional<Move>& bestMove)
{
	if (depth == 0)
		return EvaluatePosition(position);

	std::vector<Move> childMoves = MoveSearcher::GetLegalMovesFromBitboards(position);
	if (childMoves.empty())
		return EvaluatePosition(position);

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

double MoveMaker::QuiescentSearch(Position& position, int depth, double alpha, double beta, bool maximizeWhite)
{
	double score = (maximizeWhite ? 1.0 : -1.0) * EvaluatePosition(position);

	if (depth == 0)
		return score;
	if (score >= beta)
		return beta;
	if (alpha < score)
		alpha = score;

	size_t transpositionTableKey = GetTranspositionTableKey(position);
	if (m_LegalMovesTable[transpositionTableKey].first != position.GetZobristHash())
	{
		m_LegalMovesTable[transpositionTableKey].first = position.GetZobristHash();
		m_LegalMovesTable[transpositionTableKey].second = MoveSearcher::GetLegalMovesFromBitboards(position);
	}
	std::vector<Move> childMoves = m_LegalMovesTable[transpositionTableKey].second;
	
	//Sort moves
	SortMoves(position, childMoves);

	for (Move& move : childMoves)
	{
		if (move.IsCapture())
		{
			position.Update(move);
			double score = -QuiescentSearch(position, depth - 1, -beta, -alpha, !maximizeWhite);
			position.Undo(move);

			if (score >= beta)
				return beta;
			if (score > alpha)
				alpha = score;
		}
	}

	return alpha;
}

double MoveMaker::EvaluatePosition(Position& position)
{
	MoveMaker::CheckGameOver(position);
	const double score = PositionEvaluation::EvaluatePosition(position);
	position.SetGameStatus(Position::GameStatus::Running);
	return score;
}

void MoveMaker::SortMoves(const Position& position, std::vector<Move>& moves)
{
	std::sort(moves.begin(), moves.end(), [&position](const Move& move1, const Move& move2)->bool { return MovesSorter(position, move1, move2); });
	
	//Killer Moves heuristic to add...

	//Best move from previous iteration is picked as best guess
	const size_t transpositionTableKey = GetTranspositionTableKey(position);
	if (m_TranspositionTable[transpositionTableKey].m_ZobristHash == position.GetZobristHash())
	{
		const Move& previousBest = m_TranspositionTable[transpositionTableKey].m_BestMove;
		std::vector<Move>::iterator searchIt = std::find(moves.begin(), moves.end(), previousBest);
		if (searchIt != moves.end())
		{
			const size_t searchIdx = searchIt - moves.begin();
			if (searchIdx > 0)
			{
				std::rotate(moves.begin(), moves.begin() + searchIdx - 1, moves.begin() + searchIdx);
				std::swap(moves.front(), moves[searchIdx]);
			}
		}
	}
}

static constexpr std::array<PieceType, 5> PieceTypesSorted = { PieceType::Queen, PieceType::Rook, PieceType::Bishop, PieceType::Knight, PieceType::Pawn};

bool MoveMaker::MovesSorter(const Position& position, const Move& move1, const Move& move2)
{
	//Most Valuable Victim - Least Valuable Aggressor heuristic
	Bitboard toSquare1(move1.GetToSquare());
	Bitboard toSquare2(move2.GetToSquare());
	const Bitboard& enemyPieces = (position.IsWhiteToPlay() ? position.GetBlackPieces() : position.GetWhitePieces());
	if ((toSquare1 & enemyPieces) && !(toSquare2 & enemyPieces))
		return true;

	static constexpr std::array<PieceType, 5> PieceTypesSorted = { PieceType::Queen, PieceType::Rook, PieceType::Bishop, PieceType::Knight, PieceType::Pawn };
	for (PieceType type : PieceTypesSorted)
	{
		const Bitboard& enemyPieces = position.GetPiecesOfType(type, !position.IsWhiteToPlay());
		if (toSquare1 & enemyPieces)
		{
			if (!(toSquare2 & enemyPieces) || (move1.GetFromType() < move2.GetFromType()))
				return true;
		}
		else if (toSquare2 & enemyPieces)
			return false;
	}

	return false;
}

size_t MoveMaker::GetTranspositionTableKey(const Position& position) const
{
	return (position.GetZobristHash() % m_TranspositionTable.size());
}
