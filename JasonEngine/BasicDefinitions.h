#pragma once
#include <array>
#include <optional>


/// <summary> Simple enum for square indices </summary>
/// <remark> x = s % 8, y = s / 8 ; +-1 to move along x, +-8 to move along y </remark>
enum Square : int
{
	a1 = 0, b1, c1, d1, e1, f1, g1, h1,
	a2, b2, c2, d2, e2, f2, g2, h2,
	a3, b3, c3, d3, e3, f3, g3, h3,
	a4, b4, c4, d4, e4, f4, g4, h4,
	a5, b5, c5, d5, e5, f5, g5, h5,
	a6, b6, c6, d6, e6, f6, g6, h6,
	a7, b7, c7, d7, e7, f7, g7, h7,
	a8, b8, c8, d8, e8, f8, g8, h8
};

enum class PieceType
{
	Pawn,
	Knight,
	Bishop,
	Rook,
	Queen,
	King
};

/// <summary>description of a piece and its position</summary>
class Piece
{
public:
	Piece() {};
	Piece(PieceType type, int square) : m_Type(type), m_Square(square) {};
	
	int m_Square = a1;
	PieceType m_Type = PieceType::Pawn;

	/// <summary>Returns position square index</summary>
	std::array<int, 2> Position() const { return { m_Square % 8, m_Square / 8 }; };

	bool operator==(const Piece& piece) const
	{
		return (piece.m_Square == m_Square) && (piece.m_Type == m_Type);
	}
};

/// <summary>Description of a move, with all information needed to undo the move</summary>
class Move
{
public:
	Move() {};
	Move(PieceType type, int from, int to) : m_From(type, from), m_To(type, to) {};

	bool IsCapture() const { return m_Capture.has_value(); };

	bool operator==(const Move& move) const
	{
		return (m_From == move.m_From && m_To == move.m_To); //dont check for capture
	}

	bool IsCastling() const
	{
		return (m_From.m_Type == PieceType::King) &&
			(abs(m_From.m_Square - m_To.m_Square) == 2); //at least 2 lateral steps, same row
	}

	bool IsTwoStepsPawn() const
	{
		return (m_From.m_Type == PieceType::Pawn) &&
			(abs(m_From.m_Square - m_To.m_Square) == 16);
	}

	Piece m_From;
	Piece m_To;
	std::optional<Piece> m_Capture; //captured piece
	std::optional<Square> m_EnPassantBackup; //en passant square BEFORE move

	bool m_CanWhiteCastleKingSideBackup = true; //backups flags for BEFORE move
	bool m_CanBlackCastleKingSideBackup = true;
	bool m_CanWhiteCastleQueenSideBackup = true;
	bool m_CanBlackCastleQueenSideBackup = true;
};