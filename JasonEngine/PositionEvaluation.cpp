#include "pch.h"
#include "PositionEvaluation.h"
#include "MoveMaker.h"
#include "MoveSearcher.h"

double PositionEvaluation::EvaluatePosition(const Position& position, int depth)
{
	double bestScore = 0.0;
	//from one position, evaluate all possible positions by checking every legal move! (much computation)

	//if position is WhiteToPlay, possiblePositions are BlackToPlay
	//score given is signed
	//we want to check if a possible move from white has a high score (like mate!)
	//the returned value should be that high score
	//THERE MIGHT BE A BUG FOR EVEN NUMBER DEPTH
	//BUG: doesnt prevent mate in 1 anymore...
	int count = 0;
	const bool reduceCpu = true;
	std::vector<Position> possiblePositions = MoveSearcher::GetAllLinesPositions(position, depth);
	if (!possiblePositions.empty())
	{
		for (const Position& possiblePosition : possiblePositions)
		{
			double score = EvaluatePosition(possiblePosition); //score is signed
			bestScore = position.IsWhiteToPlay() ? std::max(bestScore, score) : std::min(bestScore, score);
			count++;
		}
	}
	else
	{
		//Stalemate or Mate, can't go deeper
		bestScore = EvaluatePosition(position);
	}

	return bestScore;
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
		return position.IsWhiteToPlay() ? std::numeric_limits<double>::lowest() : std::numeric_limits<double>::max();
	default:
		break;
	}

	double score = 0.0;
	//Check material
	const std::vector<Position::Piece>& whitePieces = position.GetWhitePieces();
	const std::vector<Position::Piece>& blackPieces = position.GetBlackPieces();
	double whiteMaterial = 0.0;
	double blackMaterial = 0.0;
	for (const Position::Piece& piece : whitePieces)
	{
		whiteMaterial += GetPieceValue(piece.m_Type);
	}
	for (const Position::Piece& piece : blackPieces)
	{
		blackMaterial += GetPieceValue(piece.m_Type);
	}

	score = whiteMaterial - blackMaterial;

	//Check double pawns
	int whiteDoubledPawns = CountDoubledPawn(position, true);
	int blackDoubledPawns = CountDoubledPawn(position, false);

	score -= static_cast<double>(whiteDoubledPawns) * 0.33;
	score += static_cast<double>(blackDoubledPawns) * 0.33;

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
	//Check concentration of pieces around enemy king for an attack?

	return score;
}

double PositionEvaluation::GetPieceValue(Position::PieceType type)
{
	double value = 0.0;
	switch (type)
	{
	case Position::PieceType::Queen:
		value = 9;
		break;
	case Position::PieceType::Rook:
		value = 5;
		break;
	case Position::PieceType::Bishop:
		value = 3;
		break;
	case Position::PieceType::Knight:
		value = 3;
		break;
	case Position::PieceType::Pawn:
		value = 1;
		break;
	default:
		break;
	}

	return value;
}

int PositionEvaluation::CountDoubledPawn(const Position& position, bool isWhite)
{
	int count = 0;
	const std::vector<Position::Piece>& pieces = isWhite ? position.GetWhitePieces() : position.GetBlackPieces();
	
	std::set<int> checkedFiles;
	for (const Position::Piece& piece : pieces)
	{
		if (piece.m_Type == Position::PieceType::Pawn)
		{
			if (checkedFiles.find(piece.m_Position[0]) != checkedFiles.end())
				continue;

			checkedFiles.insert(piece.m_Position[0]);
			for (const Position::Piece& piece2 : pieces)
			{
				if ((&piece != &piece2) && (piece2.m_Type == Position::PieceType::Pawn) && (piece.m_Position[0] == piece2.m_Position[0]))
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

std::set<int> PositionEvaluation::GetControlledSquares(const Position& position, const Position::Piece& piece, bool isWhite)
{
	std::set<int> controlledSquares;
	if (piece.m_Type == Position::PieceType()) //Pawn is special case
	{
		const int direction = isWhite ? 1 : -1;
		const int nextY = piece.m_Position[1] + 1 * direction;
		if (nextY > 0 && nextY < 7)
		{
			if ((piece.m_Position[0] > 0))
				controlledSquares.insert( SquareToIdx({piece.m_Position[0] - 1, piece.m_Position[1] + 1 * direction }));

			if (piece.m_Position[0] < 7)
				controlledSquares.insert( SquareToIdx({piece.m_Position[0] + 1, piece.m_Position[1] + 1 * direction }));
		}
	}
	else
	{
		std::vector<Position::Move> moves = MoveSearcher::GetLegalMoves(position, piece, true);
		for (const Position::Move& move : moves)
		{
			controlledSquares.insert(SquareToIdx(move.m_To.m_Position));
		}
	}

	return controlledSquares;
}

std::set<int> PositionEvaluation::GetControlledSquares(const Position& position, bool isWhite, std::set<int>& byPawn)
{
	std::set<int> controlledSquares;
	byPawn.clear();

	const std::vector<Position::Piece>& pieces = isWhite ? position.GetWhitePieces() : position.GetBlackPieces();
	for (const Position::Piece& piece : pieces)
	{
		const std::set<int> squares = GetControlledSquares(position, piece, isWhite);
		for (int square : squares)
		{
			controlledSquares.insert(square); //set controlled by pawn flag
			if (piece.m_Type == Position::PieceType::Pawn)
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
