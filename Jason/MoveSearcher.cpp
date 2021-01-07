#include "MoveSearcher.h"
#include "MoveMaker.h"
#include <assert.h>

std::vector<Position::Move> MoveSearcher::GetLegalMoves(const Position& position, const Position::Piece& piece, bool isWhitePiece)
{
	std::vector<std::array<int, 2>> accessibleSquares = GetAccessibleSquares(position, piece, isWhitePiece);
	
	//check and keep legal moves
	std::vector<std::array<int, 2>> legalSquares;
	for (const std::array<int, 2> & square : accessibleSquares)
	{
		//check square is not blocked by piece
		bool isBlocked = false;
		//check friendly pieces
		const std::vector<Position::Piece>& friendlyPieces = isWhitePiece ? position.GetWhitePieces() : position.GetBlackPieces();	
		for (const Position::Piece& friendlyPiece : friendlyPieces)
		{
			if ((piece.m_Position != friendlyPiece.m_Position) && //dont check itself! /!\ may not share same address
				IsMoveBlocked(friendlyPiece, piece, square))
			{
				isBlocked = true;
				break;
			}
		}

		if (isBlocked)
			continue;

		//check enemy pieces
		const std::vector<Position::Piece>& enemyPieces = isWhitePiece ? position.GetBlackPieces() : position.GetWhitePieces();
		for (const Position::Piece& enemyPiece : enemyPieces)
		{
			if (IsMoveBlocked(enemyPiece, piece, square))
			{
				//Check if can take (not blocked by other enemy pieces)
				if ((enemyPiece.m_Position == square) &&
					((piece.m_Type != Position::PieceType::Pawn) || (piece.m_Position[0] != enemyPiece.m_Position[0]))) //pawn can only take sideways
				{
					for (const Position::Piece& enemyPiece2 : enemyPieces)
					{
						if ((&enemyPiece != &enemyPiece2) &&
							IsMoveBlocked(enemyPiece2, piece, enemyPiece.m_Position))
						{
							isBlocked = true;
							break;
						}
					}
				}
				else
					isBlocked = true;

				break;
			}
		}

		if (isBlocked)
			continue;

		//move is not blocked by collision, check if illegal move
		if (IsMoveIllegal(position, piece, isWhitePiece, square))
			continue;

		legalSquares.push_back(square);
	}

	std::vector<Position::Move> legalMoves;
	for (const std::array<int, 2> & legalSquare : legalSquares)
	{
		Position::Move move;
		move.m_From = piece;
		move.m_To = piece;
		move.m_To.m_Position = legalSquare;
		//Add all possible queening moves if pawn
		if ((piece.m_Type == Position::PieceType::Pawn) &&
			((isWhitePiece && (move.m_To.m_Position[1] == 7)) || (!isWhitePiece && (move.m_To.m_Position[1] == 0))))
		{
			move.m_To.m_Type = Position::PieceType::Queen;
			legalMoves.push_back(move);
			move.m_To.m_Type = Position::PieceType::Rook;
			legalMoves.push_back(move);
			move.m_To.m_Type = Position::PieceType::Bishop;
			legalMoves.push_back(move);
			move.m_To.m_Type = Position::PieceType::Knight;
			legalMoves.push_back(move);
		}
		else
			legalMoves.push_back(move);
	}

	return legalMoves;
}

