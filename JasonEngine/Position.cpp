#include "pch.h"
#include "NotationParser.h"
#include "ZobristHash.h"
#include <assert.h>

Position::Position()
{
	m_WhitePieces.emplace_back(Piece(PieceType::Rook, 0, 0));
	m_WhitePieces.emplace_back(Piece(PieceType::Knight, 1, 0));
	m_WhitePieces.emplace_back(Piece(PieceType::Bishop, 2, 0));
	m_WhitePieces.emplace_back(Piece(PieceType::Queen, 3, 0));
	m_WhitePieces.emplace_back(Piece(PieceType::King, 4, 0));
	m_WhitePieces.emplace_back(Piece(PieceType::Bishop, 5, 0));
	m_WhitePieces.emplace_back(Piece(PieceType::Knight, 6, 0));
	m_WhitePieces.emplace_back(Piece(PieceType::Rook, 7, 0));
	m_WhitePieces.emplace_back(Piece(PieceType::Pawn, 0, 1));
	m_WhitePieces.emplace_back(Piece(PieceType::Pawn, 1, 1));
	m_WhitePieces.emplace_back(Piece(PieceType::Pawn, 2, 1));
	m_WhitePieces.emplace_back(Piece(PieceType::Pawn, 3, 1));
	m_WhitePieces.emplace_back(Piece(PieceType::Pawn, 4, 1));
	m_WhitePieces.emplace_back(Piece(PieceType::Pawn, 5, 1));
	m_WhitePieces.emplace_back(Piece(PieceType::Pawn, 6, 1));
	m_WhitePieces.emplace_back(Piece(PieceType::Pawn, 7, 1));

	m_BlackPieces.emplace_back(Piece(PieceType::Rook, 0, 7));
	m_BlackPieces.emplace_back(Piece(PieceType::Knight, 1, 7));
	m_BlackPieces.emplace_back(Piece(PieceType::Bishop, 2, 7));
	m_BlackPieces.emplace_back(Piece(PieceType::Queen, 3, 7));
	m_BlackPieces.emplace_back(Piece(PieceType::King, 4, 7));
	m_BlackPieces.emplace_back(Piece(PieceType::Bishop, 5, 7));
	m_BlackPieces.emplace_back(Piece(PieceType::Knight, 6, 7));
	m_BlackPieces.emplace_back(Piece(PieceType::Rook, 7, 7));
	m_BlackPieces.emplace_back(Piece(PieceType::Pawn, 0, 6));
	m_BlackPieces.emplace_back(Piece(PieceType::Pawn, 1, 6));
	m_BlackPieces.emplace_back(Piece(PieceType::Pawn, 2, 6));
	m_BlackPieces.emplace_back(Piece(PieceType::Pawn, 3, 6));
	m_BlackPieces.emplace_back(Piece(PieceType::Pawn, 4, 6));
	m_BlackPieces.emplace_back(Piece(PieceType::Pawn, 5, 6));
	m_BlackPieces.emplace_back(Piece(PieceType::Pawn, 6, 6));
	m_BlackPieces.emplace_back(Piece(PieceType::Pawn, 7, 6));

	m_ZobristHash = ZobristHash::Init();

	//m_WhitePieces.emplace_back(Piece(PieceType::King, 4, 0));
	//m_WhitePieces.emplace_back(Piece(PieceType::Rook, 0, 0));
	//m_WhitePieces.emplace_back(Piece(PieceType::Rook, 1, 6));

	//m_BlackPieces.emplace_back(Piece(PieceType::King, 4, 7));

	//m_CanBlackCastleKingSide = false;
	//m_CanWhiteCastleKingSide = false;
	//m_CanBlackCastleQueenSide = false;
	//m_CanWhiteCastleQueenSide = false;
}

Position::Position(const std::string& fen)
{
	NotationParser::TranslateFEN(fen, *this);
}

void Position::InitEmptyBoard()
{
	GetWhitePieces().clear();
	GetBlackPieces().clear();
	SetCanWhiteCastleKingSide(false);
	SetCanWhiteCastleQueenSide(false);
	SetCanBlackCastleKingSide(false);
	SetCanBlackCastleQueenSide(false);
}

