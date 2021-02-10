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
	//m_WhiteUndevelopedPieces = _1 ^ _e8;

	m_BlackPawns = _7;
	m_BlackKnights = _b8 | _g8;
	m_BlackBishops = _c8 | _f8;
	m_BlackRooks = _a8 | _h8;
	m_BlackQueens = _d8;
	m_BlackKing = _e8;
	m_BlackPieces = m_BlackPawns | m_BlackKnights | m_BlackBishops | m_BlackRooks | m_BlackQueens | m_BlackKing;
	//m_BlackUndevelopedPieces = _8 ^ _e1;

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
	CommitToHistory();
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

bool Position::IsInsufficientMaterialFromBitboards() const
{
	if ((m_WhitePawns > 0) || (m_BlackPawns > 0) ||
		(m_WhiteRooks > 0) || (m_BlackRooks > 0) ||
		(m_WhiteQueens > 0) || (m_BlackQueens > 0))
		return false;

	int minorPieceCount = (m_WhiteKnights | m_BlackKnights | m_WhiteBishops | m_BlackBishops).CountSetBits();
	return (minorPieceCount < 2);
}

const Bitboard& Position::GetPiecesOfType(PieceType type, bool isWhite) const
{
	switch (type)
	{
	case PieceType::Pawn:
		return (isWhite ? m_WhitePawns : m_BlackPawns);
	case PieceType::Knight:
		return (isWhite ? m_WhiteKnights : m_BlackKnights);
	case PieceType::Bishop:
		return (isWhite ? m_WhiteBishops : m_BlackBishops);
	case PieceType::Rook:
		return (isWhite ? m_WhiteRooks : m_BlackRooks);
	case PieceType::Queen:
		return (isWhite ? m_WhiteQueens : m_BlackQueens);
	case PieceType::King:
		return (isWhite ? m_WhiteKing : m_BlackKing);
	default:
		assert(false);
		return m_WhitePawns;
	}
}

Bitboard& Position::GetPiecesOfType(PieceType type, bool isWhite)
{
	switch (type)
	{
	case PieceType::Pawn:
		return (isWhite ? m_WhitePawns : m_BlackPawns);
	case PieceType::Knight:
		return (isWhite ? m_WhiteKnights : m_BlackKnights);
	case PieceType::Bishop:
		return (isWhite ? m_WhiteBishops : m_BlackBishops);
	case PieceType::Rook:
		return (isWhite ? m_WhiteRooks : m_BlackRooks);
	case PieceType::Queen:
		return (isWhite ? m_WhiteQueens : m_BlackQueens);
	case PieceType::King:
		return (isWhite ? m_WhiteKing : m_BlackKing);
	default:
		assert(false);
		return m_WhitePawns;
	}
}

std::vector<Piece> Position::GetPieces(PieceType type, bool isWhite) const
{
	const Bitboard& bitboard = GetPiecesOfType(type, isWhite);

	//loop on set bits, get pieces
	std::vector<Piece> pieces;
	uint64_t bitset = bitboard;
	while (bitset != 0)
	{
		const uint64_t t = bitset & (~bitset + 1);
		const int idx = static_cast<int>(_tzcnt_u64(bitset));
		pieces.emplace_back(Piece(type, idx));
		bitset ^= t;
	}

	return pieces;
}

std::optional<Piece> Position::GetPiece(PieceType type, bool isWhite) const
{
	std::optional<Piece> piece;
	std::vector<Piece> pieces = GetPieces(type, isWhite);
	if (!pieces.empty())
		piece = pieces.front();
	
	return piece;
}

std::vector<Piece> Position::GetPiecesToPlay(PieceType type) const
{
	return GetPieces(type, IsWhiteToPlay());
}

