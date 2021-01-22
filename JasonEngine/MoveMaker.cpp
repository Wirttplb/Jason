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
	std::vector<Move> allLegalMoves = MoveSearcher::GetLegalMoves(position);

	if (allLegalMoves.empty())
		return newPosition; //Stalemate

	//position score is signed, > 0 is good for white ; < 0 good for black
	double bestScore = position.IsWhiteToPlay() ? std::numeric_limits<double>::lowest() : std::numeric_limits<double>::max();
	Move& bestMove = allLegalMoves.front();
	//to promote castles
	//PositionEvaluation::Score bestCastlingMoveScore = bestScore;
	//std::optional<Move> castlingMove;
	
	//Evaluate all legal moves based on new position score (~minimax rule)
	for (Move& move : allLegalMoves)
	{
		const double score = m_PositionEvaluator.EvaluateMove(position, move, depth);
		
		if ((position.IsWhiteToPlay() && score > bestScore) || (!position.IsWhiteToPlay() && score < bestScore))
		{
			bestScore = score;
			bestMove = move;
		}

		//if (move.IsCastling() && ((position.IsWhiteToPlay() && score > bestCastlingMoveScore) || (!position.IsWhiteToPlay() && score < bestCastlingMoveScore)))
		//{
		//	bestCastlingMoveScore = score;
		//	castlingMove = move;
		//}
	}

	//Promote castling
	//if (castlingMove && abs(bestCastlingMoveScore - bestScore) < 0.9)
	//	bestMove = *castlingMove;

	//RANDOM MOVE
	//const int rand = std::rand();
	//bestMove = allLegalMoves[rand % allLegalMoves.size()];

	newPosition = position;
	newPosition->Update(bestMove);

	//Check there is still a king!!
	int n = 0;
	for (const Piece& piece : newPosition->GetWhitePieces())
	{
		if (piece.m_Type == PieceType::King)
			n++;
	}
	for (const Piece& piece : newPosition->GetBlackPieces())
	{
		if (piece.m_Type == PieceType::King)
			n++;
	}
	assert(n == 2);

	return newPosition;
}

bool MoveMaker::MakeMove(Position& position, Move& move)
{
	//Check move is legal
	std::vector<Move> legalMoves = MoveSearcher::GetLegalMoves(position, move.m_From, position.IsWhiteToPlay());

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
	if (position.IsInsufficientMaterial())
	{
		position.SetGameStatus(Position::GameStatus::StaleMate);
		return;
	}

	//Check all legal moves
	std::vector<Move> allLegalMoves;
	const std::vector<Piece>& piecesToMove = position.IsWhiteToPlay() ? position.GetWhitePieces() : position.GetBlackPieces();
	for (const Piece& piece : piecesToMove)
	{
		//Get Legal moves
		std::vector<Move> moves = MoveSearcher::GetLegalMoves(position, piece, position.IsWhiteToPlay());
		allLegalMoves.insert(allLegalMoves.end(), moves.begin(), moves.end());
	}

	if (allLegalMoves.empty())
	{
		if (MoveSearcher::IsKingInCheck(position, position.IsWhiteToPlay()))
			position.SetGameStatus(Position::GameStatus::CheckMate);
		else
			position.SetGameStatus(Position::GameStatus::StaleMate);
	}

	return;
}