#include "pch.h"
#include "PositionEvaluation.h"
#include "MoveMaker.h"
#include "MoveSearcher.h"
#include <assert.h>

/// <summary>
/// Absolute score for a mate
/// </summary>
static constexpr double Mate = 1000000;

double PositionEvaluation::EvaluateMove(Position& position, Move& move, int depth)
{
	//m_InitialDepth = depth;
	position.Update(move);
	constexpr double alpha = std::numeric_limits<double>::lowest();
	constexpr double beta = std::numeric_limits<double>::max();
	double score = AlphaBetaNegamax(position, depth, alpha, beta, position.IsWhiteToPlay());
	position.Undo(move);

	return score;
}

double PositionEvaluation::AlphaBetaNegamax(Position& position, int depth, double alpha, double beta, bool maximizeWhite)
{
	const double originalAlpha = alpha;

	//Transposition table lookup
	if ((m_TranspositionTable.find(position) != m_TranspositionTable.end()) && (m_TranspositionTable[position].m_Depth >= depth))
	{
		switch (m_TranspositionTable[position].m_Flag)
		{
		case TranspositionTableEntry::Flag::Exact:
			return m_TranspositionTable[position].m_Score;
		case TranspositionTableEntry::Flag::LowerBound:
			alpha = std::max(alpha, m_TranspositionTable[position].m_Score);
			break;
		case TranspositionTableEntry::Flag::UpperBound:
			beta = std::min(beta, m_TranspositionTable[position].m_Score);
			break;
		default:
			assert(false);
			break;
		}

		if (alpha >= beta)
			return m_TranspositionTable[position].m_Score;
	}

	if (depth == 0)
		return (maximizeWhite ? 1.0 : -1.0) * EvaluatePosition(position);

	std::vector<Move> childMoves = MoveSearcher::GetLegalMoves(position);
	if (childMoves.empty())
		return (maximizeWhite ? 1.0 : -1.0) * EvaluatePosition(position);

	double value = 0.0;
	const size_t piecesCount = position.GetBlackPiecesList().size() + position.GetWhitePiecesList().size();
	value = std::numeric_limits<double>::lowest();
	for (Move& childMove : childMoves)
	{
		position.Update(childMove);
		const size_t piecesCount2 = position.GetBlackPiecesList().size() + position.GetWhitePiecesList().size();
		const int actualDepth = depth - 1;// (piecesCount == piecesCount2) ? depth - 1 : depth; //go deeper when we sense a tactic (capture...) ~ Quiescent search
		value = std::max(value, -AlphaBetaNegamax(position, actualDepth, -beta, -alpha, !maximizeWhite));
		alpha = std::max(alpha, value);
		position.Undo(childMove);

		if (alpha >= beta)
			break;//cutoff
	}

	//Transposition Table Store
	m_TranspositionTable[position].m_Score = value;
	if (value <= originalAlpha)
		m_TranspositionTable[position].m_Flag = TranspositionTableEntry::Flag::UpperBound;
	else if (value >= beta)
		m_TranspositionTable[position].m_Flag = TranspositionTableEntry::Flag::LowerBound;
	else
		m_TranspositionTable[position].m_Flag = TranspositionTableEntry::Flag::Exact;

	m_TranspositionTable[position].m_Depth = depth;

	return value;
}

double PositionEvaluation::Minimax(Position& position, int depth, bool maximizeWhite)
{
	if (depth == 0)
		return EvaluatePosition(position);

	std::vector<Move> childMoves = MoveSearcher::GetLegalMoves(position);
	if (childMoves.empty())
		return EvaluatePosition(position);

	double value = 0.0;
	if (maximizeWhite)
	{
		value = std::numeric_limits<double>::lowest();
		for (Move& childMove : childMoves)
		{
			position.Update(childMove);
			value = std::max(value, Minimax(position, depth - 1, false));
			position.Undo(childMove);
		}

		return value;
	}
	else
	{
		value = std::numeric_limits<double>::max();
		for (Move& childMove : childMoves)
		{
			position.Update(childMove);
			value = std::min(value, Minimax(position, depth - 1, true));
			position.Undo(childMove);
		}

		return value;
	}
}

static double SqDistanceBetweenPieces(const Piece& a, const Piece& b)
{
	const int ax = a.m_Square % 8;
	const int ay = a.m_Square / 8;
	const int bx = a.m_Square % 8;
	const int by = a.m_Square / 8;
	return ((ax - bx)  * (ax - bx) + (ay - by) * (ay - by));
}

