#include "pch.h"
#include "NotationParser.h"
#include "ZobristHash.h"
#include <assert.h>

Position::Position()
{
	m_WhitePawns = _2;
	m_WhiteKnights = _b1 | _g1;
	m_WhiteBishops = _c1 | _f1;
	m_WhiteRooks = _a1 | _h1;
	m_WhiteQueens = _d1;
	m_WhiteKing= _e1;
	m_WhitePieces = m_WhitePawns | m_WhiteKnights | m_WhiteBishops | m_WhiteRooks | m_WhiteQueens | m_WhiteKing;

	m_BlackPawns = _7;
	m_BlackKnights = _b8 | _g8;
	m_BlackBishops = _c8 | _f8;
	m_BlackRooks = _a8 | _h8;
	m_BlackQueens = _d8;
	m_BlackKing = _e8;
	m_BlackPieces = m_BlackPawns | m_BlackKnights | m_BlackBishops | m_BlackRooks | m_BlackQueens | m_BlackKing;

	m_WhitePiecesList.emplace_back(Piece(PieceType::Rook, a1));
	m_WhitePiecesList.emplace_back(Piece(PieceType::Knight, b1));
	m_WhitePiecesList.emplace_back(Piece(PieceType::Bishop, c1));
	m_WhitePiecesList.emplace_back(Piece(PieceType::Queen, d1));
	m_WhitePiecesList.emplace_back(Piece(PieceType::King, e1));
	m_WhitePiecesList.emplace_back(Piece(PieceType::Bishop, f1));
	m_WhitePiecesList.emplace_back(Piece(PieceType::Knight,g1));
	m_WhitePiecesList.emplace_back(Piece(PieceType::Rook, h1));
	m_WhitePiecesList.emplace_back(Piece(PieceType::Pawn, a2));
	m_WhitePiecesList.emplace_back(Piece(PieceType::Pawn, b2));
	m_WhitePiecesList.emplace_back(Piece(PieceType::Pawn, c2));
	m_WhitePiecesList.emplace_back(Piece(PieceType::Pawn, d2));
	m_WhitePiecesList.emplace_back(Piece(PieceType::Pawn, e2));
	m_WhitePiecesList.emplace_back(Piece(PieceType::Pawn, f2));
	m_WhitePiecesList.emplace_back(Piece(PieceType::Pawn, g2));
	m_WhitePiecesList.emplace_back(Piece(PieceType::Pawn, h2));

	m_BlackPiecesList.emplace_back(Piece(PieceType::Rook, a8));
	m_BlackPiecesList.emplace_back(Piece(PieceType::Knight, b8));
	m_BlackPiecesList.emplace_back(Piece(PieceType::Bishop, c8));
	m_BlackPiecesList.emplace_back(Piece(PieceType::Queen, d8));
	m_BlackPiecesList.emplace_back(Piece(PieceType::King, e8));
	m_BlackPiecesList.emplace_back(Piece(PieceType::Bishop, f8));
	m_BlackPiecesList.emplace_back(Piece(PieceType::Knight, g8));
	m_BlackPiecesList.emplace_back(Piece(PieceType::Rook, h8));
	m_BlackPiecesList.emplace_back(Piece(PieceType::Pawn, a7));
	m_BlackPiecesList.emplace_back(Piece(PieceType::Pawn, b7));
	m_BlackPiecesList.emplace_back(Piece(PieceType::Pawn, c7));
	m_BlackPiecesList.emplace_back(Piece(PieceType::Pawn, d7));
	m_BlackPiecesList.emplace_back(Piece(PieceType::Pawn, e7));
	m_BlackPiecesList.emplace_back(Piece(PieceType::Pawn, f7));
	m_BlackPiecesList.emplace_back(Piece(PieceType::Pawn, g7));
	m_BlackPiecesList.emplace_back(Piece(PieceType::Pawn, h7));

	m_ZobristHash = ZobristHash::Init();
}

Position::Position(const std::string& fen)
{
	NotationParser::TranslateFEN(fen, *this);
}

void Position::InitEmptyBoard()
{
	GetWhitePiecesList().clear();
	GetBlackPiecesList().clear();
	SetCanWhiteCastleKingSide(false);
	SetCanWhiteCastleQueenSide(false);
	SetCanBlackCastleKingSide(false);
	SetCanBlackCastleQueenSide(false);
}

void Position::SetEnPassantSquare(Square square)
{
	ResetEnPassantSquare();
	m_EnPassantSquare = square;
	m_ZobristHash ^= ZobristHash::GetEnPassantKey(square);
}

