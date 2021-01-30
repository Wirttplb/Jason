#include "pch.h"
#include "PositionEvaluation.h"
#include "MoveMaker.h"
#include "MoveSearcher.h"
#include "BitboardUtility.h"
#include <assert.h>

/// <summary>Absolute score for a mate</summary>
static constexpr double Mate = 1000000;

static constexpr double BishopPairBonus = 15.0;
static constexpr double CastlingBonus = 50.0;

static constexpr double CenterPawnBonus = 50.0;
static constexpr double DoubledPawnPunishment = -20.0; //40 for a pair
static constexpr double IsolatedPawnPunishment = -40.0;

static constexpr double KnightEndgamePunishment = -10.0;
static constexpr double BishopEndgamePunishment = 10.0;

static constexpr double RookOnSemiOpenFileBonus = 20.0;
static constexpr double RookOnOpenFileBonus = 30.0;

static constexpr double Blockin_d_or_ePawnPunishment = -40.0; //Punishment for blocking unmoved pawns on d and e files

static constexpr double KnightPawnBonus = 2.0; //Knights better with lots of pawns
static constexpr double BishopPawnPunishment = -2.0; //Bishops worse with lots of pawns
static constexpr double RookPawnPunishment = -2.0; //Rooks worse with lots of pawns

static constexpr double ControlledSquareBonusFactor = 0.3;
static constexpr double CenterAttackedBonusFactor = 1.0; //Factor to multiply with how many center squares are attacked by own pieces
static constexpr double KingSquaresAttackBonusFactor = 5.0; //Factor to multiply with how many squares around enemy king that are attacked by own pieces

static constexpr double SamePieceTwicePunishment = -50.0; //Penaly for moving same piece twice in opening

static double SqDistanceBetweenPieces(const Piece& a, const Piece& b)
{
	const int ax = a.m_Square % 8;
	const int ay = a.m_Square / 8;
	const int bx = a.m_Square % 8;
	const int by = a.m_Square / 8;
	return ((ax - bx)  * (ax - bx) + (ay - by) * (ay - by));
}

double PositionEvaluation::EvaluatePosition(Position& position, bool isTerminalNode)
{
	Position positionCopy = position;

	//Check checkmate/stalemate
	MoveMaker::CheckGameOver(positionCopy, isTerminalNode);
	switch (positionCopy.GetGameStatus())
	{
	case Position::GameStatus::StaleMate:
		return 0.0;
	case Position::GameStatus::CheckMate:
	{
		double score = position.IsWhiteToPlay() ? -Mate : Mate;
		//add correction so M1 > M2 etc
		score += (position.IsWhiteToPlay() ? 1.0 : -1.0) * static_cast<int>(position.GetMoves().size());
		return score;
	}
	default:
		break;
	}

	//Check material
	double score = CountMaterial(position, true) - CountMaterial(position, false);

	//Bishop pair bonus
	if (position.GetWhiteBishops().CountSetBits() >= 2)
		score += BishopPairBonus;
	if (position.GetBlackBishops().CountSetBits() >= 2)
		score -= BishopPairBonus;

	//Rook on open files
	std::pair<int, int> whiteRooksOnOpenFiles = PositionEvaluation::CountRooksOnOpenFiles(position, true);
	std::pair<int, int> blackRooksOnOpenFiles = PositionEvaluation::CountRooksOnOpenFiles(position, false);
	score += whiteRooksOnOpenFiles.first * RookOnOpenFileBonus;
	score += whiteRooksOnOpenFiles.second * RookOnSemiOpenFileBonus;
	score -= blackRooksOnOpenFiles.first * RookOnOpenFileBonus;
	score -= blackRooksOnOpenFiles.second * RookOnSemiOpenFileBonus;

	//penalty for moving same pieces twice
	if (position.GetMoves().size() > 3)
	{
		const int lastIdx = static_cast<int>(position.GetMoves().size()) - 1;
		if (position.GetMoves()[lastIdx].m_From.m_Square == position.GetMoves()[lastIdx - 2].m_To.m_Square)
			score += (position.IsWhiteToPlay() ? SamePieceTwicePunishment : -SamePieceTwicePunishment);
	}

	//Center pawns
	score += CountCenterPawns(position, true) * CenterPawnBonus;
	score -= CountCenterPawns(position, false) * CenterPawnBonus;

	//Check double pawns
	int whiteDoubledPawns = CountDoubledPawns(position, true);
	int blackDoubledPawns = CountDoubledPawns(position, false);

	score += static_cast<double>(whiteDoubledPawns) * DoubledPawnPunishment;
	score -= static_cast<double>(blackDoubledPawns) * DoubledPawnPunishment;

	//Castling bonus: castling improves score during opening, importance of castling decays during the game
	const double castleBonus = CastlingBonus * std::max(0.0, 40.0 - static_cast<double>(position.GetMoves().size()));
	if (position.HasWhiteCastled())
		score += castleBonus;
	if (position.HasBlackCastled())
		score -= castleBonus;

	Bitboard whiteControlledSquares = GetControlledSquares(position, true);
	Bitboard blackControlledSquares = GetControlledSquares(position, false);
	score += whiteControlledSquares.CountSetBits() * ControlledSquareBonusFactor;
	score -= blackControlledSquares.CountSetBits() * ControlledSquareBonusFactor;

	Bitboard attackedSquaresAroundWhiteKing = GetAttackedSquaresAroundKing(position, blackControlledSquares, true);
	Bitboard attackedSquaresAroundBlackKing = GetAttackedSquaresAroundKing(position, whiteControlledSquares, false);
	score += attackedSquaresAroundBlackKing.CountSetBits() * KingSquaresAttackBonusFactor;
	score -= attackedSquaresAroundWhiteKing.CountSetBits() * KingSquaresAttackBonusFactor;

	//////////////////////////

	//Check space behind pawns
	//Check king in check?
	//Check concentration of pieces around enemy king for an attack/mate?
	//const Piece* whiteKing = position.GetPiece(PieceType::King, true);
	//const Piece* blackKing = position.GetPiece(PieceType::King, false);
	//if (!whiteKing || !blackKing)
	//{
	//	assert(false);
	//	return score;
	//}

	/*double squareDistanceToWhiteKing = 0.0;
	double squareDistanceToBlackKing = 1.0;
	for (const Piece& piece : whitePieces)
	{
		if (piece.m_Type != PieceType::King)
		{
			squareDistanceToBlackKing += SqDistanceBetweenPieces(piece, *blackKing);
		}
	}
	for (const Piece& piece : blackPieces)
	{
		if (piece.m_Type != PieceType::King)
		{
			squareDistanceToWhiteKing += SqDistanceBetweenPieces(piece, *whiteKing);
		}
	}*/

	//score += squareDistanceToWhiteKing * 0.003;
	//score -= squareDistanceToBlackKing * 0.003;

	return score;
}

