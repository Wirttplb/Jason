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

/// <remark>Sorted by piece value</remark>
enum class PieceType : int
{
	Pawn = 0,
	Knight = 1,
	Bishop = 2,
	Rook = 3,
	Queen = 4,
	King = 5
};

/// <summary>description of a piece and its position</summary>
/// <remark>Usage should be limited at runtime, could be removed in the future, redundant with Bitboards and Move class</remark>
class Piece
{
public:
	Piece() {};
	Piece(PieceType type, Square square) : m_Type(type), m_Square(square) {};
	Piece(PieceType type, int square) : m_Type(type), m_Square(Square(square)) {};
	
	Square m_Square = a1;
	PieceType m_Type = PieceType::Pawn;

	/// <summary>Returns square position as {x, y} (Slow!)</summary>
	std::array<int, 2> Position() const { return { m_Square % 8, m_Square / 8 }; };

	bool operator==(const Piece& piece) const
	{
		return (piece.m_Square == m_Square) && (piece.m_Type == m_Type);
	}
};

/// <summary>Description of a move, with all information needed to undo the move</summary>
/// <remark>This class is easily instantiated millions of times at runtime, single integer member variable to reduce memory footprint</remark>
class Move
{
public:
	Move() {};
	Move(PieceType type, Square from, Square to)
	{
		m_Move = static_cast<uint64_t>((from & 0x3f) | ((to & 0x3f) << 6) | ((static_cast<int>(type) & 0x7) << 12) | ((static_cast<int>(type) & 0x7) << 15));
	};
	Move(PieceType fromType, PieceType toType, Square from, Square to)
	{
		m_Move = static_cast<uint64_t>((from & 0x3f) | ((to & 0x3f) << 6) | ((static_cast<int>(fromType) & 0x7) << 12) | ((static_cast<int>(toType) & 0x7) << 15));
	};

	/// <remark>Slow operator, only for tests</remark>
	bool operator==(const Move& move) const
	{
		return (GetFrom() == move.GetFrom()) && (GetTo() == move.GetTo()); //m_Move == move.m_Move;
	}

	/// <summary>bits 0 to 5 = From square</summary>
	inline Square GetFromSquare() const { return static_cast<Square>(m_Move & 0x3f); };
	/// <summary>bits 6 to 11 = To square</summary>
	inline Square GetToSquare() const { return static_cast<Square>((m_Move >> 6) & 0x3f); }

	/// <summary>bits 12 to 14 is from type</summary>
	inline PieceType GetFromType() const
	{
		return static_cast<PieceType>((m_Move >> 12) & 0x07);
	};

	/// <summary>bits 15 to 17 is from type</summary>
	inline PieceType GetToType() const
	{
		return static_cast<PieceType>((m_Move >> 15) & 0x07);
	};

	inline void SetFromSquare(Square from)
	{
		m_Move &= ~0x3f;
		m_Move |= from & 0x3f;
	}

	inline void SetToSquare(Square to)
	{
		m_Move &= ~0xfc0;
		m_Move |= (static_cast<uint64_t>(to) & 0x3f) << 6;
	}

	inline void SetFromType(PieceType type)
	{
		m_Move &= ~0x7000;
		m_Move |= (static_cast<uint64_t>(type) & 0x7) << 12;
	}

	inline void SetToType(PieceType type)
	{
		m_Move &= ~0x38000;
		m_Move |= (static_cast<uint64_t>(type) & 0x7) << 15;
	}

	/// <summary>Returns from piece ; Slow</summary>
	Piece GetFrom() const { return Piece(GetFromType(), GetFromSquare()); };
	/// <summary>returns to piece ; Slow</summary>
	Piece GetTo() const { return Piece(GetToType(), GetToSquare()); };

	inline void SetFrom(PieceType type, Square square)
	{
		SetFromType(type);
		SetFromSquare(square);
	};

	inline void SetTo(PieceType type, Square square)
	{
		SetToType(type);
		SetToSquare(square);
	};