void Position::ResetEnPassantSquare()
{
	if (m_EnPassantSquare.has_value())
	{
		m_ZobristHash ^= ZobristHash::GetEnPassantKey(*m_EnPassantSquare);
		m_EnPassantSquare.reset();
	}
}

void Position::SetCanWhiteCastleKingSide(bool value)
{
	if (m_CanWhiteCastleKingSide != value)
		m_ZobristHash ^= ZobristHash::GetWhiteKingSideCastleKey();

	m_CanWhiteCastleKingSide = value;
}

void Position::SetCanBlackCastleKingSide(bool value)
{
	if (m_CanBlackCastleKingSide != value)
		m_ZobristHash ^= ZobristHash::GetBlackKingSideCastleKey();

	m_CanBlackCastleKingSide = value;
}

void Position::SetCanWhiteCastleQueenSide(bool value)
{
	if (m_CanWhiteCastleQueenSide != value)
		m_ZobristHash ^= ZobristHash::GetWhiteQueenSideCastleKey();

	m_CanWhiteCastleQueenSide = value;
}

void Position::SetCanBlackCastleQueenSide(bool value)
{
	if (m_CanBlackCastleQueenSide != value)
		m_ZobristHash ^= ZobristHash::GetBlackQueenSideCastleKey();

	m_CanBlackCastleQueenSide = value;
}