std::vector<std::array<int, 2>> MoveSearcher::GetAccessibleSquares(const Position& position, const Position::Piece& piece, bool isWhitePiece)
{
	std::vector<std::array<int, 2>> accessibleSquares;
	switch (piece.m_Type)
	{
	case Position::PieceType::Pawn://Pawn is a special case
	{
		if (isWhitePiece && (piece.m_Position[1] < 7) && (piece.m_Position[1] >= 1)) //cannot move from last row
		{
			accessibleSquares.push_back({ piece.m_Position[0], piece.m_Position[1] + 1 });
			if (piece.m_Position[1] == 1) //two steps
				accessibleSquares.push_back({ piece.m_Position[0], piece.m_Position[1] + 2 });
		}
		else if (!isWhitePiece && (piece.m_Position[1] <= 6) && (piece.m_Position[1] > 0))
		{
			accessibleSquares.push_back({ piece.m_Position[0], piece.m_Position[1] - 1 });
			if (piece.m_Position[1] == 6) //two steps
				accessibleSquares.push_back({ piece.m_Position[0], piece.m_Position[1] - 2 });
		}
		//Add en-passant square
		if (position.GetEnPassantSquare().has_value())
		{
			const std::array<int, 2>& square = *position.GetEnPassantSquare();
			if (abs(square[0] - piece.m_Position[0]) == 1)
			{
				const int dy = (square[1] - piece.m_Position[1]);
				if ((isWhitePiece && dy == 1) || (!isWhitePiece && dy == -1))
					accessibleSquares.push_back(square);
			}
		}
		//Add other possible captures
		size_t count = 0;
		const std::vector<Position::Piece>& enemyPieces = isWhitePiece ? position.GetBlackPieces() : position.GetWhitePieces();
		for (const Position::Piece& enemyPiece : enemyPieces)
		{
			if (abs(enemyPiece.m_Position[0] - piece.m_Position[0]) == 1)
			{
				const int dy = (enemyPiece.m_Position[1] - piece.m_Position[1]);
				if (isWhitePiece && dy == 1 || !isWhitePiece && dy == -1)
				{
					accessibleSquares.push_back(enemyPiece.m_Position);
					count++;
				}
			}

			if (count == 2)
				break;
		}

		break;
	}
	case Position::PieceType::King:
		if (piece.m_Position[1] < 7)
		{
			accessibleSquares.push_back({ piece.m_Position[0], piece.m_Position[1] + 1 });
			if (piece.m_Position[0] >= 1)
				accessibleSquares.push_back({ piece.m_Position[0] - 1, piece.m_Position[1] + 1 });
			if (piece.m_Position[0] <= 6)
				accessibleSquares.push_back({ piece.m_Position[0] + 1, piece.m_Position[1] + 1 });
		}
		if (piece.m_Position[1] > 0)
		{
			accessibleSquares.push_back({ piece.m_Position[0], piece.m_Position[1] - 1 });
			if (piece.m_Position[0] >= 1)
				accessibleSquares.push_back({ piece.m_Position[0] - 1, piece.m_Position[1] - 1 });
			if (piece.m_Position[0] <= 6)
				accessibleSquares.push_back({ piece.m_Position[0] + 1, piece.m_Position[1] - 1 });
		}
		if (piece.m_Position[0] > 0)
			accessibleSquares.push_back({ piece.m_Position[0] - 1, piece.m_Position[1] });
		if (piece.m_Position[0] < 7)
			accessibleSquares.push_back({ piece.m_Position[0] + 1, piece.m_Position[1] });

		//Add castling moves, will be ignored if illegal
		if (isWhitePiece && position.CanWhiteCastleKingSide())
			accessibleSquares.push_back({ 6, 0 });
		if (isWhitePiece && position.CanWhiteCastleQueenSide())
			accessibleSquares.push_back({ 2, 0 });
		if (!isWhitePiece && position.CanBlackCastleKingSide())
			accessibleSquares.push_back({ 6, 7 });
		if (!isWhitePiece && position.CanBlackCastleQueenSide())
			accessibleSquares.push_back({ 2, 7 });

		break;
	case Position::PieceType::Knight:
		if (piece.m_Position[1] < 6)
		{
			if (piece.m_Position[0] >= 1)
				accessibleSquares.push_back({ piece.m_Position[0] - 1, piece.m_Position[1] + 2 });
			if (piece.m_Position[0] <= 6)
				accessibleSquares.push_back({ piece.m_Position[0] + 1, piece.m_Position[1] + 2 });
		}
		if (piece.m_Position[1] > 1)
		{
			if (piece.m_Position[0] >= 1)
				accessibleSquares.push_back({ piece.m_Position[0] - 1, piece.m_Position[1] - 2 });
			if (piece.m_Position[0] <= 6)
				accessibleSquares.push_back({ piece.m_Position[0] + 1, piece.m_Position[1] - 2 });
		}
		if (piece.m_Position[0] > 1)
		{
			if (piece.m_Position[1] >= 1)
				accessibleSquares.push_back({ piece.m_Position[0] - 2, piece.m_Position[1] - 1 });
			if (piece.m_Position[1] <= 6)
				accessibleSquares.push_back({ piece.m_Position[0] - 2, piece.m_Position[1] + 1 });
		}
		if (piece.m_Position[0] < 6)
		{
			if (piece.m_Position[1] >= 1)
				accessibleSquares.push_back({ piece.m_Position[0] + 2, piece.m_Position[1] - 1 });
			if (piece.m_Position[1] <= 6)
				accessibleSquares.push_back({ piece.m_Position[0] + 2, piece.m_Position[1] + 1 });
		}
		break;
	case Position::PieceType::Queen:
	case Position::PieceType::Rook:
		//Along X
		for (int x = 0; x <= 7; x++)
		{
			if (x != piece.m_Position[0])
				accessibleSquares.push_back({ x, piece.m_Position[1] });
		}
		//Along Y
		for (int y = 0; y <= 7; y++)
		{
			if (y != piece.m_Position[1])
				accessibleSquares.push_back({ piece.m_Position[0], y });
		}
		if (piece.m_Type == Position::PieceType::Rook)
			break; //continue for queen
		[[fallthrough]];
	case Position::PieceType::Bishop:
	{
		constexpr std::array<int, 2> directions = { 1, -1 };
		for (int u : directions)
		{
			for (int d = 1; d <= 7; d++)
			{
				if (((u > 0) && (piece.m_Position[0] + d * u <= 7)) ||
					((u < 0) && (piece.m_Position[0] + d * u >= 0)))
				{
					for (int v : directions)
					{
						if (((v > 0) && (piece.m_Position[1] + d * v <= 7)) ||
							((v < 0) && (piece.m_Position[1] + d * v >= 0)))
						{
							accessibleSquares.push_back({ piece.m_Position[0] + d * u, piece.m_Position[1] + d * v });
						}
					}
				}
				else
					break;
			}
		}
		break;
	}
	default:
		assert(false); //Invalid type!
	}

	return accessibleSquares;
}