double PositionEvaluation::EvaluatePosition(const Position& position)
{
	Position positionCopy = position;

	//Check checkmate/stalemate
	MoveMaker::CheckGameOver(positionCopy);
	switch (positionCopy.GetGameStatus())
	{
	case Position::GameStatus::StaleMate:
		return 0.0;
	case Position::GameStatus::CheckMate:
	{
		double score = position.IsWhiteToPlay() ? -Mate : Mate;
		//add correction so M1 > M2 etc
		score += (position.IsWhiteToPlay() ? 1.0 : -1.0) * static_cast<int>(position.GetMoves().size());
		return score;
	}
	default:
		break;
	}

	double score = 0.0;
	//Check material
	const std::vector<Piece>& whitePieces = position.GetWhitePiecesList();
	const std::vector<Piece>& blackPieces = position.GetBlackPiecesList();
	double whiteMaterial = 0.0;
	double blackMaterial = 0.0;
	for (const Piece& piece : whitePieces)
	{
		whiteMaterial += GetPieceValue(piece.m_Type);
	}
	for (const Piece& piece : blackPieces)
	{
		blackMaterial += GetPieceValue(piece.m_Type);
	}

	score = whiteMaterial - blackMaterial;

	//check hanging pieces
	//std::vector<const Piece*> hangingPieces = GetHangingPieces(position);
	//for (const Piece* piece : hangingPieces)
	//{
	//	score += GetPieceValue(piece->m_Type) * (position.IsWhiteToPlay() ? 1.0 : -1.0);
	//}

	//Check double pawns
	int whiteDoubledPawns = CountDoubledPawn(position, true);
	int blackDoubledPawns = CountDoubledPawn(position, false);

	score -= static_cast<double>(whiteDoubledPawns) * 0.33;
	score += static_cast<double>(blackDoubledPawns) * 0.33;

	//Castling bonus: castling improves score during opening, importance of castling decays during the game
	double castleBonus = 1.0 * 0.025 * std::max(0.0, 40.0 - static_cast<double>(position.GetMoves().size()));
	if (position.HasWhiteCastled())
		score += castleBonus;
	if (position.HasBlackCastled())
		score -= castleBonus;

	//Check number of controled squares
	std::set<int> whiteControlledByPawns;
	std::set<int> whiteControlledSquares = GetControlledSquares(position, true, whiteControlledByPawns);
	std::set<int> blackControlledByPawns;
	std::set<int> blackControlledSquares = GetControlledSquares(position, false, blackControlledByPawns);

	score -= (blackControlledSquares.size() / 64.0) * 1.0;
	score += (whiteControlledSquares.size() / 64.0) * 1.0;
	//Check control of center (by pawns)
	score -= (CountCenterControlledByPawns(blackControlledByPawns, false) / 4.0) * 0.5;
	score += (CountCenterControlledByPawns(whiteControlledByPawns, true) / 4.0) * 0.5;

	//Check space behind pawns
	//Check king in check?
	//Check concentration of pieces around enemy king for an attack/mate?
	const Piece* whiteKing = position.GetPiece(PieceType::King, true);
	const Piece* blackKing = position.GetPiece(PieceType::King, false);
	if (!whiteKing || !blackKing)
	{
		assert(false);
		return score;
	}

	double squareDistanceToWhiteKing = 0.0;
	double squareDistanceToBlackKing = 1.0;
	for (const Piece& piece : whitePieces)
	{
		if (piece.m_Type != PieceType::King)
		{
			squareDistanceToBlackKing += SqDistanceBetweenPieces(piece, *blackKing);
		}
	}
	for (const Piece& piece : blackPieces)
	{
		if (piece.m_Type != PieceType::King)
		{
			squareDistanceToWhiteKing += SqDistanceBetweenPieces(piece, *whiteKing);
		}
	}

	score += squareDistanceToWhiteKing * 0.003;
	score -= squareDistanceToBlackKing * 0.003;

	assert(abs(score) < Mate);

	return score;
}

double PositionEvaluation::GetPieceValue(PieceType type)
{
	double value = 0.0;
	switch (type)
	{
	case PieceType::Queen:
		value = 9;
		break;
	case PieceType::Rook:
		value = 5;
		break;
	case PieceType::Bishop:
		value = 3;
		break;
	case PieceType::Knight:
		value = 3;
		break;
	case PieceType::Pawn:
		value = 1;
		break;
	default:
		break;
	}

	return value;
}

bool PositionEvaluation::IsPieceDefended(const Position& position, const Piece& piece, bool isWhite)
{
	const std::vector<Piece>& friendlyPieces = isWhite ? position.GetWhitePiecesList() : position.GetBlackPiecesList();
	for (const Piece& friendlyPiece : friendlyPieces)
	{
		if (&friendlyPiece == &piece)
			continue;

		std::vector<Move> moves = MoveSearcher::GetLegalMoves(position, friendlyPiece, isWhite);
		for (const Move& move : moves)
		{
			if (move.m_To.m_Square == piece.m_Square)
				return true;
		}
	}

	return false;
}