std::vector<Piece> Position::GetPiecesToPlay() const
{
	std::vector<Piece> pieces = GetPiecesToPlay(PieceType::Pawn);
	std::vector<Piece> knights = GetPiecesToPlay(PieceType::Knight);
	pieces.insert(pieces.end(), knights.begin(), knights.end());
	std::vector<Piece> bishops = GetPiecesToPlay(PieceType::Bishop);
	pieces.insert(pieces.end(), bishops.begin(), bishops.end());
	std::vector<Piece> rooks = GetPiecesToPlay(PieceType::Rook);
	pieces.insert(pieces.end(), rooks.begin(), rooks.end());
	std::vector<Piece> queens = GetPiecesToPlay(PieceType::Queen);
	pieces.insert(pieces.end(), queens.begin(), queens.end());
	std::vector<Piece> king = GetPiecesToPlay(PieceType::King);
	pieces.insert(pieces.end(), king.begin(), king.end());

	return pieces;
}

std::vector<std::pair<Piece, Piece>> Position::GetPieceMoves() const
{
	std::vector<std::pair<Piece, Piece>> moves;
	for (const Move& move : m_Moves)
	{
		moves.emplace_back(move.GetFrom(), move.GetTo());
	}

	return moves;
}

uint64_t Position::ComputeZobristHash() const
{
	uint64_t hash = 0;
	for (const Piece& piece : GetWhitePiecesList())
	{
		hash ^= ZobristHash::GetKey(piece.m_Type, piece.m_Square, true);
	}
	for (const Piece& piece : GetBlackPiecesList())
	{
		hash ^= ZobristHash::GetKey(piece.m_Type, piece.m_Square, false);
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
	//null move update
	if (move.IsNullMove())
	{
		UpdateEnPassantSquare(move);
		m_IsWhiteToPlay = !m_IsWhiteToPlay;
		m_ZobristHash ^= ZobristHash::GetBlackToMoveKey();
		m_Moves.push_back(move);
		return;
	}

	//update moved piece
	UpdatePiece(move, m_IsWhiteToPlay);

	if (m_MaintainPiecesList)
	{
		std::vector<Piece>& friendlyPieces = IsWhiteToPlay() ? GetWhitePiecesList() : GetBlackPiecesList();
		for (Piece& friendlyPiece : friendlyPieces)
		{
			if (friendlyPiece.m_Square == move.GetFromSquare())
			{
				friendlyPiece = move.GetTo(); //update type too for queening
				break;
			}
		}
	}

	//we have to correct new position by removing a piece if taken!
	const bool isCaptureEnPassant = //necessarily en passant capture if a new pawn takes the EP square
		(m_EnPassantSquare.has_value() &&
		(move.GetFromType() == PieceType::Pawn) &&
		(move.GetToSquare() == *m_EnPassantSquare));
	int captureSquare = move.GetToSquare();
	if (isCaptureEnPassant)
		captureSquare += (IsWhiteToPlay() ? -1 : 1) * 8;


	assert(captureSquare <= 64);
	//Update captured piece
	UpdateCapturedPiece(static_cast<Square>(captureSquare), move);

	if (m_MaintainPiecesList)
	{
		std::vector<Piece>& enemyPieces = IsWhiteToPlay() ? GetBlackPiecesList() : GetWhitePiecesList();
		std::optional<size_t> indexPieceToRemove;
		for (size_t i = 0; i < enemyPieces.size(); i++)
		{
			if (enemyPieces[i].m_Square == captureSquare)
			{
				indexPieceToRemove = i;
				break;
			}
		}

		if (indexPieceToRemove.has_value())
		{
			assert((enemyPieces[*indexPieceToRemove].m_Type == PieceType::Pawn) || !isCaptureEnPassant);
			enemyPieces.erase(enemyPieces.begin() + *indexPieceToRemove);
		}
	}

	//Set or reset en passant square
	UpdateEnPassantSquare(move);

	//Move rook if castle, update flags
	if (move.IsCastling())
	{
		if (IsWhiteToPlay())
			m_HasWhiteCastled = true;
		else
			m_HasBlackCastled = true;

		Move rookMove;
		rookMove.SetFromType(PieceType::Rook);
		rookMove.SetToType(PieceType::Rook);

		if (move.GetToSquare() > move.GetFromSquare()) //kingside
		{
			if (m_IsWhiteToPlay)
			{
				rookMove.SetFromSquare(h1);
				rookMove.SetToSquare(f1);
			}
			else
			{
				rookMove.SetFromSquare(h8);
				rookMove.SetToSquare(f8);
			}
		}
		else //queenside
		{
			if (m_IsWhiteToPlay)
			{
				rookMove.SetFromSquare(a1);
				rookMove.SetToSquare(d1);
			}
			else
			{
				rookMove.SetFromSquare(a8);
				rookMove.SetToSquare(d8);
			}
		}

		UpdatePiece(rookMove, m_IsWhiteToPlay);

		if (m_MaintainPiecesList)
		{
			//search the corresponding rook
			std::vector<Piece>& friendlyPieces = IsWhiteToPlay() ? GetWhitePiecesList() : GetBlackPiecesList();
			for (Piece& friendlyPiece : friendlyPieces)
			{
				if (friendlyPiece == rookMove.GetFrom())
				{
					assert(friendlyPiece.m_Type == PieceType::Rook);
					friendlyPiece.m_Square = rookMove.GetToSquare();
					break;
				}
			}
		}
	}

	//Update castling rights
	//King moves
	move.SetCanWhiteCastleKingSideBackup(m_CanWhiteCastleKingSide);
	move.SetCanWhiteCastleQueenSideBackup(m_CanWhiteCastleQueenSide);
	move.SetCanBlackCastleKingSideBackup(m_CanBlackCastleKingSide);
	move.SetCanBlackCastleQueenSideBackup(m_CanBlackCastleQueenSide);
	if (move.GetFromType() == PieceType::King)
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
		if ((move.GetFromSquare() == a1) || (move.GetToSquare() == a1)) //rook move or capture
			SetCanWhiteCastleQueenSide(false);
		if ((move.GetFromSquare() == h1) || (move.GetToSquare() == h1))
			SetCanWhiteCastleKingSide(false);
		if ((move.GetFromSquare() == a8) || (move.GetToSquare() == a8))
			SetCanBlackCastleQueenSide(false);
		if ((move.GetFromSquare() == h8) || (move.GetToSquare() == h8))
			SetCanBlackCastleKingSide(false);
	}

	m_IsWhiteToPlay = !m_IsWhiteToPlay;
	m_ZobristHash ^= ZobristHash::GetBlackToMoveKey();
	CommitToHistory();

	m_Moves.push_back(move);
}