double PositionEvaluation::CountMaterial(const Position& position, bool isWhite)
{
	double material = 0.0;
	material += GetPieceValue(PieceType::Pawn) * (isWhite ? position.GetWhitePawns().CountSetBits() : position.GetBlackPawns().CountSetBits());
	material += GetPieceValue(PieceType::Knight) * (isWhite ? position.GetWhiteKnights().CountSetBits() : position.GetBlackKnights().CountSetBits());
	material += GetPieceValue(PieceType::Bishop) * (isWhite ? position.GetWhiteBishops().CountSetBits() : position.GetBlackBishops().CountSetBits());
	material += GetPieceValue(PieceType::Rook) * (isWhite ? position.GetWhiteRooks().CountSetBits() : position.GetBlackRooks().CountSetBits());
	material += GetPieceValue(PieceType::Queen) * (isWhite ? position.GetWhiteQueens().CountSetBits() : position.GetBlackQueens().CountSetBits());
	return material;
}

constexpr double PositionEvaluation::GetPieceValue(PieceType type)
{
	double value = 0.0;
	switch (type)
	{
	case PieceType::Queen:
		value = 900.0;
		break;
	case PieceType::Rook:
		value = 490.0;
		break;
	case PieceType::Bishop:
		value = 320.0;
		break;
	case PieceType::Knight:
		value = 290.0;
		break;
	case PieceType::Pawn:
		value = 100.0;
		break;
	default:
		break;
	}

	return value;
}

int PositionEvaluation::CountDoubledPawns(const Position& position, bool isWhite)
{
	int count = 0;
	const Bitboard& pawns = isWhite ? position.GetWhitePawns() : position.GetBlackPawns();
	for (int i = 0; i < 8; i++)
	{
		Bitboard file = pawns & _files[i];
		int countOnFile = file.CountSetBits();
		if (countOnFile > 1)
			count += countOnFile;
	}

	return count;
}

int PositionEvaluation::CountCenterPawns(const Position& position, bool isWhite)
{
	static const Bitboard whiteCenter = _d4 | _e4;
	static const Bitboard blackCenter = _d5 | _e5;
	Bitboard centerPawns = (isWhite ? position.GetWhitePawns() : position.GetBlackPawns()) &
		(isWhite ? whiteCenter : blackCenter);

	return centerPawns.CountSetBits();
}

Bitboard PositionEvaluation::GetControlledSquares(Position& position, bool isWhite)
{
	//SHOULD BE UPDATED TO replace pawn moves vs pawn takes
	return MoveSearcher::GetPseudoLegalSquaresFromBitboards(position, isWhite);
}

Bitboard PositionEvaluation::GetAttackedSquaresAroundKing(const Position& position, const Bitboard& attackedSquares, bool isWhite)
{
	const Bitboard& kingSquare = (isWhite ? position.GetWhiteKing() : position.GetBlackKing());
	const int kingSquareIdx = kingSquare.GetSquare();
	return (MoveSearcher::GetMoveTable(PieceType::King)[kingSquareIdx] | kingSquare) & (attackedSquares);
}

std::pair<int, int> PositionEvaluation::CountRooksOnOpenFiles(const Position& position, bool isWhite)
{
	std::pair<int, int> result = { 0,0 };
	const Bitboard& rooks = (isWhite ? position.GetWhiteRooks() : position.GetBlackRooks());
	Bitboard allPawns = position.GetWhitePawns() | position.GetBlackPawns();
	const Bitboard& sameColorPawns = (isWhite ? position.GetWhitePawns() : position.GetBlackPawns());

	//loop over set bits
	uint64_t bitset = rooks;
	while (bitset != 0)
	{
		const uint64_t t = bitset & (~bitset + 1);
		const int idx = static_cast<int>(_tzcnt_u64(bitset));

		const int file = idx & 7;// or % 8
		if (!(_files[file] & allPawns))
			result.first++;
		else if (!(_files[file] & sameColorPawns))
			result.second++;

		bitset ^= t;
	}

	return result;
}