bool PositionEvaluation::IsPieceAttacked(const Position& position, const Piece& piece, bool isWhite)
{
	const std::vector<Piece>& enemyPieces = isWhite ? position.GetBlackPiecesList() : position.GetWhitePiecesList();
	for (const Piece& enemyPiece : enemyPieces)
	{
		std::vector<Move> moves  = MoveSearcher::GetLegalMoves(position, enemyPiece, !isWhite);
		for (const Move& move : moves)
		{
			if (move.m_To.m_Square == piece.m_Square)
				return true;
		}
	}

	return false;
}

std::vector<const Piece*> PositionEvaluation::GetHangingPieces(const Position& position)
{
	std::vector<const Piece*> hangingPieces;
	const std::vector<Piece>& enemyPieces = position.IsWhiteToPlay() ? position.GetBlackPiecesList() : position.GetWhitePiecesList();
	for (const Piece& piece : enemyPieces)
	{
		if (IsPieceAttacked(position, piece, !position.IsWhiteToPlay()) && !IsPieceDefended(position, piece, !position.IsWhiteToPlay()))
			hangingPieces.push_back(&piece);
	}

	return hangingPieces;
}

int PositionEvaluation::CountDoubledPawn(const Position& position, bool isWhite)
{
	int count = 0;
	const std::vector<Piece>& pieces = isWhite ? position.GetWhitePiecesList() : position.GetBlackPiecesList();
	
	std::set<int> checkedFiles;
	for (const Piece& piece : pieces)
	{
		if (piece.m_Type == PieceType::Pawn)
		{
			if (checkedFiles.find(piece.m_Square % 8) != checkedFiles.end())
				continue;

			checkedFiles.insert(piece.m_Square % 8);
			for (const Piece& piece2 : pieces)
			{
				if ((&piece != &piece2) && (piece2.m_Type == PieceType::Pawn) && (piece.m_Square % 8 == piece2.m_Square % 8))
					count++;
			}
		}
	}

	return count;
}

static int SquareToIdx(const std::array<int, 2>& square)
{
	return square[0] + 8 * square[1];
}

std::set<int> PositionEvaluation::GetControlledSquares(const Position& position, const Piece& piece, bool isWhite)
{
	std::set<int> controlledSquares;
	if (piece.m_Type == PieceType()) //Pawn is special case
	{
		const int direction = isWhite ? 1 : -1;
		const int nextY = piece.m_Square / 8 + 1 * direction;
		if (nextY > 0 && nextY < 7)
		{
			if ((piece.m_Square % 8 > 0))
				controlledSquares.insert(piece.m_Square - 1 + 8 * direction );

			if (piece.m_Square % 8 < 7)
				controlledSquares.insert(piece.m_Square + 1 + 8 * direction);
		}
	}
	else
	{
		std::vector<Move> moves = MoveSearcher::GetLegalMoves(position, piece, true);
		for (const Move& move : moves)
		{
			controlledSquares.insert(move.m_To.m_Square);
		}
	}

	return controlledSquares;
}

std::set<int> PositionEvaluation::GetControlledSquares(const Position& position, bool isWhite, std::set<int>& byPawn)
{
	std::set<int> controlledSquares;
	byPawn.clear();

	const std::vector<Piece>& pieces = isWhite ? position.GetWhitePiecesList() : position.GetBlackPiecesList();
	for (const Piece& piece : pieces)
	{
		const std::set<int> squares = GetControlledSquares(position, piece, isWhite);
		for (int square : squares)
		{
			controlledSquares.insert(square); //set controlled by pawn flag
			if (piece.m_Type == PieceType::Pawn)
				byPawn.insert(square);
		}
	}

	return controlledSquares;
}

int PositionEvaluation::CountCenterControlledByPawns(const std::set<int>& squares, bool isWhite)
{
	std::set<int> center;
	for (int square : squares)
	{
		if (isWhite && (
			square == SquareToIdx({ 2, 4 }) ||
			square == SquareToIdx({ 3, 4 }) ||
			square == SquareToIdx({ 4, 4 }) ||
			square == SquareToIdx({ 5, 4 })))
			center.insert(square);
		else if (!isWhite && (
			square == SquareToIdx({ 2, 3 }) ||
			square == SquareToIdx({ 3, 3 }) ||
			square == SquareToIdx({ 4, 3 }) ||
			square == SquareToIdx({ 5, 3 })))
			center.insert(square);
	}

	return static_cast<int>(center.size());
}
