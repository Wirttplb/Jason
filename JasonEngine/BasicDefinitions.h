#pragma once
#include <array>

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

class Move
{
public:
	Piece m_From;
	Piece m_To;
	bool m_IsCapture = false;

	bool operator==(const Move& move) const
	{
		return (m_From == move.m_From && m_To == move.m_To); //dont check for capture
	}

	bool IsCastling() const
	{
		return ((m_From.m_Type == PieceType::King) && abs(m_From.m_Position[0] - m_To.m_Position[0]) > 1);
	}
};