void Position::Undo(const Move& move)
{
	UncommitToHistory(m_ZobristHash);

	if (move.IsNullMove())
	{
		UndoEnPassantSquare(move);
		m_IsWhiteToPlay = !m_IsWhiteToPlay;
		m_ZobristHash ^= ZobristHash::GetBlackToMoveKey();
		m_Moves.pop_back();
		return;
	}

	//Undo moved piece
	UndoPiece(move, !m_IsWhiteToPlay);

	if (m_MaintainPiecesList)
	{
		std::vector<Piece>& friendlyPieces = !IsWhiteToPlay() ? GetWhitePiecesList() : GetBlackPiecesList();
		for (Piece& friendlyPiece : friendlyPieces)
		{
			if (friendlyPiece.m_Square == move.GetToSquare())
			{
				friendlyPiece = move.GetFrom();
				break;
			}
		}
	}

	//Undo capture
	if (move.IsCapture())
	{
		UpdateSquare(move.GetCaptureType(), move.GetCaptureSquare(), m_IsWhiteToPlay);

		if (m_MaintainPiecesList)
		{
			std::vector<Piece>& enemyPieces = !m_IsWhiteToPlay ? GetBlackPiecesList() : GetWhitePiecesList();
			enemyPieces.emplace_back(Piece(move.GetCaptureType(), move.GetCaptureSquare()));
		}
	}

	//Undo en passant square
	UndoEnPassantSquare(move);

	//undo moved rook for castles
	if (move.IsCastling())
	{
		if (move.GetToSquare() > move.GetFromSquare()) //kingside
		{
			Move undoRookMove(PieceType::Rook, (!IsWhiteToPlay() ? f1 : f8), (!IsWhiteToPlay() ? h1 : h8));
			UpdatePiece(undoRookMove, !m_IsWhiteToPlay);

			if (m_MaintainPiecesList)
			{
				//search the corresponding rook
				std::vector<Piece>& friendlyPieces = !IsWhiteToPlay() ? GetWhitePiecesList() : GetBlackPiecesList();
				for (Piece& friendlyPiece : friendlyPieces)
				{
					if (friendlyPiece.m_Square == (!IsWhiteToPlay() ? f1 : f8))
					{
						assert(friendlyPiece.m_Type == PieceType::Rook);
						friendlyPiece.m_Square = (!IsWhiteToPlay() ? h1 : h8);
						break;
					}
				}
			}
		}
		else //queenside
		{
			Move undoRookMove(PieceType::Rook, (!IsWhiteToPlay() ? d1 : d8), (!IsWhiteToPlay() ? a1 : a8));
			UpdatePiece(undoRookMove, !m_IsWhiteToPlay);

			if (m_MaintainPiecesList)
			{
				//search the corresponding rook
				std::vector<Piece>& friendlyPieces = !IsWhiteToPlay() ? GetWhitePiecesList() : GetBlackPiecesList();
				for (Piece& friendlyPiece : friendlyPieces)
				{
					if (friendlyPiece.m_Square == (!IsWhiteToPlay() ? d1 : d8))
					{
						assert(friendlyPiece.m_Type == PieceType::Rook);
						friendlyPiece.m_Square = (!IsWhiteToPlay() ? a1 : a8);
						break;
					}
				}
			}
		}
	}

	//Undo castling flags
	SetCanWhiteCastleKingSide(move.GetCanWhiteCastleKingSideBackup());
	SetCanWhiteCastleQueenSide(move.GetCanWhiteCastleQueenSideBackup());
	SetCanBlackCastleKingSide(move.GetCanBlackCastleKingSideBackup());
	SetCanBlackCastleQueenSide(move.GetCanBlackCastleQueenSideBackup());
	if (move.IsCastling())
	{
		if (!m_IsWhiteToPlay)
			m_HasWhiteCastled = false;
		else
			m_HasBlackCastled = false;
	}

	m_Moves.pop_back();

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
	isOk = isOk && ((allPieces2 & whitePieces2) == Bitboard());
	isOk = isOk && ((m_WhitePawns & m_WhiteKnights) == Bitboard());
	isOk = isOk && ((m_BlackPawns & m_BlackKnights) == Bitboard());
	isOk = isOk && ((m_WhitePawns & m_WhiteBishops) == Bitboard());
	isOk = isOk && ((m_BlackPawns & m_BlackBishops) == Bitboard());
	isOk = isOk && ((m_WhitePawns & m_WhiteRooks) == Bitboard());
	isOk = isOk && ((m_BlackPawns & m_BlackRooks) == Bitboard());

	return isOk;
}