void Position::SetEnPassantSquare(std::array<int, 2> square)
{
	ResetEnPassantSquare();
	m_EnPassantSquare = square;
	m_ZobristHash ^= ZobristHash::GetEnPassantKey(square[0]);
}

void Position::ResetEnPassantSquare()
{
	if (m_EnPassantSquare.has_value())
	{
		m_ZobristHash ^= ZobristHash::GetEnPassantKey((*m_EnPassantSquare)[0]);
		m_EnPassantSquare.reset();
	}
}

void Position::SetCanWhiteCastleKingSide(bool value)
{
	m_CanWhiteCastleKingSide = value;

	if (!value)
		m_ZobristHash ^= ZobristHash::GetWhiteKingSideCastleKey();
}

void Position::SetCanBlackCastleKingSide(bool value)
{
	m_CanBlackCastleKingSide = value;

	if (!value)
		m_ZobristHash ^= ZobristHash::GetBlackKingSideCastleKey();
}

void Position::SetCanWhiteCastleQueenSide(bool value)
{
	m_CanWhiteCastleQueenSide = value;

	if (!value)
		m_ZobristHash ^= ZobristHash::GetWhiteQueenSideCastleKey();
}

void Position::SetCanBlackCastleQueenSide(bool value)
{
	m_CanBlackCastleQueenSide = value;

	if (!value)
		m_ZobristHash ^= ZobristHash::GetBlackQueenSideCastleKey();
}

bool Position::IsInsufficientMaterial() const
{
	bool isStaleMate = true;

	for (const std::vector<Piece>& pieceSets : { GetWhitePieces(), GetBlackPieces() })
	{
		int minorPieceCount = 0;
		for (const Piece& piece : pieceSets)
		{
			switch (piece.m_Type)
			{
			case PieceType::Rook:
			case PieceType::Queen:
				isStaleMate = false;
				break;
			case PieceType::Knight:
			case PieceType::Bishop:
				minorPieceCount++;
				break;
			case PieceType::Pawn:
				isStaleMate = false;
			}
		}

		if (minorPieceCount >= 2)
			isStaleMate = false;

		if (!isStaleMate)
			break;
	}

	return isStaleMate;
}

bool Position::IsSquareOccupiedByOpponent(const std::array<int, 2>& square) const
{
	bool isOccupied = false;
	const std::vector<Piece>& enemyPieces = m_IsWhiteToPlay ? GetBlackPieces() : GetWhitePieces();
	for (const Piece& piece : enemyPieces)
	{
		if (piece.m_Position == square)
		{
			isOccupied = true;
			break;
		}
	}

	return isOccupied;
}

std::vector<const Piece*> Position::GetPieces(PieceType type, bool isWhite) const
{
	std::vector<const Piece*> pieces;
	const std::vector<Piece>& friendlyPieces = isWhite ? GetWhitePieces() : GetBlackPieces();
	for (const Piece& piece : friendlyPieces)
	{
		if (piece.m_Type == type)
		{
			pieces.push_back(&piece);
			if (piece.m_Type == PieceType::King)
				break; //only 2 king
		}
	}

	return pieces;
}

const Piece* Position::GetPiece(PieceType type, bool isWhite) const
{
	std::vector<const Piece*> pieces = GetPieces(type, isWhite);
	if (!pieces.empty())
		return pieces.front();
	else
		return nullptr;
}

std::vector<const Piece*> Position::GetPiecesToPlay(PieceType type) const
{
	return GetPieces(type, IsWhiteToPlay());
}

