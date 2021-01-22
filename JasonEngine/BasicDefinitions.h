#pragma once
#include <array>
#include <optional>

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
	Piece(PieceType type, int x, int y) : m_Type(type), m_Position({ x,y }) {};
	Piece(PieceType type, const std::array<int, 2> square) : m_Type(type), m_Position(square) {};
	std::array<int, 2> m_Position = { 0, 0 }; //values should range from 0 to 7
	PieceType m_Type = PieceType::Pawn;

	bool operator==(const Piece& piece) const
	{
		return (piece.m_Position == m_Position) && (piece.m_Type == m_Type);
	}
};

/// <summary>Description of a move, with all information needed to undo the move</summary>
class Move
{
public:
	bool IsCapture() const { return m_Capture.has_value(); };

	bool operator==(const Move& move) const
	{
		return (m_From == move.m_From && m_To == move.m_To); //dont check for capture
	}

	bool IsCastling() const
	{
		return ((m_From.m_Type == PieceType::King) && abs(m_From.m_Position[0] - m_To.m_Position[0]) > 1);
	}

	bool IsTwoStepsPawn() const
	{
		return (m_From.m_Type == PieceType::Pawn) && (abs(m_From.m_Position[1] - m_To.m_Position[1]) == 2);
	}

	Piece m_From;
	Piece m_To;
	std::optional<Piece> m_Capture; //captured piece
	std::optional<std::array<int, 2>> m_EnPassantBackup; //en passant square BEFORE move

	bool m_CanWhiteCastleKingSideBackup = true; //backups flags for BEFORE move
	bool m_CanBlackCastleKingSideBackup = true;
	bool m_CanWhiteCastleQueenSideBackup = true;
	bool m_CanBlackCastleQueenSideBackup = true;
};