void Position::CommitToHistory(uint64_t key)
{
	m_History[key & 0x3FFF]++;
}

void Position::CommitToHistory()
{
	CommitToHistory(m_ZobristHash);
}

void Position::UncommitToHistory(uint64_t key)
{
	m_History[key & 0x3FFF]--;
}

int Position::GetHistoryCount()
{
	return m_History[m_ZobristHash & 0x3FFF]; //we don't prevent a crash
}

void Position::UpdatePiece(const Move& move, bool isWhite)
{
	UpdateSquare(move.GetFromType(), move.GetFromSquare(), isWhite);
	UpdateSquare(move.GetToType(), move.GetToSquare(), isWhite);
}

void Position::UndoPiece(const Move& move, bool isWhite)
{
	UpdateSquare(move.GetToType(), move.GetToSquare(), isWhite);
	UpdateSquare(move.GetFromType(), move.GetFromSquare(), isWhite);
}

void Position::UpdateSquare(PieceType type, Square square, bool isWhite)
{
	const Bitboard movedPiece(square);

	GetPiecesOfType(type, isWhite) ^= movedPiece;
	(isWhite ? m_WhitePieces : m_BlackPieces) ^= movedPiece;

	//update zobrist hash the same way
	m_ZobristHash ^= ZobristHash::GetKey(type, square, isWhite);
}

