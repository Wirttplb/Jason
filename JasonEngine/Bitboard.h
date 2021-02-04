#pragma once
#include <stdint.h>
#include <array>
#include <string>
#include <ostream>
#include "BasicDefinitions.h"


/// <summary>
/// Bitboard representation, Little endian rank-file (LERF)
/// </summary>
/// 
/// Compass Rose for square indices:
/// noWe         north         noEa
///           +7 + 8 + 9
///             \  |  /
///    west - 1 < -0 -> + 1    east
///	            /  |  \
///	         - 9 - 8 - 7
///	soWe         south         soEa
/// 
class Bitboard
{
public:
	Bitboard();

	/// <summary>Constructor from list of bits, from a8-h8 to a1-h1, like a chessboard from above.</summary>
	Bitboard(const std::array<bool, 64>& bits);
	constexpr Bitboard(Square square);
	Bitboard(int square);

	Bitboard operator~() const;
	Bitboard operator&(const Bitboard& bitboard) const;
	void operator&=(const Bitboard& bitboard);
	Bitboard operator|(const Bitboard& bitboard) const;
	void operator|=(const Bitboard& bitboard);
	Bitboard operator^(const Bitboard& bitboard) const;
	void operator^=(const Bitboard& bitboard);
	Bitboard operator<<(int shift) const;
	Bitboard operator>>(int shift) const;
	Bitboard operator-(const Bitboard& bitboard) const;
	operator uint64_t() const { return m_Value; };

	bool operator==(const Bitboard& bitboard) const { return (m_Value == bitboard.m_Value); }; //for hash definition

	Bitboard RotateLeft(int s) const;
	Bitboard RotateRight(int s) const;
	Bitboard FlipVertically() const;
	Bitboard FlipA1H8() const;
	Bitboard Rotate90Clockwise() const;
	Bitboard Rotate90AntiClockwise() const;
	/// <remark> Main Diagonal is mapped to 1st rank </summary>
	Bitboard PseudoRotate45Clockwise() const;
	Bitboard PseudoRotate45AntiClockwise() const;
	Bitboard UndoPseudoRotate45Clockwise() const;
	Bitboard UndoPseudoRotate45AntiClockwise() const;

	int CountSetBits() const;

	/// <returns>Square index ; only a single bit should be set, otherwise unknown result</returns>
	/// <remark>Row index is square / 8 (or square >> 3) ; File index is square % 8 (or square & 7)<remark>
	int GetSquare() const;

	std::string ToString() const;

	uint64_t m_Value = 0;
};

template<>
struct std::hash<Bitboard>
{
	uint64_t operator()(const Bitboard& position) const
	{
		return position.m_Value;
	}
};

std::ostream& operator<<(std::ostream& os, const Bitboard& bitboard);

static const Bitboard _a1(Square::a1);
static const Bitboard _a2(Square::a2);
static const Bitboard _a3(Square::a3);
static const Bitboard _a4(Square::a4);
static const Bitboard _a5(Square::a5);
static const Bitboard _a6(Square::a6);
static const Bitboard _a7(Square::a7);
static const Bitboard _a8(Square::a8);

static const Bitboard _b1(Square::b1);
static const Bitboard _b2(Square::b2);
static const Bitboard _b3(Square::b3);
static const Bitboard _b4(Square::b4);
static const Bitboard _b5(Square::b5);
static const Bitboard _b6(Square::b6);
static const Bitboard _b7(Square::b7);
static const Bitboard _b8(Square::b8);

static const Bitboard _c1(Square::c1);
static const Bitboard _c2(Square::c2);
static const Bitboard _c3(Square::c3);
static const Bitboard _c4(Square::c4);
static const Bitboard _c5(Square::c5);
static const Bitboard _c6(Square::c6);
static const Bitboard _c7(Square::c7);
static const Bitboard _c8(Square::c8);

static const Bitboard _d1(Square::d1);
static const Bitboard _d2(Square::d2);
static const Bitboard _d3(Square::d3);
static const Bitboard _d4(Square::d4);
static const Bitboard _d5(Square::d5);
static const Bitboard _d6(Square::d6);
static const Bitboard _d7(Square::d7);
static const Bitboard _d8(Square::d8);

static const Bitboard _e1(Square::e1);
static const Bitboard _e2(Square::e2);
static const Bitboard _e3(Square::e3);
static const Bitboard _e4(Square::e4);
static const Bitboard _e5(Square::e5);
static const Bitboard _e6(Square::e6);
static const Bitboard _e7(Square::e7);
static const Bitboard _e8(Square::e8);

static const Bitboard _f1(Square::f1);
static const Bitboard _f2(Square::f2);
static const Bitboard _f3(Square::f3);
static const Bitboard _f4(Square::f4);
static const Bitboard _f5(Square::f5);
static const Bitboard _f6(Square::f6);
static const Bitboard _f7(Square::f7);
static const Bitboard _f8(Square::f8);

static const Bitboard _g1(Square::g1);
static const Bitboard _g2(Square::g2);
static const Bitboard _g3(Square::g3);
static const Bitboard _g4(Square::g4);
static const Bitboard _g5(Square::g5);
static const Bitboard _g6(Square::g6);
static const Bitboard _g7(Square::g7);
static const Bitboard _g8(Square::g8);

static const Bitboard _h1(Square::h1);
static const Bitboard _h2(Square::h2);
static const Bitboard _h3(Square::h3);
static const Bitboard _h4(Square::h4);
static const Bitboard _h5(Square::h5);
static const Bitboard _h6(Square::h6);
static const Bitboard _h7(Square::h7);
static const Bitboard _h8(Square::h8);

