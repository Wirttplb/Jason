#include "pch.h"
#include "MoveMaker.h"
#include "PositionEvaluation.h"
#include <string>
#include <assert.h>
#include <algorithm>

bool MoveMaker::MakeMove(Position& position, int depth, double& score)
{
	std::optional<Move> bestMove = FindMove(position, depth, score);

	if (bestMove.has_value())
		position.Update(*bestMove);

	return bestMove.has_value();
}

std::optional<Move> MoveMaker::FindMove(Position& position, int depth, double& score)
{
	constexpr double alpha = std::numeric_limits<double>::lowest();
	constexpr double beta = std::numeric_limits<double>::max();
	std::optional<Move> bestMove;
	const bool allowNullMove = true;
	m_KillerMoves = {};

	score = (position.IsWhiteToPlay() ? 1.0 : -1.0) * AlphaBetaNegamax(position, depth, 0, alpha, beta, position.IsWhiteToPlay(), allowNullMove, bestMove);
		
	return bestMove;
}

bool MoveMaker::MakeMove(Position& position, Move& move)
{
	//Check move is legal
	MoveList<MaxMoves>& legalMoves = m_MoveLists[0];
	MoveSearcher::GetLegalMovesFromBitboards(position, move.GetFromType(), move.GetFromSquare(), position.IsWhiteToPlay(), legalMoves);

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

void MoveMaker::CheckGameOver(Position& position, int ply)
{
	//check insufficient material or repetition
	if (position.IsInsufficientMaterialFromBitboards() || (position.GetHistoryCount() >= 3))
	{
		position.SetGameStatus(Position::GameStatus::Draw);
		return;
	}

	//Get and check legal moves > 0
	/*const size_t tableKey = MoveMaker::GetTranspositionTableKey(position);
	if (m_LegalMovesTable[tableKey].first != position.GetZobristHash())
	{
		m_LegalMovesTable[tableKey].first = position.GetZobristHash();
		MoveSearcher::GetLegalMovesFromBitboards(position, m_LegalMovesTable[tableKey].second);
	}*/

	if (ply < 0)
		MoveSearcher::GetLegalMovesFromBitboards(position, m_MoveLists[ply]);

	MoveList<MaxMoves>& childMoves = m_MoveLists[ply];
	MoveSearcher::GetLegalMovesFromBitboards(position, childMoves);

	//const MoveList<MaxMoves>& allLegalMoves = m_LegalMovesTable[tableKey].second;
	if (childMoves.empty())
	{
		if (MoveSearcher::IsKingInCheckFromBitboards(position, position.IsWhiteToPlay()))
			position.SetGameStatus(Position::GameStatus::CheckMate);
		else
			position.SetGameStatus(Position::GameStatus::Draw);
	}
}

double MoveMaker::AlphaBetaNegamax(Position& position, int depth, int ply, double alpha, double beta, bool maximizeWhite, bool allowNullMove, std::optional<Move>& bestMove)
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
			//Repetition would affect the score, can't use it directly
			if (position.GetHistoryCount() < 2)
			{
				bestMove = m_TranspositionTable[transpositionTableKey].m_BestMove;
				return m_TranspositionTable[transpositionTableKey].m_Score;
			}

			break;
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

		if (m_TranspositionTable[transpositionTableKey].m_Flag != TranspositionTableEntry::Flag::Exact)
		{
			if (alpha >= beta)
			{
				if (position.GetHistoryCount() < 2)
				{
					bestMove = m_TranspositionTable[transpositionTableKey].m_BestMove;
					return m_TranspositionTable[transpositionTableKey].m_Score;
				}
			}
		}
	}

	if (depth <= 0)
		return QuiescentSearch(position, ply, alpha, beta, maximizeWhite);

	//Null move heuristic
	constexpr int R = 2; //reduced depth constant
	if (allowNullMove  && depth - 1 - R >= 0)
	{
		if (!MoveSearcher::IsKingInCheckFromBitboards(position, position.IsWhiteToPlay()))
		{
			Move nullMove;
			nullMove.SetNullMove();
			position.Update(nullMove);
			std::optional<Move> bestMoveDummy;
			const double score = -AlphaBetaNegamax(position, depth - 1 - R, ply + 1 ,-beta, -alpha, !maximizeWhite, !allowNullMove, bestMoveDummy);
			position.Undo(nullMove);

			if (score >= beta)
				return score;//cutoff
		}
	}

	//Don't search valid moves again if it has been done in last iteration
	//There should be no collision otherwise childMoves may become invalid after next iterations
	//if (m_LegalMovesTable[transpositionTableKey].first != position.GetZobristHash())
	//{
	//	m_LegalMovesTable[transpositionTableKey].first = position.GetZobristHash();
	//	MoveSearcher::GetLegalMovesFromBitboards(position, m_MoveLists[depth]);
	//	m_LegalMovesTable[transpositionTableKey].second = &m_MoveLists[depth];
	//	//MoveSearcher::GetLegalMovesFromBitboards(position, m_LegalMovesTable[transpositionTableKey].second);
	//}
	//m_LegalMovesTable[transpositionTableKey].second;

	MoveList<MaxMoves>& childMoves = m_MoveLists[ply];
	MoveSearcher::GetLegalMovesFromBitboards(position, childMoves);

	if (childMoves.empty())
		return (maximizeWhite ? 1.0 : -1.0) * EvaluatePosition(position, ply);

	//Sort moves
	SortMoves(position, ply, childMoves);

	//Search child nodes
	double value = std::numeric_limits<double>::lowest();
	for (Move& childMove : childMoves)
	{
		position.Update(childMove);
		std::optional<Move> bestMoveDummy; //only returns best move from 0 depth
		const double score = -AlphaBetaNegamax(position, depth - 1, ply + 1, -beta, -alpha, !maximizeWhite, !allowNullMove, bestMoveDummy);
		position.Undo(childMove);

		if (score > value)
		{
			value = score;
			bestMove = childMove;
		}
		
		alpha = std::max(alpha, value);

		if (alpha >= beta)
		{
			//Store killer move
			if (!childMove.IsCapture())
			{
				std::rotate(m_KillerMoves[ply].begin(), m_KillerMoves[ply].end() - 1, m_KillerMoves[ply].end());
				m_KillerMoves[ply][0] = childMove;
			}

			break;//cutoff
		}
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

	MoveList<MaxMoves> childMoves;
	MoveSearcher::GetLegalMovesFromBitboards(position, childMoves);
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

double MoveMaker::QuiescentSearch(Position& position, int ply, double alpha, double beta, bool maximizeWhite)
{
	const double standPat = (maximizeWhite ? 1.0 : -1.0) * EvaluatePosition(position, ply);
	if (standPat >= beta)
		return beta;
	if (alpha < standPat)
		alpha = standPat;

	/*const size_t tableKey = GetTranspositionTableKey(position);
	if (m_LegalMovesTable[tableKey].first != position.GetZobristHash())
	{
		m_LegalMovesTable[tableKey].first = position.GetZobristHash();
		MoveSearcher::GetLegalMovesFromBitboards(position, m_LegalMovesTable[tableKey].second);
	}*/
	//MoveList<MaxMoves> childMoves = m_LegalMovesTable[tableKey].second;

	MoveList<MaxMoves>& childMoves = m_MoveLists[ply];
	MoveSearcher::GetLegalMovesFromBitboards(position, childMoves);

	if (childMoves.empty())
		return standPat;

	//Sort moves
	SortMoves(position, ply, childMoves);

	//Search child capture nodes
	for (Move& childMove : childMoves)
	{
		position.Update(childMove);
		if (childMove.IsCapture())
		{
			const double score = -QuiescentSearch(position, ply + 1, -beta, -alpha, !maximizeWhite);

			if (score >= beta)
			{
				position.Undo(childMove);
				return beta;
			}
			if (score > alpha)
				alpha = score;
		}
		position.Undo(childMove);
	}

	return alpha;
}

double MoveMaker::EvaluatePosition(Position& position, int ply)
{
	MoveMaker::CheckGameOver(position, ply);
	const double score = PositionEvaluation::EvaluatePosition(position);
	position.SetGameStatus(Position::GameStatus::Running);
	return score;
}

void MoveMaker::SortMoves(const Position& position, int ply, MoveList<MaxMoves>& moves)
{
	std::sort(moves.begin(), moves.end(), [&position, &ply, this](const Move& move1, const Move& move2)->bool { return MovesSorter(position, ply, move1, move2); });

	//Best move from previous iteration is picked as best guess
	const size_t transpositionTableKey = GetTranspositionTableKey(position);
	if (m_TranspositionTable[transpositionTableKey].m_ZobristHash == position.GetZobristHash())
	{
		const Move& previousBest = m_TranspositionTable[transpositionTableKey].m_BestMove;
		MoveList<MaxMoves>::const_iterator searchIt = std::find(moves.begin(), moves.end(), previousBest);
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

bool MoveMaker::MovesSorter(const Position& position, int ply, const Move& move1, const Move& move2)
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

	//Killer Moves
	const bool isMove1Killer = std::find(m_KillerMoves[ply].begin(), m_KillerMoves[ply].end(), move1) != m_KillerMoves[ply].end();
	const bool isMove2Killer = std::find(m_KillerMoves[ply].begin(), m_KillerMoves[ply].end(), move2) != m_KillerMoves[ply].end();
	if (isMove1Killer && !isMove2Killer)
		return true;

	return false;
}

size_t MoveMaker::GetTranspositionTableKey(const Position& position) const
{
	return (position.GetZobristHash() % m_TranspositionTable.size());
}
