#include "pch.h"
#include "MoveSearcher.h"
#include "MoveMaker.h"
#include <assert.h>
#include <iterator>

std::vector<Move> MoveSearcher::GetLegalMoves(const Position& position)
{
	std::vector<Move> allLegalMoves;

	const std::vector<Piece>& piecesToMove = position.IsWhiteToPlay() ? position.GetWhitePiecesList() : position.GetBlackPiecesList();
	for (const Piece& piece : piecesToMove)
	{
		//Get Legal moves
		std::vector<Move> moves = MoveSearcher::GetLegalMoves(position, piece, position.IsWhiteToPlay());
		allLegalMoves.insert(allLegalMoves.end(),
			std::make_move_iterator(moves.begin()),
			std::make_move_iterator(moves.end()));
	}

	return allLegalMoves;
}

std::vector<Move> MoveSearcher::GetLegalMoves(const Position& position, const Piece& piece, bool isWhitePiece)
{
	std::vector<std::array<int, 2>> accessibleSquares = GetAccessibleSquares(position, piece, isWhitePiece);
	
	//check and keep legal moves
	std::vector<std::array<int, 2>> legalSquares;
	for (const std::array<int, 2> & square : accessibleSquares)
	{
		//check square is not blocked by piece
		bool isBlocked = false;
		//check friendly pieces
		const std::vector<Piece>& friendlyPieces = isWhitePiece ? position.GetWhitePiecesList() : position.GetBlackPiecesList();	
		for (const Piece& friendlyPiece : friendlyPieces)
		{
			if ((piece.m_Square != friendlyPiece.m_Square) && //dont check itself! /!\ may not share same address
				IsMoveBlocked(friendlyPiece, piece, square))
			{
				isBlocked = true;
				break;
			}
		}

		if (isBlocked)
			continue;

		//check enemy pieces
		const std::vector<Piece>& enemyPieces = isWhitePiece ? position.GetBlackPiecesList() : position.GetWhitePiecesList();
		for (const Piece& enemyPiece : enemyPieces)
		{
			if (IsMoveBlocked(enemyPiece, piece, square))
			{
				//Check if can take (not blocked by other enemy pieces)
				//todo: Could optimize if pieces are adjacent
				if ((enemyPiece.Position() == square) &&
					!((piece.m_Type == PieceType::King) && (abs(piece.Position()[0] - square[0]) > 1)) && //castles can't take!
					((piece.m_Type != PieceType::Pawn) || (piece.Position()[0] != enemyPiece.Position()[0]))) //pawn can only take sideways
				{
					for (const Piece& enemyPiece2 : enemyPieces)
					{
						if ((&enemyPiece != &enemyPiece2) &&
							IsMoveBlocked(enemyPiece2, piece, enemyPiece.Position()))
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
		Move move;
		move.m_From = piece;
		move.m_To = piece;
		move.m_To.m_Square = static_cast<Square>(square[0] + 8 * square[1]);
		if (IsMoveIllegal(position, move, isWhitePiece))
			continue;

		legalSquares.push_back(square);
	}

	std::vector<Move> legalMoves;
	for (const std::array<int, 2> & legalSquare : legalSquares)
	{
		Move move;
		move.m_From = piece;
		move.m_To = piece;
		move.m_To.m_Square = legalSquare[0] + 8* legalSquare[1];
		//Add all possible queening moves if pawn
		if ((piece.m_Type == PieceType::Pawn) &&
			((isWhitePiece && (move.m_To.Position()[1] == 7)) || (!isWhitePiece && (move.m_To.Position()[1] == 0))))
		{
			move.m_To.m_Type = PieceType::Queen;
			legalMoves.emplace_back(move);
			move.m_To.m_Type = PieceType::Rook;
			legalMoves.emplace_back(move);
			move.m_To.m_Type = PieceType::Bishop;
			legalMoves.emplace_back(move);
			move.m_To.m_Type = PieceType::Knight;
			legalMoves.emplace_back(move);
		}
		else
			legalMoves.emplace_back(move);
	}

	return legalMoves;
}

std::vector<std::array<int, 2>> MoveSearcher::GetAccessibleSquares(const Position& position, const Piece& piece, bool isWhitePiece)
{
	std::vector<std::array<int, 2>> accessibleSquares;
	switch (piece.m_Type)
	{
	case PieceType::Pawn://Pawn is a special case
	{
		if (isWhitePiece && (piece.Position()[1] < 7) && (piece.Position()[1] >= 1)) //cannot move from last row
		{
			accessibleSquares.push_back({ piece.Position()[0], piece.Position()[1] + 1 });
			if (piece.Position()[1] == 1) //two steps
				accessibleSquares.push_back({ piece.Position()[0], piece.Position()[1] + 2 });
		}
		else if (!isWhitePiece && (piece.Position()[1] <= 6) && (piece.Position()[1] > 0))
		{
			accessibleSquares.push_back({ piece.Position()[0], piece.Position()[1] - 1 });
			if (piece.Position()[1] == 6) //two steps
				accessibleSquares.push_back({ piece.Position()[0], piece.Position()[1] - 2 });
		}
		//Add en-passant square
		if (position.GetEnPassantSquare().has_value())
		{
			int enPassantSquare = *position.GetEnPassantSquare();
			const std::array<int, 2>& square = { enPassantSquare % 8, enPassantSquare / 8 };
			if (abs(square[0] - piece.Position()[0]) == 1)
			{
				const int dy = (square[1] - piece.Position()[1]);
				if ((isWhitePiece && dy == 1) || (!isWhitePiece && dy == -1))
					accessibleSquares.push_back(square);
			}
		}
		//Add other possible captures
		size_t count = 0;
		const std::vector<Piece>& enemyPieces = isWhitePiece ? position.GetBlackPiecesList() : position.GetWhitePiecesList();
		for (const Piece& enemyPiece : enemyPieces)
		{
			if (abs(enemyPiece.Position()[0] - piece.Position()[0]) == 1)
			{
				const int dy = (enemyPiece.Position()[1] - piece.Position()[1]);
				if (isWhitePiece && dy == 1 || !isWhitePiece && dy == -1)
				{
					accessibleSquares.push_back(enemyPiece.Position());
					count++;
				}
			}

			if (count == 2)
				break;
		}

		break;
	}
	case PieceType::King:
		if (piece.Position()[1] < 7)
		{
			accessibleSquares.push_back({ piece.Position()[0], piece.Position()[1] + 1 });
			if (piece.Position()[0] >= 1)
				accessibleSquares.push_back({ piece.Position()[0] - 1, piece.Position()[1] + 1 });
			if (piece.Position()[0] <= 6)
				accessibleSquares.push_back({ piece.Position()[0] + 1, piece.Position()[1] + 1 });
		}
		if (piece.Position()[1] > 0)
		{
			accessibleSquares.push_back({ piece.Position()[0], piece.Position()[1] - 1 });
			if (piece.Position()[0] >= 1)
				accessibleSquares.push_back({ piece.Position()[0] - 1, piece.Position()[1] - 1 });
			if (piece.Position()[0] <= 6)
				accessibleSquares.push_back({ piece.Position()[0] + 1, piece.Position()[1] - 1 });
		}
		if (piece.Position()[0] > 0)
			accessibleSquares.push_back({ piece.Position()[0] - 1, piece.Position()[1] });
		if (piece.Position()[0] < 7)
			accessibleSquares.push_back({ piece.Position()[0] + 1, piece.Position()[1] });

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
	case PieceType::Knight:
		if (piece.Position()[1] < 6)
		{
			if (piece.Position()[0] >= 1)
				accessibleSquares.push_back({ piece.Position()[0] - 1, piece.Position()[1] + 2 });
			if (piece.Position()[0] <= 6)
				accessibleSquares.push_back({ piece.Position()[0] + 1, piece.Position()[1] + 2 });
		}
		if (piece.Position()[1] > 1)
		{
			if (piece.Position()[0] >= 1)
				accessibleSquares.push_back({ piece.Position()[0] - 1, piece.Position()[1] - 2 });
			if (piece.Position()[0] <= 6)
				accessibleSquares.push_back({ piece.Position()[0] + 1, piece.Position()[1] - 2 });
		}
		if (piece.Position()[0] > 1)
		{
			if (piece.Position()[1] >= 1)
				accessibleSquares.push_back({ piece.Position()[0] - 2, piece.Position()[1] - 1 });
			if (piece.Position()[1] <= 6)
				accessibleSquares.push_back({ piece.Position()[0] - 2, piece.Position()[1] + 1 });
		}
		if (piece.Position()[0] < 6)
		{
			if (piece.Position()[1] >= 1)
				accessibleSquares.push_back({ piece.Position()[0] + 2, piece.Position()[1] - 1 });
			if (piece.Position()[1] <= 6)
				accessibleSquares.push_back({ piece.Position()[0] + 2, piece.Position()[1] + 1 });
		}
		break;
	case PieceType::Queen:
	case PieceType::Rook:
		//Along X
		for (int x = 0; x <= 7; x++)
		{
			if (x != piece.Position()[0])
				accessibleSquares.push_back({ x, piece.Position()[1] });
		}
		//Along Y
		for (int y = 0; y <= 7; y++)
		{
			if (y != piece.Position()[1])
				accessibleSquares.push_back({ piece.Position()[0], y });
		}
		if (piece.m_Type == PieceType::Rook)
			break; //continue for queen
		[[fallthrough]];
	case PieceType::Bishop:
	{
		constexpr std::array<int, 2> directions = { 1, -1 };
		for (int u : directions)
		{
			for (int d = 1; d <= 7; d++)
			{
				if (((u > 0) && (piece.Position()[0] + d * u <= 7)) ||
					((u < 0) && (piece.Position()[0] + d * u >= 0)))
				{
					for (int v : directions)
					{
						if (((v > 0) && (piece.Position()[1] + d * v <= 7)) ||
							((v < 0) && (piece.Position()[1] + d * v >= 0)))
						{
							accessibleSquares.push_back({ piece.Position()[0] + d * u, piece.Position()[1] + d * v });
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

bool MoveSearcher::IsMoveBlocked(const Piece& blockingPiece, const Piece& piece, const std::array<int, 2>& square)
{
	bool isBlocking = false;

	switch (piece.m_Type)
	{
	case PieceType::King:
	{
		if ((square[0] - piece.Position()[0]) > 1) //kingside castle
		{
			if (std::array<int, 2>{square[0] - 1, square[1]} == blockingPiece.Position()) //check in between square
				isBlocking = true;
		}
		else if ((square[0] - piece.Position()[0]) < -1) //queenside castle
		{
			if (std::array<int, 2>{square[0] - 1, square[1]} == blockingPiece.Position())
				isBlocking = true;
			if (std::array<int, 2>{square[0] + 1, square[1]} == blockingPiece.Position())
				isBlocking = true;
		}
	}
	[[fallthrough]];
	case PieceType::Knight:
	case PieceType::Pawn:
		if (square == blockingPiece.Position())
			isBlocking = true;
		if (piece.m_Type == PieceType::Pawn && abs(square[1] - piece.Position()[1]) > 1) //double square move
		{
			if ((square[1] - piece.Position()[1]) > 0 && std::array<int, 2>{square[0], square[1] - 1} == blockingPiece.Position()) //white pawn
				isBlocking = true;
			if ((square[1] - piece.Position()[1]) < 0 && std::array<int, 2>{square[0], square[1] + 1} == blockingPiece.Position()) //black pawn
				isBlocking = true;
		}
		break;
	case PieceType::Queen:
	case PieceType::Rook:
	{
		constexpr std::array<bool, 2> checkRow = { false, true };
		for (bool row : checkRow)
		{
			const size_t i = (!row) * 1; //must be 0 for row (X
			const size_t j = row * 1; //must be 1 for row (Y)

			//j = const => we check row
			if ((square[j] == blockingPiece.Position()[j]) && (square[j] == piece.Position()[j]))
			{
				const int min = std::min(piece.Position()[i], square[i]);
				const int max = std::max(piece.Position()[i], square[i]);
				if (min <= blockingPiece.Position()[i] && blockingPiece.Position()[i] <= max)
				{
					isBlocking = true;
					break;
				}
			}
		}
		if (piece.m_Type == PieceType::Rook)
			break;
	}
	[[fallthrough]];
	case PieceType::Bishop:
	{
		//check same diagonal
		if ((abs(piece.Position()[0] - blockingPiece.Position()[0]) == abs(piece.Position()[1] - blockingPiece.Position()[1])) && //same diagonal
			((abs(piece.Position()[0] - square[0]) == abs(piece.Position()[1] - square[1]))) &&
			((abs(blockingPiece.Position()[0] - square[0]) == abs(blockingPiece.Position()[1] - square[1]))))
		{
			//check blocking piece in between (only have to check X or Y as they're already on same diagonal)
			const int min = std::min(piece.Position()[0], square[0]);
			const int max = std::max(piece.Position()[0], square[0]);
			if (min <= blockingPiece.Position()[0] && blockingPiece.Position()[0] <= max)
				isBlocking = true;
		}

		break;
	}
	default:
		assert(false); //invalid type!
	}

	return isBlocking;
}

bool MoveSearcher::IsMoveIllegal(const Position& position, const Move& move, bool isWhitePiece)
{
	//const std::array<int, 2>&

	//we check new position with moved piece
	Position newPosition = position;

	Move moveCopy = move;
	newPosition.Update(moveCopy);

	bool isIllegal = IsKingInCheck(newPosition, isWhitePiece);

	//Also check castling moves
	if (!isIllegal && move.IsCastling())
	{
		if (IsKingInCheck(position, isWhitePiece))
			isIllegal = true;
		else if (move.m_To.m_Square > move.m_From.m_Square) //kingside castle, check in between square
		{
			newPosition = position;
			moveCopy.m_To.m_Square--;
			newPosition.Update(moveCopy);
			isIllegal = IsKingInCheck(newPosition, isWhitePiece);
		}
		else //queenside
		{
			newPosition = position;
			moveCopy.m_To.m_Square++;
			newPosition.Update(moveCopy);
			isIllegal = IsKingInCheck(newPosition, isWhitePiece);
		}
	}

	return isIllegal;
}

std::vector<Position> MoveSearcher::GetAllPossiblePositions(const Position& position)
{
	std::vector<Position> positions;

	const std::vector<Piece>& piecesToPlay = position.IsWhiteToPlay() ? position.GetWhitePiecesList() : position.GetBlackPiecesList();
	for (const Piece& piece : piecesToPlay)
	{
		std::vector<Move> pieceMoves = GetLegalMoves(position, piece, position.IsWhiteToPlay());
		for (Move& move : pieceMoves)
		{
			Position newPosition = position;
			newPosition.Update(move);
			positions.emplace_back(std::move(newPosition));
		}
	}

	return positions;
}

std::vector<Position> MoveSearcher::GetAllPossiblePositions(const Position& position, int depth)
{
	if (depth == 0)
		return { position };

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

std::unordered_set<Position> MoveSearcher::GetAllUniquePositions(const Position& position, int depth)
{
	if (depth == 0)
		return { position };

	std::unordered_set<Position> deeperPositions;
	std::vector<Position> possiblePositions = GetAllPossiblePositions(position);
	if (depth > 1)
	{
		for (const Position& possiblePosition : possiblePositions)
		{
			std::unordered_set<Position> positions = GetAllUniquePositions(possiblePosition, depth - 1);
			deeperPositions.insert(positions.begin(), positions.end());
		}
	}
	else
	{
		std::copy(possiblePositions.begin(), possiblePositions.end(), std::inserter(deeperPositions, deeperPositions.end()));
	}

	return deeperPositions;
}

static constexpr int maxDepth = 3;

std::vector<Move> MoveSearcher::GetAllLineMoves(const Position& position)
{
	std::vector<Move> linesMoves;
	std::vector<Move> moves = GetLegalMoves(position);
	const size_t piecesCount = position.GetBlackPiecesList().size() + position.GetWhitePiecesList().size();

	//for (Move& move : moves)
	//{
	//	//Keep new position only when we want to check a tactic (after capture, forced move, pieces aligned, king undefended...)
	//	//Make move and undo move to get piece count
	//	
	//	const size_t piecesCount2 = newPosition.GetBlackPiecesList().size() + newPosition.GetWhitePiecesList().size();
	//	if (piecesCount != piecesCount2)
	//		lines.emplace_back(std::move(newPosition));
	//}

	return linesMoves;
}

bool MoveSearcher::IsKingInCheck(const Position& position, bool isWhitePiece)
{
	bool isKingInCheck = false;
	const std::vector<Piece>& enemyPieces = isWhitePiece ? position.GetBlackPiecesList() : position.GetWhitePiecesList();
	const std::vector<Piece>& friendlyPieces = isWhitePiece ? position.GetWhitePiecesList() : position.GetBlackPiecesList();
	const Piece* king = nullptr;

	for (const Piece& friendlyPiece : friendlyPieces)
	{
		if (friendlyPiece.m_Type == PieceType::King)
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

	for (const Piece& enemyPiece : enemyPieces)
	{
		bool canGetToKing = false;
		std::vector<std::array<int, 2>> accessibleSquares = GetAccessibleSquares(position, enemyPiece, !isWhitePiece);
		for (const std::array<int, 2> & square : accessibleSquares)
		{
			//For pawns, ignore squares in front of them (can't give a check)
			if ((enemyPiece.m_Type == PieceType::Pawn) && enemyPiece.Position()[0] == square[0])
				continue;

			if (square == king->Position())
			{
				canGetToKing = true;
				break;
			}
		}

		if (!canGetToKing)
			continue;

		bool protectedByFriend = false;
		for (const Piece& friendlyPiece : friendlyPieces)
		{
			if ((&friendlyPiece != king) && IsMoveBlocked(friendlyPiece, enemyPiece, king->Position()))
			{
				protectedByFriend = true;
				break;
			}
		}

		if (protectedByFriend)
			continue;

		bool protectedByEnemy = false;
		for (const Piece& enemyPiece2 : enemyPieces)
		{
			if ((&enemyPiece != &enemyPiece2) && IsMoveBlocked(enemyPiece2, enemyPiece, king->Position()))
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

std::optional<Move> MoveSearcher::GetRandomMove(const Position& position)
{
	std::optional<Move> move;
	std::vector<Move> allLegalMoves = MoveSearcher::GetLegalMoves(position);
	if (allLegalMoves.empty())
		return move;

	const int rand = std::rand();
	move = allLegalMoves[rand % allLegalMoves.size()];
	return move;
}