static const Bitboard _a
({
	1, 0, 0, 0, 0, 0, 0, 0,
	1, 0, 0, 0, 0, 0, 0, 0,
	1, 0, 0, 0, 0, 0, 0, 0,
	1, 0, 0, 0, 0, 0, 0, 0,
	1, 0, 0, 0, 0, 0, 0, 0,
	1, 0, 0, 0, 0, 0, 0, 0,
	1, 0, 0, 0, 0, 0, 0, 0,
	1, 0, 0, 0, 0, 0, 0, 0
});

static const Bitboard _b
({
	0, 1, 0, 0, 0, 0, 0, 0,
	0, 1, 0, 0, 0, 0, 0, 0,
	0, 1, 0, 0, 0, 0, 0, 0,
	0, 1, 0, 0, 0, 0, 0, 0,
	0, 1, 0, 0, 0, 0, 0, 0,
	0, 1, 0, 0, 0, 0, 0, 0,
	0, 1, 0, 0, 0, 0, 0, 0,
	0, 1, 0, 0, 0, 0, 0, 0
});

static const Bitboard _c
({
	0, 0, 1, 0, 0, 0, 0, 0,
	0, 0, 1, 0, 0, 0, 0, 0,
	0, 0, 1, 0, 0, 0, 0, 0,
	0, 0, 1, 0, 0, 0, 0, 0,
	0, 0, 1, 0, 0, 0, 0, 0,
	0, 0, 1, 0, 0, 0, 0, 0,
	0, 0, 1, 0, 0, 0, 0, 0,
	0, 0, 1, 0, 0, 0, 0, 0
});

static const Bitboard _d
({
	0, 0, 0, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 0, 0, 0, 0
});

static const Bitboard _e
({
	0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 0, 1, 0, 0, 0
});

static const Bitboard _f
({
	0, 0, 0, 0, 0, 1, 0, 0,
	0, 0, 0, 0, 0, 1, 0, 0,
	0, 0, 0, 0, 0, 1, 0, 0,
	0, 0, 0, 0, 0, 1, 0, 0,
	0, 0, 0, 0, 0, 1, 0, 0,
	0, 0, 0, 0, 0, 1, 0, 0,
	0, 0, 0, 0, 0, 1, 0, 0,
	0, 0, 0, 0, 0, 1, 0, 0
});

static const Bitboard _g
({
	0, 0, 0, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, 0, 1, 0
});

static const Bitboard _h
({
	0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 1
});

static const Bitboard _8
({
	1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
});

static const Bitboard _7
({
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
});

static const Bitboard _6
({
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
});

static const Bitboard _5
({
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
});

static const Bitboard _4
({
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
});

static const Bitboard _3
({
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
});

static const Bitboard _2
({
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0
});

static const Bitboard _1
({
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1
});

static const Bitboard LightSquares
({
	1, 0, 1, 0, 1, 0, 1, 0,
	0, 1, 0, 1, 0, 1, 0, 1,
	1, 0, 1, 0, 1, 0, 1, 0,
	0, 1, 0, 1, 0, 1, 0, 1,
	1, 0, 1, 0, 1, 0, 1, 0,
	0, 1, 0, 1, 0, 1, 0, 1,
	1, 0, 1, 0, 1, 0, 1, 0,
	0, 1, 0, 1, 0, 1, 0, 1
});

static const Bitboard DarkSquares
({
	0, 1, 0, 1, 0, 1, 0, 1,
	1, 0, 1, 0, 1, 0, 1, 0,
	0, 1, 0, 1, 0, 1, 0, 1,
	1, 0, 1, 0, 1, 0, 1, 0,
	0, 1, 0, 1, 0, 1, 0, 1,
	1, 0, 1, 0, 1, 0, 1, 0,
	0, 1, 0, 1, 0, 1, 0, 1,
	1, 0, 1, 0, 1, 0, 1, 0
});

static const Bitboard a1h8
({
	0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, 1, 0, 0,
	0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 1, 0, 0, 0, 0,
	0, 0, 1, 0, 0, 0, 0, 0,
	0, 1, 0, 0, 0, 0, 0, 0,
	1, 0, 0, 0, 0, 0, 0, 0
});

static const Bitboard b1h7
({
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, 1, 0, 0,
	0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 1, 0, 0, 0, 0,
	0, 0, 1, 0, 0, 0, 0, 0,
	0, 1, 0, 0, 0, 0, 0, 0
});

static const Bitboard c1h6
({
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, 1, 0, 0,
	0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 1, 0, 0, 0, 0,
	0, 0, 1, 0, 0, 0, 0, 0
});

static const std::array<Bitboard, 8> _rows = { _1, _2, _3, _4, _5, _6, _7, _8 };
static const std::array<Bitboard, 8> _files = { _a, _b, _c, _d, _e, _f, _g, _h };
static const Bitboard _bcdefg = _b & _c & _d & _e & _f & _g;

///<remark>Includes the r row</summary>
static constexpr Bitboard GetRowsAbove(int r)
{
	Bitboard rows;
	for (int i = r; i < 8; i++)
	{
		rows |= _rows[i];
	}

	return rows;
}

static constexpr Bitboard GetRowsUnder(int r)
{
	Bitboard rows;
	for (int i = r; i >= 0; i--)
	{
		rows |= _rows[i];
	}

	return rows;
}

///<remark>Includes the f file</summary>
static constexpr Bitboard GetFilesOnRight(int f)
{
	Bitboard files;
	for (int i = f; i < 8; i++)
	{
		files |= _files[i];
	}

	return files;
}

static constexpr Bitboard GetFilesOnLeft(int f)
{
	Bitboard files;
	for (int i = f; i >= 0; i--)
	{
		files |= _files[i];
	}

	return files;
}