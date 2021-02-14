#include "pch.h"
#include "MoveMaker.h"
#include "PositionEvaluation.h"
#include <string>
#include <assert.h>
#include <algorithm>

bool MoveMaker::MakeMove(double maxTime, Position& position, int maxDepth, int& score, int& searchDepth)
{
	std::optional<Move> bestMove = FindMove(maxTime, position, maxDepth, score, searchDepth);

	if (bestMove.has_value())
		position.Update(*bestMove);

	return bestMove.has_value();
}

bool MoveMaker::MakeMove(Position& position, int maxDepth, int& score)
{
	int searchDepth = 1; //ignored
	return MakeMove(3600.0, position, maxDepth, score, searchDepth);
}

std::optional<Move> MoveMaker::FindMove(double maxTime, Position& position, int maxDepth, int& score, int& searchDepth)
{
	constexpr int alpha = -Mate;
	constexpr int beta = Mate;
	std::optional<Move> bestMove;
	const bool allowNullMove = false;
	m_KillerMoves = {};

	time_t start;
	time_t t;
	time(&start);
	//Iterative deepening
	for (searchDepth = 1; searchDepth <= maxDepth; searchDepth++)
	{
		score = AlphaBetaNegamax(position, searchDepth, 0, alpha, beta, position.IsWhiteToPlay(), allowNullMove, bestMove);

		//break if mate found
		if (score > 10000)
			break;

		//break if out of time
		time(&t);
		if (difftime(start, t) > (maxTime  * 0.33)) //dont go deeper if third of allowed time already spent
			break;
	}
	
	searchDepth = std::min(searchDepth, maxDepth);
	score *= (position.IsWhiteToPlay() ? 1 : -1);
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
	if (position.IsInsufficientMaterialFromBitboards() || position.IsRepetitionDraw())
	{
		position.SetGameStatus(Position::GameStatus::Draw);
		return;
	}

	if (ply < 0)
	{
		ply = 0;
		MoveSearcher::GetLegalMovesFromBitboards(position, m_MoveLists[ply]);
	}

	MoveList<MaxMoves>& childMoves = m_MoveLists[ply];
	MoveSearcher::GetLegalMovesFromBitboards(position, childMoves);

	if (childMoves.empty())
	{
		if (MoveSearcher::IsKingInCheckFromBitboards(position, position.IsWhiteToPlay()))
			position.SetGameStatus(Position::GameStatus::CheckMate);
		else
			position.SetGameStatus(Position::GameStatus::Draw);
	}
}

