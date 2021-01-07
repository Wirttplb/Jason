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

	double bestScore = 0.0;
	Position::Move& bestMove = allLegalMoves.front();
	for (const Position::Move& move : allLegalMoves)
	{
		double score = 0.0; //INSERT SCORE CALCULATION HERE
		if (score > bestScore)
		{
			bestScore = score;
			bestMove = move;
		}
	}

	//FOR NOW, RANDOM MOVE!!
	const int rand = std::rand();
	bestMove = allLegalMoves[rand % allLegalMoves.size()];
	//TO TEST CASTLING MOVES, FORCE TO CASTLE IF POSSIBLE
	for (const Position::Move& move : allLegalMoves)
	{
		if (move.m_From.m_Type == Position::PieceType::King &&
			abs(move.m_From.m_Position[0] - move.m_To.m_Position[0]) > 1)
			bestMove = move;
	}

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

bool MoveMaker::CheckGameOver(Position& position)
{
	//check for insufficient material
	if (position.IsInsufficientMaterial())
	{
		position.SetGameStatus(Position::GameStatus::StaleMate);
		return true;
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
		return true;
	}

	return false;
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

std::string MoveMaker::TranslateToAlgebraic(Position::PieceType type)
{
	std::string pieceString;
	switch (type)
	{
		case Position::PieceType::Pawn:
			pieceString = "P";
			break;
		case Position::PieceType::Rook:
			pieceString = "R";
			break;
		case Position::PieceType::Bishop:
			pieceString = "B";
			break;
		case Position::PieceType::Knight:
			pieceString = "N";
			break;
		case Position::PieceType::Queen:
			pieceString = "Q";
			break;
		case Position::PieceType::King:
			pieceString = "K";
			break;
		default:
			assert(false);
	}

	return pieceString;
}

static char NthLetter(int n)
{
	assert(n >= 0 && n <= 7);// 26);
	constexpr char alphabet[] = "abcdefgh";// ijklmnopqrstuvwxyz";
	return alphabet[n];
}

static int LetterToNumber(char a)
{
	int number = 0;
	if (a == 'a')
		number = 0;
	else if (a == 'b')
		number = 1;
	else if (a == 'c')
		number = 2;
	else if (a == 'd')
		number = 3;
	else if (a == 'e')
		number = 4;
	else if (a == 'f')
		number = 5;
	else if (a == 'g')
		number = 6;
	else if (a == 'h')
		number = 7;
	else
		number = -1;

	return number;
}

static Position::PieceType LetterToPieceType(char A)
{
	Position::PieceType type = Position::PieceType::Pawn;
	if (A == 'Q')
		type = Position::PieceType::Queen;
	else if (A == 'R')
		type = Position::PieceType::Rook;
	else if (A == 'N')
		type = Position::PieceType::Knight;
	else if (A == 'B')
		type = Position::PieceType::Bishop;
	else if (A == 'K')
		type = Position::PieceType::King;
	else
		type = Position::PieceType::Pawn;

	return type;
}

std::string MoveMaker::TranslateToAlgebraic(const std::array<int, 2>& square)
{
	std::string squareString = NthLetter(square[0]) + std::to_string(square[1] + 1);
	return squareString;
}

std::string MoveMaker::TranslateToAlgebraic(const Position::Move& move)
{
	std::string moveString;

	//Check for castling moves
	if (move.m_From.m_Type == Position::PieceType::King &&
		(abs(move.m_To.m_Position[0] - move.m_From.m_Position[0]) > 1))
	{
		if ((move.m_To.m_Position[0] - move.m_From.m_Position[0]) > 0) //kingside
			moveString = "O-O";
		else //queenside
			moveString = "O-O-O";
	}
	else
	{
		//We use Disambiguation for every move (much less complicated)
		moveString = (move.m_From.m_Type == Position::PieceType::Pawn) ? "" : TranslateToAlgebraic(move.m_From.m_Type);
		moveString += TranslateToAlgebraic(move.m_From.m_Position);
		if (move.m_IsCapture)
			moveString += "x";
		moveString += TranslateToAlgebraic(move.m_To.m_Position);
		
		//queening
		if (move.m_From.m_Type != move.m_To.m_Type)
			moveString += "=" + TranslateToAlgebraic(move.m_To.m_Type);
	}

	return moveString;
}

std::optional<Position::Move> MoveMaker::TranslateFromAlgebraic(const Position& position, const std::string& moveString)
{
	std::optional<Position::Move> move;
	if (moveString.size() < 2)
		return move; //invalid length

	std::vector<Position::Piece> piece;
	//castling is easy to parse
	const bool isKingSideCastle = moveString == "O-O";
	const bool isQueenSideCastle = moveString == "O-O-O";
	if (isKingSideCastle || isQueenSideCastle)
	{
		piece = position.GetPiecesToPlay(Position::PieceType::King);
		if (piece.empty())
		{
			assert(false); //no king?!
			return move;
		}

		move = Position::Move();
		move->m_From = piece.front();
		move->m_To = move->m_From;
		move->m_To.m_Position[0] += isKingSideCastle ? 2 : -2;
		return move;
	}

	//first expected char is symbol
	piece = position.GetPiecesToPlay(LetterToPieceType(moveString[0]));
	if (piece.empty())
		return move;

	std::optional<int> fromRow;
	std::optional<int> fromFile;
	std::optional<int> toRow;
	std::optional<int> toFile;
	Position::PieceType toType = piece.front().m_Type;

	switch (moveString.size())
	{
	case 2: //pawn move
	{
		if (piece.front().m_Type != Position::PieceType::Pawn)
			return move;

		if (!isalpha(moveString[0]) || !isdigit(moveString[1]))
			return move;
		fromFile = LetterToNumber(moveString[0]);
		toFile = fromFile;
		const std::string rowString = moveString.substr(1, 1);
		toRow = std::stoi(rowString) - 1;
		break;
	}
	case 3: //pawn takes (abbreviated) or queening (a8=Q) or normal move
	{
		if (moveString[1] == 'x') //pawn takes (abbreviated)
		{
			if (piece.front().m_Type == Position::PieceType::Pawn)
				return move;

			if (!isalpha(moveString[0]) || !isalpha(moveString[2]))
				return move;

			fromFile = LetterToNumber(moveString[0]);
			toFile = LetterToNumber(moveString[2]);
		}
		else if (piece.front().m_Type == Position::PieceType::Pawn) //queening
		{
			if (!isalpha(moveString[0]) || !isdigit(moveString[1]) || !isalpha(moveString[2]))
				return move;

			fromFile = LetterToNumber(moveString[0]);
			toFile = fromFile;
			fromRow = position.IsWhiteToPlay() ? 6 : 1;
			toRow = position.IsWhiteToPlay() ? 7 : 0;
			toType = LetterToPieceType(moveString[2]);
		}
		else
		{
			if (!isalpha(moveString[0]) || !isalpha(moveString[1]) || !isdigit(moveString[2]))
				return move;

			toFile = LetterToNumber(moveString[1]);
			const std::string rowString = moveString.substr(2, 1);
			toRow = std::stoi(rowString) - 1;
		}
		break;
	}
	case 4: //capture move or disambiguition or queening
	{
		if (piece.front().m_Type == Position::PieceType::Pawn) //pawn takes (not abbreviated) or queens a8=Q
		{
			if (moveString[1] == 'x')
			{
				if (!isalpha(moveString[0]) || !isalpha(moveString[2]) || !isdigit(moveString[3]))
					return move;

				fromFile = LetterToNumber(moveString[0]);
				toFile = LetterToNumber(moveString[2]);
				const std::string rowString = moveString.substr(3, 1);
				toRow = std::stoi(rowString) - 1;
			}
			else if (moveString[2] == '=')
			{
				if (!isalpha(moveString[0]) || !isdigit(moveString[1]) || !isalpha(moveString[3]))
					return move;

				toFile = LetterToNumber(moveString[0]);
				const std::string rowString = moveString.substr(1, 1);
				toRow = std::stoi(rowString) - 1;
				toType = LetterToPieceType(moveString[3]);
			}
		}
		else if (moveString[1] == 'x') //piece takes
		{
			if (!isalpha(moveString[0]) || !isalpha(moveString[2]) || !isdigit(moveString[3]))
				return move;

			toFile = LetterToNumber(moveString[2]);
			const std::string rowString = moveString.substr(3, 1);
			toRow = std::stoi(rowString) - 1;
		}
		else if (isalpha(moveString[1])) //file disambiguition
		{
			if (!isalpha(moveString[0]) || !isalpha(moveString[2]) || !isdigit(moveString[3]))
				return move;

			fromFile = LetterToNumber(moveString[1]);
			toFile = LetterToNumber(moveString[2]);
			const std::string rowString = moveString.substr(3, 1);
			toRow = std::stoi(rowString) - 1;
		}
		else if (isdigit(moveString[1])) //row disambiguition
		{
			if (!isalpha(moveString[0]) || !isalpha(moveString[2]) || !isdigit(moveString[3]))
				return move;

			std::string rowString = moveString.substr(1, 1);
			fromRow = std::stoi(rowString) - 1;
			toFile = LetterToNumber(moveString[2]);
			rowString = moveString.substr(3, 1);
			toRow = std::stoi(rowString) - 1;
		}
		else //invalid move
			return move;
		break;
	}
	case 5: //capture move with disambiguition or double disambiguition (not capture) Nfxg5 or N1xg5 or Nf1g5 OR queening abbreviated bxa=Q
	{
		if (moveString[2] == 'x') //capture, single disambiguition
		{
			if (isdigit(moveString[1]))
			{
				const std::string rowString = moveString.substr(1, 1);
				fromRow = std::stoi(rowString) - 1;
			}
			else if (isalpha(moveString[1]))
				fromFile = LetterToNumber(moveString[1]);
		}
		else
		{
			if (!isalpha(moveString[1]) || !isdigit(moveString[2]))
				return move;

			fromFile = LetterToNumber(moveString[1]);
			const std::string rowString = moveString.substr(2, 1);
			fromRow = std::stoi(rowString) - 1;
		}

		toFile = LetterToNumber(moveString[3]);
		const std::string rowString = moveString.substr(4, 1);
		toRow = std::stoi(rowString) - 1;
		break;
	}
	case 6: //double disambiguition with capture or queening by capture (Nf1xg5 or bxa8=Q)
	{
		if (piece.front().m_Type != Position::PieceType::Pawn)
		{
			if (!isalpha(moveString[0]) || !isalpha(moveString[1]) || !isdigit(moveString[2]) ||
				moveString[3] != 'x' || !isalpha(moveString[4]) || !isdigit(moveString[5]))
				return move;

			fromFile = LetterToNumber(moveString[1]);
			std::string rowString = moveString.substr(2, 1);
			fromRow = std::stoi(rowString) - 1;
			toFile = LetterToNumber(moveString[4]);
			rowString = moveString.substr(5, 1);
			toRow = std::stoi(rowString) - 1;
		}
		else
		{
			if (!isalpha(moveString[0]) || moveString[1] != 'x' || !isalpha(moveString[2]) || !isdigit(moveString[3]) ||
				moveString[4] != '=' || !isalpha(moveString[5]))
				return move;

			fromFile = LetterToNumber(moveString[0]);
			toFile = LetterToNumber(moveString[2]);
			std::string rowString = moveString.substr(3, 1);
			toRow = std::stoi(rowString) - 1;
			toType = LetterToPieceType(moveString[5]);
		}

		break;
	}
	default:
		return move;
	}
	
	//check row/file numbers
	if (fromRow.has_value() && (fromRow < 0 || fromRow > 7))
		return move;
	if (fromFile.has_value() && (fromFile < 0 || fromFile > 7))
		return move;
	if (toRow.has_value() && (toRow < 0 || toRow > 7))
		return move;
	if (toFile.has_value() && (toFile < 0 || toFile > 7))
		return move;
	//get from square
	if (piece.size() == 1)
	{
		move = Position::Move();
		move->m_From = piece.front();
		move->m_To = piece.front();
		if (!toFile.has_value())
			return Position::Move(); //should not happen
		if (!toRow.has_value())
		{
			//pawn move
			toRow = move->m_From.m_Position[1] + 1;
		}

		move->m_To.m_Position = { *toFile, *toRow }; //no check for valid square, allow cheating!
	}
	else
	{
		//search correct piece
		const Position::Piece* pieceToMove = nullptr;
		int count = 0;
		for (const Position::Piece& p : piece)
		{
			bool canReachSquare = false;
			const std::vector<Position::Move> legalMoves = MoveSearcher::GetLegalMoves(position, p, position.IsWhiteToPlay());
			for (const Position::Move& legalMove : legalMoves)
			{
				const std::array<int, 2>& square = legalMove.m_To.m_Position;
				if (square == std::array<int, 2>{*toFile, * toRow})
				{
					canReachSquare = true;
					break;
				}
			}

			if (!canReachSquare)
				continue;

			if (fromRow.has_value() && fromRow == p.m_Position[1])
			{
				pieceToMove = &p;
				count++;
			}
			else if (fromFile.has_value() && fromFile == p.m_Position[0])
			{
				pieceToMove = &p;
				count++;
			}
			else if (!fromRow.has_value() && !fromFile.has_value())
			{
				pieceToMove = &p;
				count++;
			}
		}

		if (count != 1)
			return Position::Move(); //invalid move
		assert(pieceToMove);

		move = Position::Move();
		move->m_From = *pieceToMove;
		move->m_To = *pieceToMove;
		if (!toFile.has_value() || !toRow.has_value())
			return Position::Move();
		move->m_To.m_Position = { *toFile, *toRow };
	}

	if (move.has_value())
		move->m_To.m_Type = toType;

	return move;
}