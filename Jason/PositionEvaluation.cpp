#include "PositionEvaluation.h"
#include "MoveMaker.h"
#include "MoveSearcher.h"
#include <set>

double PositionEvaluation::EvaluatePosition(const Position& position, int depth)
{
	double bestScore = 0.0;
	//from one position, evaluate all possible positions by checking every legal move! (much computation)

	//if position is WhiteToPlay, possiblePositions are BlackToPlay
	//score given is signed
	//we want to check if a possible move from white has a high score (like mate!)
	//the returned value should be that high score
	//THERE MIGHT BE A BUG FOR EVEN NUMBER DEPTH
	int count = 0;
	std::vector<Position> possiblePositions = MoveSearcher::GetAllPossiblePositions(position, depth);
	for (const Position& possiblePosition : possiblePositions)
	{
		double score = EvaluatePosition(possiblePosition); //score is signed
		bestScore = position.IsWhiteToPlay() ? std::max(bestScore, score) : std::min(bestScore, score);
		count++;
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
	std::set<int> whiteControlledSquares;
	std::set<int> blackControlledSquares;
	for (const Position::Piece& piece : whitePieces)
	{
		std::vector<Position::Move> moves = MoveSearcher::GetLegalMoves(position, piece, true); //legal moves doesnt take into account pawn captures...
		for (const Position::Move& move : moves)
		{
			whiteControlledSquares.insert(move.m_To.m_Position[0] + 8*move.m_To.m_Position[1]);
		}
	}
	for (const Position::Piece& piece : blackPieces)
	{
		std::vector<Position::Move> moves = MoveSearcher::GetLegalMoves(position, piece, false);
		for (const Position::Move& move : moves)
		{
			blackControlledSquares.insert(move.m_To.m_Position[0] + 8 * move.m_To.m_Position[1]);
		}
	}

	score -= (blackControlledSquares.size() / 64.0) * 3.0;
	score += (whiteControlledSquares.size() / 64.0) * 3.0;
	//Check space behind pawns
	//Check control of center
	//Check king in check?

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
