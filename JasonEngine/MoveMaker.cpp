#include "pch.h"
#include "MoveMaker.h"
#include "MoveSearcher.h"
#include "PositionEvaluation.h"
#include <string>
#include <assert.h>

bool MoveMaker::MakeMove(Position& position)
{
	bool moveFound = false;
	std::optional<Position> newPosition = FindMove(position);

	if (newPosition.has_value())
	{
		position = *newPosition;
		position.SetWhiteToPlay(!position.IsWhiteToPlay());
		moveFound = true;
	}

	return moveFound;
}

std::optional<Position> MoveMaker::FindMove(Position& position)
{
	std::optional<Position> newPosition;
	std::vector<Position::Move> allLegalMoves;

	const std::vector<Position::Piece>& piecesToMove = position.IsWhiteToPlay() ? position.GetWhitePieces() : position.GetBlackPieces();
	for (const Position::Piece& piece : piecesToMove)
	{
		//Get Legal moves
		std::vector<Position::Move> moves = MoveSearcher::GetLegalMoves(position, piece, position.IsWhiteToPlay());
		allLegalMoves.insert(allLegalMoves.end(), moves.begin(), moves.end());
	}

	if (allLegalMoves.empty())
		return newPosition; //Stalemate

	double bestScore = position.IsWhiteToPlay() ? std::numeric_limits<double>::lowest() : std::numeric_limits<double>::max();
	Position::Move& bestMove = allLegalMoves.front();
	double bestCastlingMoveScore = bestScore;
	std::optional<Position::Move> castlingMove;
	
	for (const Position::Move& move : allLegalMoves)
	{
		Position possiblePosition = position;
		MoveMaker::UpdatePosition(possiblePosition, move);
		possiblePosition.SetWhiteToPlay(!possiblePosition.IsWhiteToPlay());
		double score = PositionEvaluation::EvaluatePosition(possiblePosition, 3);
		
		if ((position.IsWhiteToPlay() && score > bestScore) || (!position.IsWhiteToPlay() && score < bestScore))
		{ //position.IsWhiteToPlay => we want to get lowest score / !position.IsWhiteToPlay => we want to get highest score
			bestScore = score;
			bestMove = move;
		}

		if (move.IsCastling() && ((position.IsWhiteToPlay() && score > bestCastlingMoveScore) || (!position.IsWhiteToPlay() && score < bestCastlingMoveScore)))
		{ //position.IsWhiteToPlay => we want to get lowest score / !position.IsWhiteToPlay => we want to get highest score
			bestCastlingMoveScore = score;
			castlingMove = move;
		}
	}

	//Promote castling
	if (castlingMove && abs(bestCastlingMoveScore - bestScore) < 0.9)
		bestMove = *castlingMove;

	//RANDOM MOVE
	//const int rand = std::rand();
	//bestMove = allLegalMoves[rand % allLegalMoves.size()];

	newPosition = position;
	UpdatePosition(*newPosition, bestMove);

	//Check there is still a king!!
	int n = 0;
	for (const Position::Piece& piece : newPosition->GetWhitePieces())
	{
		if (piece.m_Type == Position::PieceType::King)
			n++;
	}
	for (const Position::Piece& piece : newPosition->GetBlackPieces())
	{
		if (piece.m_Type == Position::PieceType::King)
			n++;
	}
	assert(n == 2);

	return newPosition;
}

