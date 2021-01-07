#include "Position.h"

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

	//m_WhitePieces.emplace_back(Piece(PieceType::King, 4, 0));
	//m_WhitePieces.emplace_back(Piece(PieceType::Rook, 0, 0));
	//m_WhitePieces.emplace_back(Piece(PieceType::Rook, 5, 0));

	//m_BlackPieces.emplace_back(Piece(PieceType::King, 4, 7));
	//m_BlackPieces.emplace_back(Piece(PieceType::Rook, 7, 7));
	//m_BlackPieces.emplace_back(Piece(PieceType::Rook, 4, 6));
	////m_BlackPieces.emplace_back(Piece(PieceType::Pawn, 5, 6));

	//m_CanBlackCastleKingSide = true;
	//m_CanWhiteCastleKingSide = false;
	//m_CanBlackCastleQueenSide = false;
	//m_CanWhiteCastleQueenSide = true;
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

std::vector<Position::Piece> Position::GetPiecesToPlay(PieceType type) const
{
	std::vector<Position::Piece> pieces;
	const std::vector<Piece>& friendlyPieces = m_IsWhiteToPlay ? GetWhitePieces() : GetBlackPieces();
	for (const Piece& piece : friendlyPieces)
	{
		if (piece.m_Type == type)
		{
			pieces.push_back(piece);
		}
	}

	return pieces;
}
