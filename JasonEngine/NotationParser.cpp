#include "pch.h"
#include "NotationParser.h"
#include "MoveSearcher.h"
#include <assert.h>

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

void NotationParser::TranslateFEN(const std::string& fen, Position& position)
{
	position.InitEmptyBoard();

	//e.g.: rnbqkbnr / pppppppp / 8 / 8 / 8 / 8 / PPPPPPPP / RNBQKBNR w KQkq - 0 1
	int squareIdx = 0; //start from top left
	int dashCount = 0;
	std::optional<int> enPassantRow;
	std::optional<int> enPassantFile;
	for (char c : fen)
	{
		if (squareIdx < 64)
		{
			if (isalpha(c))
			{
				const std::array<int, 2> square = { squareIdx % 8, 7 - squareIdx / 8 };
				if (c == 'r')
				{
					position.GetBlackPieces().emplace_back(Piece(PieceType::Rook, square));
				}
				else if (c == 'n')
				{
					position.GetBlackPieces().emplace_back(Piece(PieceType::Knight, square));
				}
				else if (c == 'b')
				{
					position.GetBlackPieces().emplace_back(Piece(PieceType::Bishop, square));
				}
				else if (c == 'q')
				{
					position.GetBlackPieces().emplace_back(Piece(PieceType::Queen, square));
				}
				else if (c == 'k')
				{
					position.GetBlackPieces().emplace_back(Piece(PieceType::King, square));
				}
				else if (c == 'p')
				{
					position.GetBlackPieces().emplace_back(Piece(PieceType::Pawn, square));
				}
				else if (c == 'R')
				{
					position.GetWhitePieces().emplace_back(Piece(PieceType::Rook, square));
				}
				else if (c == 'N')
				{
					position.GetWhitePieces().emplace_back(Piece(PieceType::Knight, square));
				}
				else if (c == 'B')
				{
					position.GetWhitePieces().emplace_back(Piece(PieceType::Bishop, square));
				}
				else if (c == 'Q')
				{
					position.GetWhitePieces().emplace_back(Piece(PieceType::Queen, square));
				}
				else if (c == 'K')
				{
					position.GetWhitePieces().emplace_back(Piece(PieceType::King, square));
				}
				else if (c == 'P')
				{
					position.GetWhitePieces().emplace_back(Piece(PieceType::Pawn, square));
				}

				squareIdx++;
			}
			else
			{
				if (isdigit(c))
				{
					squareIdx += c - '0';
				}
				else if (c == '//')
				{
					//do nothing
				}
			}
		}
		else
		{
			if (c == 'w')
			{
				position.SetWhiteToPlay(true);
			}
			else if (c == 'b')
			{
				position.SetWhiteToPlay(false);
			}
			else if (c == 'K')
			{
				position.SetCanWhiteCastleKingSide(true);
			}
			else if (c == 'Q')
			{
				position.SetCanWhiteCastleQueenSide(true);
			}
			else if (c == 'k')
			{
				position.SetCanBlackCastleKingSide(true);
			}
			else if (c == 'q')
			{
				position.SetCanBlackCastleQueenSide(true);
			}
			else if (c == '-') //no castling or en passant
			{
				dashCount++;
			}
			else if (isalpha(c)) //en passant file
			{
				enPassantFile = LetterToNumber(c);
			}
			else if (isdigit(c) && enPassantFile.has_value()) //en passant row 
			{
				enPassantRow = (c - '0') + 1; //0 based
			}
			else if (isdigit(c))
			{
				//move clocks
			}
		}
	}

	if (enPassantFile.has_value() && enPassantRow.has_value())
	{
		position.SetEnPassantSquare({ *enPassantFile , *enPassantRow });
	}
}

std::string NotationParser::TranslateToAlgebraic(PieceType type)
{
	std::string pieceString;
	switch (type)
	{
	case PieceType::Pawn:
		pieceString = "P";
		break;
	case PieceType::Rook:
		pieceString = "R";
		break;
	case PieceType::Bishop:
		pieceString = "B";
		break;
	case PieceType::Knight:
		pieceString = "N";
		break;
	case PieceType::Queen:
		pieceString = "Q";
		break;
	case PieceType::King:
		pieceString = "K";
		break;
	default:
		assert(false);
	}

	return pieceString;
}