void Position::UpdateCapturedPiece(Square squareIdx, Move& move)
{
	const Bitboard captureSquare(squareIdx);

	//update full set
	Bitboard* bitboard = (m_IsWhiteToPlay ? &m_BlackPieces : &m_WhitePieces);
	if (!(*bitboard & captureSquare))
		return;
	*bitboard ^= (*bitboard & captureSquare);

	//update piece type set
	 bitboard = (m_IsWhiteToPlay ? &m_BlackPawns : &m_WhitePawns);
	if ((*bitboard & captureSquare) > 0)
	{
		*bitboard ^= (*bitboard & captureSquare);
		move.SetCapture(PieceType::Pawn, squareIdx);
		m_ZobristHash ^= ZobristHash::GetKey(PieceType::Pawn, squareIdx, !m_IsWhiteToPlay);
		return;
	}

	bitboard = (m_IsWhiteToPlay ? &m_BlackKnights : &m_WhiteKnights);
	if ((*bitboard & captureSquare) > 0)
	{
		*bitboard ^= (*bitboard & captureSquare);
		move.SetCapture(PieceType::Knight, squareIdx);
		m_ZobristHash ^= ZobristHash::GetKey(PieceType::Knight, squareIdx, !m_IsWhiteToPlay);
		return;
	}

	bitboard = (m_IsWhiteToPlay ? &m_BlackBishops : &m_WhiteBishops);
	if ((*bitboard & captureSquare) > 0)
	{
		*bitboard ^= (*bitboard & captureSquare);
		move.SetCapture(PieceType::Bishop, squareIdx);
		m_ZobristHash ^= ZobristHash::GetKey(PieceType::Bishop, squareIdx, !m_IsWhiteToPlay);
		return;
	}

	bitboard = (m_IsWhiteToPlay ? &m_BlackRooks : &m_WhiteRooks);
	if ((*bitboard & captureSquare) > 0)
	{
		*bitboard ^= (*bitboard & captureSquare);
		move.SetCapture(PieceType::Rook, squareIdx);
		m_ZobristHash ^= ZobristHash::GetKey(PieceType::Rook, squareIdx, !m_IsWhiteToPlay);
		return;
	}

	bitboard = (m_IsWhiteToPlay ? &m_BlackQueens : &m_WhiteQueens);
	if ((*bitboard & captureSquare) > 0)
	{
		*bitboard ^= (*bitboard & captureSquare);
		move.SetCapture(PieceType::Queen, squareIdx);
		m_ZobristHash ^= ZobristHash::GetKey(PieceType::Queen, squareIdx, !m_IsWhiteToPlay);
		return;
	}

	bitboard = (m_IsWhiteToPlay ? &m_BlackKing : &m_WhiteKing);
	if ((*bitboard & captureSquare) > 0)
	{
		*bitboard ^= (*bitboard & captureSquare);
		move.SetCapture(PieceType::King, squareIdx);
		m_ZobristHash ^= ZobristHash::GetKey(PieceType::King, squareIdx, !m_IsWhiteToPlay);
		return;
	}	
}

void Position::UpdateEnPassantSquare(Move& move)
{
	if (m_EnPassantSquare.has_value())
		move.SetEnPassantBackup(*m_EnPassantSquare); //backup en passant square
	if (move.IsTwoStepsPawn())
		SetEnPassantSquare(static_cast<Square>(move.GetFromSquare() + (IsWhiteToPlay() ? 8 : -8)));
	else
		ResetEnPassantSquare();
}

void Position::UndoEnPassantSquare(const Move& move)
{
	if (move.HasEnPassantBackup())
	{
		Square enPassantSquare = static_cast<Square>(move.GetEnPassantBackupFile() + (m_IsWhiteToPlay ? 16 : 40));
		SetEnPassantSquare(enPassantSquare);
	}
	else
		ResetEnPassantSquare();
}