bool MoveMaker::MakeMove(Position& position, Position::Move& move)
{
	//Check move is legal
	std::vector<Position::Move> legalMoves = MoveSearcher::GetLegalMoves(position, move.m_From, position.IsWhiteToPlay());

	bool isLegal = false;
	for (const Position::Move& legalMove : legalMoves)
	{
		if (legalMove == move)
		{
			isLegal = true;
			break;
		}
	}

	if (!isLegal)
		return false;

	MoveMaker::UpdatePosition(position, move);
	position.SetWhiteToPlay(!position.IsWhiteToPlay());
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
	std::vector<Position::Move> allLegalMoves;
	const std::vector<Position::Piece>& piecesToMove = position.IsWhiteToPlay() ? position.GetWhitePieces() : position.GetBlackPieces();
	for (const Position::Piece& piece : piecesToMove)
	{
		//Get Legal moves
		std::vector<Position::Move> moves = MoveSearcher::GetLegalMoves(position, piece, position.IsWhiteToPlay());
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

void MoveMaker::UpdatePosition(Position& position, const Position::Move& move)
{
	//update moving piece
	std::vector<Position::Piece>& friendlyPieces = position.IsWhiteToPlay() ? position.GetWhitePieces() : position.GetBlackPieces();
	for (Position::Piece& friendlyPiece : friendlyPieces)
	{
		if (friendlyPiece.m_Position == move.m_From.m_Position)
		{
			friendlyPiece = move.m_To; //update type too for queening
			break;
		}
	}

	//we have to correct new position by removing a piece if taken!
	std::vector<Position::Piece>& enemyPieces = position.IsWhiteToPlay() ? position.GetBlackPieces() : position.GetWhitePieces();
	std::optional<size_t> indexPieceToRemove;
	for (size_t i = 0; i < enemyPieces.size(); i++)
	{
		if (enemyPieces[i].m_Position == move.m_To.m_Position)
		{
			indexPieceToRemove = i;
			break;
		}
	}

	//set or reset en passant square
	if ((move.m_From.m_Type == Position::PieceType::Pawn) && (abs(move.m_From.m_Position[1] - move.m_To.m_Position[1]) == 2))
	{
		position.SetEnPassantSquare({
		move.m_From.m_Position[0],
		move.m_From.m_Position[1] + (position.IsWhiteToPlay() ? 1 : -1) });
	}
	else
		position.ResetEnPassantSquare();

	if (indexPieceToRemove.has_value())
		enemyPieces.erase(enemyPieces.begin() + *indexPieceToRemove);

	Position::Move move2 = move; //set capture flag
	move2.m_IsCapture = indexPieceToRemove.has_value();

	//Move rook if castle
	if ((move2.m_From.m_Type == Position::PieceType::King) && abs(move2.m_From.m_Position[0] - move2.m_To.m_Position[0]) > 1)
	{
		if ((move2.m_To.m_Position[0] - move2.m_From.m_Position[0]) > 1) //kingside
		{
			//search the corresponding rook
			std::array<int, 2> rookSquare = move2.m_To.m_Position;
			rookSquare[0] = 7;
			for (Position::Piece& friendlyPiece : friendlyPieces)
			{
				if (friendlyPiece.m_Position == rookSquare)
				{
					assert(friendlyPiece.m_Type == Position::PieceType::Rook);
					friendlyPiece.m_Position = rookSquare;
					friendlyPiece.m_Position[0] -= 2;
					break;
				}
			}
		}
		else //queenside
		{
			std::array<int, 2> rookSquare = move2.m_To.m_Position;
			rookSquare[0] = 0;
			for (Position::Piece& friendlyPiece : friendlyPieces)
			{
				if (friendlyPiece.m_Position == rookSquare)
				{
					assert(friendlyPiece.m_Type == Position::PieceType::Rook);
					friendlyPiece.m_Position = rookSquare;
					friendlyPiece.m_Position[0] += 3;
					break;
				}
			}
		}
	}

	//Update castling flags
	//Rook moves
	constexpr std::array<int, 2> bl = { 0,0 };
	constexpr std::array<int, 2> br = { 7,0 };
	constexpr std::array<int, 2> tl = { 7,0 };
	constexpr std::array<int, 2> tr = { 7,7 };
	if ((move2.m_From.m_Position == bl) || (move2.m_To.m_Position == bl)) //rook move or capture
		position.SetCanWhiteCastleQueenSide(false);
	if ((move2.m_From.m_Position == br) || (move2.m_To.m_Position == br))
		position.SetCanWhiteCastleKingSide(false);
	if ((move2.m_From.m_Position == tl) || (move2.m_To.m_Position == tl))
		position.SetCanBlackCastleQueenSide(false);
	if ((move2.m_From.m_Position == tr) || (move2.m_To.m_Position == tr))
		position.SetCanBlackCastleKingSide(false);
	//King moves
	if (position.IsWhiteToPlay() && (move2.m_From.m_Type == Position::PieceType::King))
	{
		position.SetCanWhiteCastleKingSide(false);
		position.SetCanWhiteCastleQueenSide(false);
	}
	if (!position.IsWhiteToPlay() && (move2.m_From.m_Type == Position::PieceType::King))
	{
		position.SetCanBlackCastleKingSide(false);
		position.SetCanBlackCastleQueenSide(false);
	}

	position.GetMoves().push_back(move2);
}