static PieceType LetterToPieceType(char A)
{
	PieceType type = PieceType::Pawn;
	if (A == 'Q')
		type = PieceType::Queen;
	else if (A == 'R')
		type = PieceType::Rook;
	else if (A == 'N')
		type = PieceType::Knight;
	else if (A == 'B')
		type = PieceType::Bishop;
	else if (A == 'K')
		type = PieceType::King;
	else
		type = PieceType::Pawn;

	return type;
}

std::string NotationParser::TranslateToAlgebraic(const std::array<int, 2>& square)
{
	std::string squareString = NthLetter(square[0]) + std::to_string(square[1] + 1);
	return squareString;
}

std::string NotationParser::TranslateToAlgebraic(const Move& move)
{
	std::string moveString;

	//Check for castling moves
	if (move.m_From.m_Type == PieceType::King &&
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
		moveString = (move.m_From.m_Type == PieceType::Pawn) ? "" : TranslateToAlgebraic(move.m_From.m_Type);
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

std::optional<Move> NotationParser::TranslateFromAlgebraic(const Position& position, const std::string& moveString)
{
	std::optional<Move> move;
	if (moveString.size() < 2)
		return move; //invalid length

	std::vector<const Piece*> pieces;
	//castling is easy to parse
	const bool isKingSideCastle = moveString == "O-O";
	const bool isQueenSideCastle = moveString == "O-O-O";
	if (isKingSideCastle || isQueenSideCastle)
	{
		pieces = position.GetPiecesToPlay(PieceType::King);
		if (pieces.empty())
		{
			assert(false); //no king?!
			return move;
		}

		move = Move();
		move->m_From = *pieces.front();
		move->m_To = move->m_From;
		move->m_To.m_Position[0] += isKingSideCastle ? 2 : -2;
		return move;
	}

	//first expected char is symbol
	pieces = position.GetPiecesToPlay(LetterToPieceType(moveString[0]));
	if (pieces.empty())
		return move;

	std::optional<int> fromRow;
	std::optional<int> fromFile;
	std::optional<int> toRow;
	std::optional<int> toFile;
	PieceType toType = pieces.front()->m_Type;

	switch (moveString.size())
	{
	case 2: //pawn move
	{
		if (pieces.front()->m_Type != PieceType::Pawn)
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
			if (pieces.front()->m_Type == PieceType::Pawn)
				return move;

			if (!isalpha(moveString[0]) || !isalpha(moveString[2]))
				return move;

			fromFile = LetterToNumber(moveString[0]);
			toFile = LetterToNumber(moveString[2]);
		}
		else if (pieces.front()->m_Type == PieceType::Pawn) //queening
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
		if (pieces.front()->m_Type == PieceType::Pawn) //pawn takes (not abbreviated) or queens a8=Q
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
		if (pieces.front()->m_Type != PieceType::Pawn)
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
	if (pieces.size() == 1)
	{
		move = Move();
		move->m_From = *pieces.front();
		move->m_To = *pieces.front();
		if (!toFile.has_value())
			return Move(); //should not happen
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
		const Piece* pieceToMove = nullptr;
		int count = 0;
		for (const Piece* p : pieces)
		{
			bool canReachSquare = false;
			const std::vector<Move> legalMoves = MoveSearcher::GetLegalMoves(position, *p, position.IsWhiteToPlay());
			for (const Move& legalMove : legalMoves)
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

			if (fromRow.has_value() && fromRow == p->m_Position[1])
			{
				pieceToMove = p;
				count++;
			}
			else if (fromFile.has_value() && fromFile == p->m_Position[0])
			{
				pieceToMove = p;
				count++;
			}
			else if (!fromRow.has_value() && !fromFile.has_value())
			{
				pieceToMove = p;
				count++;
			}
		}

		if (count != 1)
			return Move(); //invalid move
		assert(pieceToMove);

		move = Move();
		move->m_From = *pieceToMove;
		move->m_To = *pieceToMove;
		if (!toFile.has_value() || !toRow.has_value())
			return Move();
		move->m_To.m_Position = { *toFile, *toRow };
	}

	if (move.has_value())
		move->m_To.m_Type = toType;

	return move;
}