bool Position::IsInsufficientMaterial() const
{
	bool isStaleMate = true;

	for (const std::vector<Piece>& pieceSets : { GetWhitePiecesList(), GetBlackPiecesList() })
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

std::vector<const Piece*> Position::GetPieces(PieceType type, bool isWhite) const
{
	std::vector<const Piece*> pieces;
	const std::vector<Piece>& friendlyPieces = isWhite ? GetWhitePiecesList() : GetBlackPiecesList();
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

uint64_t Position::ComputeZobristHash() const
{
	uint64_t hash = 0;
	for (const Piece& piece : GetWhitePiecesList())
	{
		hash ^= ZobristHash::GetKey(piece, true);
	}
	for (const Piece& piece : GetBlackPiecesList())
	{
		hash ^= ZobristHash::GetKey(piece, false);
	}

	if (m_EnPassantSquare.has_value())
		hash ^= ZobristHash::GetEnPassantKey(*m_EnPassantSquare);
	if (m_CanWhiteCastleQueenSide)
		hash ^= ZobristHash::GetWhiteQueenSideCastleKey();
	if (m_CanWhiteCastleKingSide)
		hash ^= ZobristHash::GetWhiteKingSideCastleKey();
	if (m_CanBlackCastleQueenSide)
		hash ^= ZobristHash::GetBlackQueenSideCastleKey();
	if (m_CanBlackCastleKingSide)
		hash ^= ZobristHash::GetBlackKingSideCastleKey();
	if (!m_IsWhiteToPlay)
		hash ^= ZobristHash::GetBlackToMoveKey();

	return hash;
}

void Position::Update(Move& move)
{
	//update moving piece
	std::vector<Piece>& friendlyPieces = IsWhiteToPlay() ? GetWhitePiecesList() : GetBlackPiecesList();
	for (Piece& friendlyPiece : friendlyPieces)
	{
		if (friendlyPiece.m_Square == move.m_From.m_Square)
		{
			friendlyPiece = move.m_To; //update type too for queening
			break;
		}
	}

	UpdatePiece(move);
	//update zobrist hash the same way
	m_ZobristHash ^= ZobristHash::GetKey(move.m_From, IsWhiteToPlay());
	m_ZobristHash ^= ZobristHash::GetKey(move.m_To, IsWhiteToPlay());

	//we have to correct new position by removing a piece if taken!
	const bool isCaptureEnPassant = //necessarily en passant capture if a new pawn takes the EP square
		(m_EnPassantSquare.has_value() &&
		(move.m_From.m_Type == PieceType::Pawn) &&
		(move.m_To.m_Square == *m_EnPassantSquare));
	int captureSquare = move.m_To.m_Square;
	if (isCaptureEnPassant)
		captureSquare += (IsWhiteToPlay() ? -1 : 1) * 8;

	std::vector<Piece>& enemyPieces = IsWhiteToPlay() ? GetBlackPiecesList() : GetWhitePiecesList();
	std::optional<size_t> indexPieceToRemove;
	for (size_t i = 0; i < enemyPieces.size(); i++)
	{
		if (enemyPieces[i].m_Square == captureSquare)
		{
			indexPieceToRemove = i;
			move.m_Capture = enemyPieces[i];
			m_ZobristHash ^= ZobristHash::GetKey(enemyPieces[i], !IsWhiteToPlay());
			break;
		}
	}

	UpdateCapturedPiece(captureSquare);

	if (indexPieceToRemove.has_value())
	{
		assert((enemyPieces[*indexPieceToRemove].m_Type == PieceType::Pawn) || !isCaptureEnPassant);
		enemyPieces.erase(enemyPieces.begin() + *indexPieceToRemove);
	}

	//set or reset en passant square
	if (move.IsTwoStepsPawn())
		SetEnPassantSquare(static_cast<Square>(move.m_From.m_Square + (IsWhiteToPlay() ? 8 : -8)));
	else
		ResetEnPassantSquare();

	//Move rook if castle
	if (move.IsCastling())
	{
		Move rookMove;
		rookMove.m_From.m_Type = PieceType::Rook;
		rookMove.m_To.m_Type = PieceType::Rook;

		if (move.m_To.m_Square > move.m_From.m_Square) //kingside
		{
			if (m_IsWhiteToPlay)
			{
				rookMove.m_From.m_Square = h1;
				rookMove.m_To.m_Square = f1;
			}
			else
			{
				rookMove.m_From.m_Square = h8;
				rookMove.m_To.m_Square = f8;
			}

			UpdatePiece(rookMove);
		}
		else //queenside
		{
			if (m_IsWhiteToPlay)
			{
				rookMove.m_From.m_Square = a1;
				rookMove.m_To.m_Square = d1;
			}
			else
			{
				rookMove.m_From.m_Square = a8;
				rookMove.m_To.m_Square = d8;
			}

			UpdatePiece(rookMove);
		}

		//search the corresponding rook
		for (Piece& friendlyPiece : friendlyPieces)
		{
			if (friendlyPiece == rookMove.m_From)
			{
				assert(friendlyPiece.m_Type == PieceType::Rook);
				m_ZobristHash ^= ZobristHash::GetKey(friendlyPiece, IsWhiteToPlay());
				friendlyPiece.m_Square = rookMove.m_To.m_Square;
				m_ZobristHash ^= ZobristHash::GetKey(friendlyPiece, IsWhiteToPlay());
				break;
			}
		}

		if (IsWhiteToPlay())
			m_HasWhiteCastled = true;
		else
			m_HasBlackCastled = true;
	}

	//Update castling rights
	//King moves
	move.m_CanWhiteCastleKingSideBackup = m_CanWhiteCastleKingSide;
	move.m_CanWhiteCastleQueenSideBackup = m_CanWhiteCastleQueenSide;
	move.m_CanBlackCastleKingSideBackup = m_CanBlackCastleKingSide;
	move.m_CanBlackCastleQueenSideBackup = m_CanBlackCastleQueenSide;
	if (move.m_From.m_Type == PieceType::King)
	{
		if (IsWhiteToPlay())
		{
			SetCanWhiteCastleKingSide(false);
			SetCanWhiteCastleQueenSide(false);
		}
		else
		{
			SetCanBlackCastleKingSide(false);
			SetCanBlackCastleQueenSide(false);
		}
	}
	else
	{
		//Rook moves
		if ((move.m_From.m_Square == a1) || (move.m_To.m_Square == a1)) //rook move or capture
			SetCanWhiteCastleQueenSide(false);
		if ((move.m_From.m_Square == h1) || (move.m_To.m_Square == h1))
			SetCanWhiteCastleKingSide(false);
		if ((move.m_From.m_Square == a8) || (move.m_To.m_Square == a8))
			SetCanBlackCastleQueenSide(false);
		if ((move.m_From.m_Square == h8) || (move.m_To.m_Square == h8))
			SetCanBlackCastleKingSide(false);
	}

	m_IsWhiteToPlay = !m_IsWhiteToPlay;
	m_ZobristHash ^= ZobristHash::GetBlackToMoveKey();

	GetMoves().push_back(move);
}

void Position::Undo(const Move& move)
{
	//undo moved piece
	std::vector<Piece>& friendlyPieces = !IsWhiteToPlay() ? GetWhitePiecesList() : GetBlackPiecesList();
	for (Piece& friendlyPiece : friendlyPieces)
	{
		if (friendlyPiece.m_Square == move.m_To.m_Square)
		{
			friendlyPiece = move.m_From;
			break;
		}
	}

	m_ZobristHash ^= ZobristHash::GetKey(move.m_To, !IsWhiteToPlay());
	m_ZobristHash ^= ZobristHash::GetKey(move.m_From, !IsWhiteToPlay());

	//undo capture
	std::vector<Piece>& enemyPieces = !IsWhiteToPlay() ? GetBlackPiecesList() : GetWhitePiecesList();
	if (move.m_Capture.has_value())
	{
		enemyPieces.emplace_back(*move.m_Capture);
		m_ZobristHash ^= ZobristHash::GetKey(*move.m_Capture, IsWhiteToPlay());
	}

	//undo en passant square
	if (move.m_EnPassantBackup.has_value())
		SetEnPassantSquare(*move.m_EnPassantBackup);
	else
		ResetEnPassantSquare();

	//undo moved rook for castles
	if (move.IsCastling())
	{
		if (move.m_To.m_Square > move.m_From.m_Square) //kingside
		{
			//search the corresponding rook
			for (Piece& friendlyPiece : friendlyPieces)
			{
				if (friendlyPiece.m_Square == (!IsWhiteToPlay() ? f1 : f8))
				{
					assert(friendlyPiece.m_Type == PieceType::Rook);
					m_ZobristHash ^= ZobristHash::GetKey(friendlyPiece, !IsWhiteToPlay());
					friendlyPiece.m_Square = (!IsWhiteToPlay() ? h1 : h8);
					m_ZobristHash ^= ZobristHash::GetKey(friendlyPiece, !IsWhiteToPlay());
					break;
				}
			}
		}
		else //queenside
		{
			for (Piece& friendlyPiece : friendlyPieces)
			{
				if (friendlyPiece.m_Square == (!IsWhiteToPlay() ? d1 : d8))
				{
					assert(friendlyPiece.m_Type == PieceType::Rook);
					m_ZobristHash ^= ZobristHash::GetKey(friendlyPiece, !IsWhiteToPlay());
					friendlyPiece.m_Square = (!IsWhiteToPlay() ? a1 : a8);
					m_ZobristHash ^= ZobristHash::GetKey(friendlyPiece, !IsWhiteToPlay());
					break;
				}
			}
		}
	}

	//Undo castling flags
	SetCanWhiteCastleKingSide(move.m_CanWhiteCastleKingSideBackup);
	SetCanWhiteCastleQueenSide(move.m_CanWhiteCastleQueenSideBackup);
	SetCanBlackCastleKingSide(move.m_CanBlackCastleKingSideBackup);
	SetCanBlackCastleQueenSide(move.m_CanBlackCastleQueenSideBackup);
	if (move.IsCastling())
	{
		if (!m_IsWhiteToPlay)
			m_HasWhiteCastled = false;
		else
			m_HasBlackCastled = false;
	}

	GetMoves().pop_back();

	m_IsWhiteToPlay = !m_IsWhiteToPlay;
	m_ZobristHash ^= ZobristHash::GetBlackToMoveKey();
}

bool Position::AreEqual(const Position& position) const
{
	bool areEqual = (m_IsWhiteToPlay == position.IsWhiteToPlay()) &&
		(m_CanWhiteCastleKingSide == position.CanWhiteCastleKingSide()) &&
		(m_CanBlackCastleKingSide == position.CanBlackCastleKingSide()) &&
		(m_CanWhiteCastleQueenSide == position.CanWhiteCastleQueenSide()) &&
		(m_CanBlackCastleQueenSide == position.CanBlackCastleQueenSide()) &&
		(m_EnPassantSquare == position.GetEnPassantSquare()) &&
		(m_WhitePiecesList.size() == position.GetWhitePiecesList().size()) &&
		(m_BlackPiecesList.size() == position.GetBlackPiecesList().size());
	if (!areEqual)
		return areEqual;

	for (const Piece& piece : m_WhitePiecesList)
	{
		bool isFound = false;
		for (const Piece& piece2 : position.GetWhitePiecesList())
		{
			if (piece == piece2)
			{
				isFound = true;
				break;
			}
		}

		if (!isFound)
			return false;
	}

	for (const Piece& piece : m_BlackPiecesList)
	{
		bool isFound = false;
		for (const Piece& piece2 : position.GetBlackPiecesList())
		{
			if (piece == piece2)
			{
				isFound = true;
				break;
			}
		}

		if (!isFound)
			return false;
	}

	return areEqual;
}

bool Position::CheckBitboardsSanity() const
{
	Bitboard whitePieces2 = m_WhitePawns | m_WhiteKnights | m_WhiteBishops | m_WhiteRooks | m_WhiteQueens | m_WhiteKing;
	Bitboard blackPieces2 = m_BlackPawns | m_BlackKnights | m_BlackBishops | m_BlackRooks | m_BlackQueens | m_BlackKing;
	Bitboard allPieces2 = whitePieces2 | blackPieces2;
	bool isOk = (whitePieces2 == m_WhitePieces);
	isOk = isOk && (blackPieces2 == m_BlackPieces);
	isOk = isOk && (allPieces2 == (m_WhitePieces | m_BlackPieces));

	Bitboard whitePieces3;
	for (const Piece& piece : GetWhitePiecesList())
	{
		whitePieces3 |= Bitboard(piece.m_Square);
	}
	Bitboard blackPieces3;
	for (const Piece& piece : GetBlackPiecesList())
	{
		blackPieces3 |= Bitboard(piece.m_Square);
	}

	isOk |= ((whitePieces3 | blackPieces3) == (m_WhitePieces | m_BlackPieces));

	//check overlap
	isOk = isOk & ((allPieces2 & whitePieces2) == Bitboard());
	isOk = isOk & ((m_WhitePawns & m_WhiteKnights) == Bitboard());
	isOk = isOk & ((m_BlackPawns & m_BlackKnights) == Bitboard());
	isOk = isOk & ((m_WhitePawns & m_WhiteBishops) == Bitboard());
	isOk = isOk & ((m_BlackPawns & m_BlackBishops) == Bitboard());
	isOk = isOk & ((m_WhitePawns & m_WhiteRooks) == Bitboard());
	isOk = isOk & ((m_BlackPawns & m_BlackRooks) == Bitboard());

	return isOk;
}

void Position::UpdatePiece(const Move& move)
{
	for (const Piece* piece : {&move.m_From, &move.m_To})
	{
		const Bitboard movedPiece(piece->m_Square);

		switch (piece->m_Type)
		{
		case PieceType::Pawn:
			(m_IsWhiteToPlay ? m_WhitePawns : m_BlackPawns) ^= movedPiece;
			break;
		case PieceType::Knight:
			(m_IsWhiteToPlay ? m_WhiteKnights : m_BlackKnights) ^= movedPiece;
			break;
		case PieceType::Bishop:
			(m_IsWhiteToPlay ? m_WhiteBishops : m_BlackBishops) ^= movedPiece;
			break;
		case PieceType::Rook:
			(m_IsWhiteToPlay ? m_WhiteRooks : m_BlackRooks) ^= movedPiece;
			break;
		case PieceType::Queen:
			(m_IsWhiteToPlay ? m_WhiteQueens : m_BlackQueens) ^= movedPiece;
			break;
		case PieceType::King:
			(m_IsWhiteToPlay ? m_WhiteKing : m_BlackKing) ^= movedPiece;
			break;
		default:
			assert(false);
			break;
		}

		(m_IsWhiteToPlay ? m_WhitePieces : m_BlackPieces) ^= movedPiece;
	}
}

void Position::UpdateCapturedPiece(int squareIdx)
{
	const Bitboard capturedPiece(squareIdx);
	Bitboard* bitboard = (m_IsWhiteToPlay ? &m_BlackPawns : &m_WhitePawns);
	*bitboard ^= (*bitboard & capturedPiece);
	bitboard = (m_IsWhiteToPlay ? &m_BlackKnights : &m_WhiteKnights);
	*bitboard ^= (*bitboard & capturedPiece);
	bitboard = (m_IsWhiteToPlay ? &m_BlackBishops : &m_WhiteBishops);
	*bitboard ^= (*bitboard & capturedPiece);
	bitboard = (m_IsWhiteToPlay ? &m_BlackRooks : &m_WhiteRooks);
	*bitboard ^= (*bitboard & capturedPiece);
	bitboard = (m_IsWhiteToPlay ? &m_BlackQueens : &m_WhiteQueens);
	*bitboard ^= (*bitboard & capturedPiece);
	bitboard = (m_IsWhiteToPlay ? &m_BlackKing : &m_WhiteKing);
	*bitboard ^= (*bitboard & capturedPiece);
	bitboard = (m_IsWhiteToPlay ? &m_BlackPieces : &m_WhitePieces);
	*bitboard ^= (*bitboard & capturedPiece);
}