int MoveMaker::AlphaBetaNegamax(Position& position, int depth, int ply, int alpha, int beta, bool maximizeWhite, bool allowNullMove, std::optional<Move>& bestMove)
{
	const int originalAlpha = alpha;

	//Transposition table lookup
	const size_t transpositionTableKey = GetTranspositionTableKey(position);
	if (!position.IsRepetition()) //Repetition would affect the score, can't use TT
	{
		
		if ((m_TranspositionTable[transpositionTableKey].m_ZobristHash == position.GetZobristHash()) &&
			(m_TranspositionTable[transpositionTableKey].m_Depth >= depth))
		{
			switch (m_TranspositionTable[transpositionTableKey].m_Flag)
			{
			case TranspositionTableEntry::Flag::Exact:
			{
				bestMove = m_TranspositionTable[transpositionTableKey].m_BestMove;
				return m_TranspositionTable[transpositionTableKey].m_Score;
				break;
			}
			case TranspositionTableEntry::Flag::LowerBound:
				alpha = std::max(alpha, static_cast<int>(m_TranspositionTable[transpositionTableKey].m_Score));
				break;
			case TranspositionTableEntry::Flag::UpperBound:
				beta = std::min(beta, static_cast<int>(m_TranspositionTable[transpositionTableKey].m_Score));
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
			const int score = -AlphaBetaNegamax(position, depth - 1 - R, ply + 1 ,-beta, -alpha, !maximizeWhite, !allowNullMove, bestMoveDummy);
			position.Undo(nullMove);

			if (score >= beta)
				return score;//cutoff
		}
	}

	MoveList<MaxMoves>& childMoves = m_MoveLists[ply];
	MoveSearcher::GetLegalMovesFromBitboards(position, childMoves);

	if (childMoves.empty())
		return (maximizeWhite ? 1 : -1) * EvaluatePosition(position, ply);

	//Sort moves
	SortMoves(position, ply, childMoves);

	//Search child nodes
	int value = std::numeric_limits<int>::lowest();
	for (Move& childMove : childMoves)
	{
		position.Update(childMove);
		std::optional<Move> bestMoveDummy; //only returns best move from 0 depth
		const int score = -AlphaBetaNegamax(position, depth - 1, ply + 1, -beta, -alpha, !maximizeWhite, !allowNullMove, bestMoveDummy);
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
	assert(abs(value) <= Mate);
	m_TranspositionTable[transpositionTableKey].m_Score = value;
	m_TranspositionTable[transpositionTableKey].m_Depth = depth;
	assert(bestMove.has_value());
	m_TranspositionTable[transpositionTableKey].m_BestMove = *bestMove;
	if (value <= originalAlpha)
		m_TranspositionTable[transpositionTableKey].m_Flag = TranspositionTableEntry::Flag::UpperBound;
	else if (value >= beta)
		m_TranspositionTable[transpositionTableKey].m_Flag = TranspositionTableEntry::Flag::LowerBound;
	else
		m_TranspositionTable[transpositionTableKey].m_Flag = TranspositionTableEntry::Flag::Exact;

	return value;
}

int MoveMaker::Minimax(Position& position, int depth, bool maximizeWhite, std::optional<Move>& bestMove)
{
	if (depth == 0)
		return EvaluatePosition(position);

	MoveList<MaxMoves> childMoves;
	MoveSearcher::GetLegalMovesFromBitboards(position, childMoves);
	if (childMoves.empty())
		return EvaluatePosition(position);

	int value = 0;
	if (maximizeWhite)
	{
		value = std::numeric_limits<int>::lowest();
		for (Move& childMove : childMoves)
		{
			position.Update(childMove);
			std::optional<Move> dummyBestMove;
			const int score = Minimax(position, depth - 1, false, dummyBestMove);
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
		value = std::numeric_limits<int>::max();
		for (Move& childMove : childMoves)
		{
			position.Update(childMove);
			std::optional<Move> dummyBestMove;
			const int score = std::min(value, Minimax(position, depth - 1, true, dummyBestMove));
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

int MoveMaker::QuiescentSearch(Position& position, int ply, int alpha, int beta, bool maximizeWhite)
{
	const int standPat = (maximizeWhite ? 1 : -1) * EvaluatePosition(position, ply);
	if (standPat >= beta)
		return beta;
	if (alpha < standPat)
		alpha = standPat;

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
			const int score = -QuiescentSearch(position, ply + 1, -beta, -alpha, !maximizeWhite);

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

int MoveMaker::EvaluatePosition(Position& position, int ply)
{
	MoveMaker::CheckGameOver(position, ply);
	const int score = PositionEvaluation::EvaluatePosition(position, ply);
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
	//Killer Moves
	const bool isMove1Killer = std::find(m_KillerMoves[ply].begin(), m_KillerMoves[ply].end(), move1) != m_KillerMoves[ply].end();
	const bool isMove2Killer = std::find(m_KillerMoves[ply].begin(), m_KillerMoves[ply].end(), move2) != m_KillerMoves[ply].end();
	if (isMove1Killer && !isMove2Killer)
		return true;
	else if (!isMove1Killer && isMove2Killer)
		return false;

	//Check capture of last moved piece
	if (!position.GetMoves().empty())
	{
		const bool isMove1Recapture = move1.GetToSquare() == position.GetMoves().back().GetToSquare();
		const bool isMove2Recapture = move2.GetToSquare() == position.GetMoves().back().GetToSquare();
		if (isMove1Recapture && !isMove2Recapture)
			return true;
		else if (!isMove1Recapture && isMove2Recapture)
			return false;
		else if (isMove1Recapture && isMove2Recapture)
			return move1.GetFromType() < move2.GetFromType(); //LVA
	}

	//Check all other captures with Most Valuable Victim - Least Valuable Aggressor heuristic
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