	/// <summary>capture flag is bit 18</summary>
	inline bool IsCapture() const
	{
		return ((m_Move >> 18) & 0x1) > 0;
	};

	/// <summary>capture type is bits 19, 20, 21</summary>
	inline PieceType GetCaptureType() const
	{
		return static_cast<PieceType>((m_Move >> 19) & 0x07);
	}

	/// <summary>capture square is bits 22 to 27</summary>
	inline Square GetCaptureSquare() const
	{
		return static_cast<Square>((m_Move >> 22) & 0x3f);
	}

	inline void SetCapture(PieceType type, Square square)
	{
		//m_Capture = Piece(type, square);
		m_Move &= ~0xFFC0000;
		m_Move |= 1 << 18; //is capture flag
		m_Move |= (static_cast<uint64_t>(type) & 0x7) << 19;
		m_Move |= (static_cast<uint64_t>(square) & 0x3f) << 22;
	};

	/// <summary>en passant flag is bit 28</summary>
	inline bool HasEnPassantBackup() const
	{
		return ((m_Move >> 28) & 0x1) > 0;
	}

	/// <summary>en passant file is bits 29 to 31</summary>
	inline int GetEnPassantBackupFile() const
	{
		return ((m_Move >> 29) & 0x07);
	}

	inline void SetEnPassantBackup(Square square)
	{
		m_Move &= ~0xF0000000;
		m_Move |= 1 << 28; //has en passant backup flag
		m_Move |= static_cast<uint64_t>((square & 7) & 0x7) << 29;
	}

	inline bool GetCanWhiteCastleKingSideBackup() const
	{
		return ((m_Move >> 32) & 0x1) > 0;
	}

	inline bool GetCanBlackCastleKingSideBackup() const
	{
		return ((m_Move >> 33) & 0x1) > 0;
	}

	inline bool GetCanWhiteCastleQueenSideBackup() const
	{
		return ((m_Move >> 34) & 0x1) > 0;
	}

	inline bool GetCanBlackCastleQueenSideBackup() const
	{
		return ((m_Move >> 35) & 0x1) > 0;
	}

	inline void SetCanWhiteCastleKingSideBackup(bool value)
	{
		m_Move &= ~0x100000000;
		m_Move |= static_cast<uint64_t>(value ? 1 : 0) << 32;
	}

	inline void SetCanBlackCastleKingSideBackup(bool value)
	{
		m_Move &= ~0x200000000;
		m_Move |= static_cast<uint64_t>(value ? 1 : 0) << 33;
	}

	inline void SetCanWhiteCastleQueenSideBackup(bool value)
	{
		m_Move &= ~0x400000000;
		m_Move |= static_cast<uint64_t>(value ? 1 : 0) << 34;
	}

	inline void SetCanBlackCastleQueenSideBackup(bool value)
	{
		m_Move &= ~0x800000000;
		m_Move |= static_cast<uint64_t>(value ? 1 : 0) << 35;
	}

	inline bool IsCastling() const
	{
		return (GetFromType() == PieceType::King) &&
			(abs(GetFromSquare() - GetToSquare()) == 2); //2 lateral steps, same row
	}

	inline bool IsTwoStepsPawn() const
	{
		return (GetFromType() == PieceType::Pawn) &&
			(abs(GetFromSquare() - GetToSquare()) == 16);
	}

	inline bool IsQueening() const
	{
		if (GetFromType() == PieceType::Pawn)
		{
			const int toSquare = GetToSquare();
			return ((toSquare <= 7) || (toSquare >= 56));
		}

		return false;
	}

	inline bool IsNullMove() const
	{
		return (m_Move == 0);
	}

private:
	uint64_t m_Move = 0;
};

static const Move WhiteKingSideCastle(PieceType::King, e1, g1);
static const Move WhiteQueenSideCastle(PieceType::King, e1, c1);
static const Move BlackKingSideCastle(PieceType::King, e8, g8);
static const Move BlackQueenSideCastle(PieceType::King, e8, c8);