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
	position.Update(move);
	constexpr double alpha = std::numeric_limits<double>::lowest();
	constexpr double beta = std::numeric_limits<double>::max();
	double score = 0.0;// AlphaBetaNegamax(position, depth, alpha, beta, position.IsWhiteToPlay());
	position.Undo(move);

	return score;
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
	score = CountMaterial(position, true) - CountMaterial(position, false);

	//penalty for moving same pieces twice
	if (position.GetMoves().size() > 3)
	{
		const int lastIdx = static_cast<int>(position.GetMoves().size()) - 1;
		if (position.GetMoves()[lastIdx].m_From.m_Square == position.GetMoves()[lastIdx - 2].m_To.m_Square)
			score = (position.IsWhiteToPlay() ? -0.33 : 0.33);
	}

	//Check double pawns
	int whiteDoubledPawns = CountDoubledPawns(position, true);
	int blackDoubledPawns = CountDoubledPawns(position, false);

	score -= static_cast<double>(whiteDoubledPawns) * 0.33;
	score += static_cast<double>(blackDoubledPawns) * 0.33;

	//Castling bonus: castling improves score during opening, importance of castling decays during the game
	double castleBonus = 1.0 * 0.025 * std::max(0.0, 40.0 - static_cast<double>(position.GetMoves().size()));
	if (position.HasWhiteCastled())
		score += castleBonus;
	if (position.HasBlackCastled())
		score -= castleBonus;

	Bitboard whiteControlledSquares = GetControlledSquares(position, true);
	Bitboard blackControlledSquares = GetControlledSquares(position, false);
	score -= (blackControlledSquares.CountSetBits() / 64.0) * 1.0;
	score += (whiteControlledSquares.CountSetBits() / 64.0) * 1.0;

	score += CountCenterPawns(position, true) * 0.5;
	score -= CountCenterPawns(position, false) * 0.5;

	//////////////////////////

	//Check space behind pawns
	//Check king in check?
	//Check concentration of pieces around enemy king for an attack/mate?
	//const Piece* whiteKing = position.GetPiece(PieceType::King, true);
	//const Piece* blackKing = position.GetPiece(PieceType::King, false);
	//if (!whiteKing || !blackKing)
	//{
	//	assert(false);
	//	return score;
	//}

	/*double squareDistanceToWhiteKing = 0.0;
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
	}*/

	//score += squareDistanceToWhiteKing * 0.003;
	//score -= squareDistanceToBlackKing * 0.003;

	return score;
}

double PositionEvaluation::CountMaterial(const Position& position, bool isWhite)
{
	double material = 0.0;
	material += (isWhite ? position.GetWhitePawns().CountSetBits() : position.GetBlackPawns().CountSetBits());
	material += 3.0 * (isWhite ? position.GetWhiteKnights().CountSetBits() : position.GetBlackKnights().CountSetBits());
	material += 3.5 * (isWhite ? position.GetWhiteBishops().CountSetBits() : position.GetBlackBishops().CountSetBits());
	material += 5.0 * (isWhite ? position.GetWhiteRooks().CountSetBits() : position.GetBlackRooks().CountSetBits());
	material += 9.0 * (isWhite ? position.GetWhiteQueens().CountSetBits() : position.GetBlackQueens().CountSetBits());
	return material;
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

int PositionEvaluation::CountDoubledPawns(const Position& position, bool isWhite)
{
	int count = 0;
	const Bitboard& pawns = isWhite ? position.GetWhitePawns() : position.GetBlackPawns();
	for (int i = 0; i < 8; i++)
	{
		Bitboard file = pawns & _files[i];
		int countOnFile = file.CountSetBits();
		if (countOnFile > 1)
			count += countOnFile;
	}

	return count;
}

int PositionEvaluation::CountCenterPawns(const Position& position, bool isWhite)
{
	static const Bitboard whiteCenter = _d4 | _e4;
	static const Bitboard blackCenter = _d5 | _e5;
	Bitboard centerPawns = (isWhite ? position.GetWhitePawns() : position.GetBlackPawns()) &
		(isWhite ? whiteCenter : blackCenter);

	return centerPawns.CountSetBits();
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

Bitboard PositionEvaluation::GetControlledSquares(const Position& position, bool isWhite)
{
	return MoveSearcher::GetPseudoLegalSquaresFromBitboards(position, isWhite);
}

int PositionEvaluation::CountCenterControlledByPawns(const std::set<int>& squares, bool isWhite)
{
	std::set<int> center;
	for (int square : squares)
	{
		if (isWhite && (
			square == c4 ||
			square == d4 ||
			square == e4 ||
			square == f4))
			center.insert(square);
		else if (!isWhite && (
			square == c5 ||
			square == d5 ||
			square == e5 ||
			square == f5))
			center.insert(square);
	}

	return static_cast<int>(center.size());
}