void Position::UpdatePosition(const Move& move)
{
	//update moving piece
	std::vector<Piece>& friendlyPieces = IsWhiteToPlay() ? GetWhitePieces() : GetBlackPieces();
	for (Piece& friendlyPiece : friendlyPieces)
	{
		if (friendlyPiece.m_Position == move.m_From.m_Position)
		{
			friendlyPiece = move.m_To; //update type too for queening
			break;
		}
	}

	//update zobrist hash the same way
	m_ZobristHash ^= ZobristHash::GetKey(move.m_From, IsWhiteToPlay());
	m_ZobristHash ^= ZobristHash::GetKey(move.m_To, IsWhiteToPlay());

	//we have to correct new position by removing a piece if taken!
	std::vector<Piece>& enemyPieces = IsWhiteToPlay() ? GetBlackPieces() : GetWhitePieces();
	std::optional<size_t> indexPieceToRemove;
	for (size_t i = 0; i < enemyPieces.size(); i++)
	{
		if (enemyPieces[i].m_Position == move.m_To.m_Position)
		{
			indexPieceToRemove = i;
			m_ZobristHash ^= ZobristHash::GetKey(enemyPieces[i], !IsWhiteToPlay());
			break;
		}
	}


	if (indexPieceToRemove.has_value())
		enemyPieces.erase(enemyPieces.begin() + *indexPieceToRemove);

	//set or reset en passant square
	if ((move.m_From.m_Type == PieceType::Pawn) && (abs(move.m_From.m_Position[1] - move.m_To.m_Position[1]) == 2))
	{
		SetEnPassantSquare({
		move.m_From.m_Position[0],
		move.m_From.m_Position[1] + (IsWhiteToPlay() ? 1 : -1) });
	}
	else
		ResetEnPassantSquare();

	Move move2 = move; //set capture flag
	move2.m_IsCapture = indexPieceToRemove.has_value();

	//Move rook if castle
	if ((move2.m_From.m_Type == PieceType::King) && abs(move2.m_From.m_Position[0] - move2.m_To.m_Position[0]) > 1)
	{
		if ((move2.m_To.m_Position[0] - move2.m_From.m_Position[0]) > 1) //kingside
		{
			//search the corresponding rook
			std::array<int, 2> rookSquare = move2.m_To.m_Position;
			rookSquare[0] = 7;
			for (Piece& friendlyPiece : friendlyPieces)
			{
				if (friendlyPiece.m_Position == rookSquare)
				{
					assert(friendlyPiece.m_Type == PieceType::Rook);
					friendlyPiece.m_Position[0] -= 2;

					Piece rook(PieceType::Rook, rookSquare[0], rookSquare[1]);
					m_ZobristHash ^= ZobristHash::GetKey(rook, IsWhiteToPlay());
					rook.m_Position[0] -= 2;
					m_ZobristHash ^= ZobristHash::GetKey(rook, IsWhiteToPlay());
					break;
				}
			}
		}
		else //queenside
		{
			std::array<int, 2> rookSquare = move2.m_To.m_Position;
			rookSquare[0] = 0;
			for (Piece& friendlyPiece : friendlyPieces)
			{
				if (friendlyPiece.m_Position == rookSquare)
				{
					assert(friendlyPiece.m_Type == PieceType::Rook);
					friendlyPiece.m_Position[0] += 3;
					
					Piece rook(PieceType::Rook, rookSquare[0], rookSquare[1]);
					m_ZobristHash ^= ZobristHash::GetKey(rook, IsWhiteToPlay());
					rook.m_Position[0] += 3;
					m_ZobristHash ^= ZobristHash::GetKey(rook, IsWhiteToPlay());
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
		SetCanWhiteCastleQueenSide(false);
	if ((move2.m_From.m_Position == br) || (move2.m_To.m_Position == br))
		SetCanWhiteCastleKingSide(false);
	if ((move2.m_From.m_Position == tl) || (move2.m_To.m_Position == tl))
		SetCanBlackCastleQueenSide(false);
	if ((move2.m_From.m_Position == tr) || (move2.m_To.m_Position == tr))
		SetCanBlackCastleKingSide(false);
	//King moves
	if (IsWhiteToPlay() && (move2.m_From.m_Type == PieceType::King))
	{
		SetCanWhiteCastleKingSide(false);
		SetCanWhiteCastleQueenSide(false);
	}
	if (!IsWhiteToPlay() && (move2.m_From.m_Type == PieceType::King))
	{
		SetCanBlackCastleKingSide(false);
		SetCanBlackCastleQueenSide(false);
	}

	m_IsWhiteToPlay = !m_IsWhiteToPlay;
	m_ZobristHash ^= ZobristHash::GetWhiteToMoveKey();

	GetMoves().push_back(move2);
}