bool MoveSearcher::IsMoveBlocked(const Position::Piece& blockingPiece, const Position::Piece& piece, const std::array<int, 2>& square)
{
	bool isBlocking = false;

	switch (piece.m_Type)
	{
	case Position::PieceType::King:
	{
		if ((square[0] - piece.m_Position[0]) > 1) //kingside castle
		{
			if (std::array<int, 2>{square[0] - 1, square[1]} == blockingPiece.m_Position) //check in between square
				isBlocking = true;
		}
		else if ((square[0] - piece.m_Position[0]) < -1) //queenside castle
		{
			if (std::array<int, 2>{square[0] - 1, square[1]} == blockingPiece.m_Position)
				isBlocking = true;
			if (std::array<int, 2>{square[0] + 1, square[1]} == blockingPiece.m_Position)
				isBlocking = true;
		}
	}
	[[fallthrough]];
	case Position::PieceType::Knight:
	case Position::PieceType::Pawn:
		if (square == blockingPiece.m_Position)
			isBlocking = true;
		if (piece.m_Type == Position::PieceType::Pawn && abs(square[1] - piece.m_Position[1]) > 1) //double square move
		{
			if ((square[1] - piece.m_Position[1]) > 0 && std::array<int, 2>{square[0], square[1] - 1} == blockingPiece.m_Position) //white pawn
				isBlocking = true;
			if ((square[1] - piece.m_Position[1]) < 0 && std::array<int, 2>{square[0], square[1] + 1} == blockingPiece.m_Position) //black pawn
				isBlocking = true;
		}
		break;
	case Position::PieceType::Queen:
	case Position::PieceType::Rook:
	{
		constexpr std::array<bool, 2> checkRow = { false, true };
		for (bool row : checkRow)
		{
			const size_t i = (!row) * 1; //must be 0 for row (X
			const size_t j = row * 1; //must be 1 for row (Y)

			//j = const => we check row
			if ((square[j] == blockingPiece.m_Position[j]) && (square[j] == piece.m_Position[j]))
			{
				const int min = std::min(piece.m_Position[i], square[i]);
				const int max = std::max(piece.m_Position[i], square[i]);
				if (min <= blockingPiece.m_Position[i] && blockingPiece.m_Position[i] <= max)
				{
					isBlocking = true;
					break;
				}
			}
		}
		if (piece.m_Type == Position::PieceType::Rook)
			break;
	}
	[[fallthrough]];
	case Position::PieceType::Bishop:
	{
		//check same diagonal
		if ((abs(piece.m_Position[0] - blockingPiece.m_Position[0]) == abs(piece.m_Position[1] - blockingPiece.m_Position[1])) && //same diagonal
			((abs(piece.m_Position[0] - square[0]) == abs(piece.m_Position[1] - square[1]))) &&
			((abs(blockingPiece.m_Position[0] - square[0]) == abs(blockingPiece.m_Position[1] - square[1]))))
		{
			//check blocking piece in between (only have to check X or Y as they're already on same diagonal)
			const int min = std::min(piece.m_Position[0], square[0]);
			const int max = std::max(piece.m_Position[0], square[0]);
			if (min <= blockingPiece.m_Position[0] && blockingPiece.m_Position[0] <= max)
				isBlocking = true;
		}

		break;
	}
	default:
		assert(false); //invalid type!
	}

	return isBlocking;
}

bool MoveSearcher::IsMoveIllegal(const Position& position, const Position::Piece& piece, bool isWhitePiece, const std::array<int, 2>& square)
{
	//we check new position with moved piece
	Position newPosition = position;

	Position::Move move;
	move.m_From = piece;
	move.m_To.m_Type = piece.m_Type;
	move.m_To.m_Position = square;
	MoveMaker::UpdatePosition(newPosition, move);

	bool isIllegal = IsKingInCheck(newPosition, isWhitePiece);

	//Also check castling moves
	if (!isIllegal && piece.m_Type == Position::PieceType::King && (abs(piece.m_Position[0] - square[0]) > 1))
	{
		if (IsKingInCheck(position, isWhitePiece))
			isIllegal = true;
		else if ((square[0] - piece.m_Position[0]) > 0) //kingside castle, check in between square
		{
			newPosition = position;
			move.m_To.m_Position = square;
			move.m_To.m_Position[0] -= 1;
			MoveMaker::UpdatePosition(newPosition, move);
			isIllegal = IsKingInCheck(newPosition, isWhitePiece);
		}
		else //queenside
		{
			newPosition = position;
			move.m_To.m_Position = square;
			move.m_To.m_Position[0] += 1;
			MoveMaker::UpdatePosition(newPosition, move);
			isIllegal = IsKingInCheck(newPosition, isWhitePiece);
		}
	}

	return isIllegal;
}

std::vector<Position> MoveSearcher::GetAllPossiblePositions(const Position& position)
{
	std::vector<Position> positions;

	const std::vector<Position::Piece>& piecesToPlay = position.IsWhiteToPlay() ? position.GetWhitePieces() : position.GetBlackPieces();
	for (const Position::Piece& piece : piecesToPlay)
	{
		std::vector<Position::Move> moves = GetLegalMoves(position, piece, position.IsWhiteToPlay());
		for (const Position::Move& move : moves)
		{
			Position newPosition = position;
			MoveMaker::UpdatePosition(newPosition, move);
			newPosition.SetWhiteToPlay(!newPosition.IsWhiteToPlay());
			positions.emplace_back(newPosition);
		}
	}

	return positions;
}

std::vector<Position> MoveSearcher::GetAllPossiblePositions(const Position& position, int depth)
{
	std::vector<Position> deeperPositions;
	std::vector<Position> possiblePositions = GetAllPossiblePositions(position);
	if (depth > 1)
	{
		for (const Position& possiblePosition : possiblePositions)
		{
			std::vector<Position> positions = GetAllPossiblePositions(possiblePosition, depth - 1);
			deeperPositions.insert(deeperPositions.end(), positions.begin(), positions.end());
		}
	}
	else
	{
		deeperPositions = std::move(possiblePositions);
	}

	return deeperPositions;
}

bool MoveSearcher::IsKingInCheck(const Position& position, bool isWhitePiece)
{
	bool isKingInCheck = false;
	const std::vector<Position::Piece>& enemyPieces = isWhitePiece ? position.GetBlackPieces() : position.GetWhitePieces();
	const std::vector<Position::Piece>& friendlyPieces = isWhitePiece ? position.GetWhitePieces() : position.GetBlackPieces();
	const Position::Piece* king = nullptr;

	for (const Position::Piece& friendlyPiece : friendlyPieces)
	{
		if (friendlyPiece.m_Type == Position::PieceType::King)
		{
			king = &friendlyPiece;
			break;
		}
	}

	if (!king)
	{
		assert(false);
		return false;
	}

	for (const Position::Piece& enemyPiece : enemyPieces)
	{
		bool canGetToKing = false;
		std::vector<std::array<int, 2>> accessibleSquares = GetAccessibleSquares(position, enemyPiece, !isWhitePiece);
		for (const std::array<int, 2> & square : accessibleSquares)
		{
			//For pawns, ignore squares in front of them (can't give a check)
			if ((enemyPiece.m_Type == Position::PieceType::Pawn) && enemyPiece.m_Position[0] == square[0])
				continue;

			if (square == king->m_Position)
			{
				canGetToKing = true;
				break;
			}
		}

		if (!canGetToKing)
			continue;

		bool protectedByFriend = false;
		for (const Position::Piece& friendlyPiece : friendlyPieces)
		{
			if ((&friendlyPiece != king) && IsMoveBlocked(friendlyPiece, enemyPiece, king->m_Position))
			{
				protectedByFriend = true;
				break;
			}
		}

		if (protectedByFriend)
			continue;

		bool protectedByEnemy = false;
		for (const Position::Piece& enemyPiece2 : enemyPieces)
		{
			if ((&enemyPiece != &enemyPiece2) && IsMoveBlocked(enemyPiece2, enemyPiece, king->m_Position))
			{
				protectedByEnemy = true;
				break;
			}
		}

		if (protectedByEnemy)
			continue;

		isKingInCheck = true;
		break;
	}

	return isKingInCheck;
}
