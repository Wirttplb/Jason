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

static constexpr double CenterPawnBonus = 40.0;
static constexpr double DoubledPawnPunishment = -20.0; //40 for a pair
static constexpr double IsolatedPawnPunishment = -40.0;
static constexpr std::array<double, 3> AdvancedPawnBonus = {30.0, 50.0, 60.0}; //on rows 5, 6, 7

static constexpr double KnightEndgamePunishment = -10.0;
static constexpr double BishopEndgamePunishment = 10.0;

static constexpr double RookOnSemiOpenFileBonus = 20.0;
static constexpr double RookOnOpenFileBonus = 30.0;

static constexpr double Blocking_d_or_ePawnPunishment = -40.0; //Punishment for blocking unmoved pawns on d and e files

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

double PositionEvaluation::EvaluatePosition(Position& position)
{
	std::vector<std::pair<Piece, Piece>> moves = position.GetPieceMoves();
	if (moves.size() >= 6)
	{
		if (moves[0].second == Piece(PieceType::Pawn, d4) &&
			moves[1].second == Piece(PieceType::Pawn, d5) &&
			moves[2].second == Piece(PieceType::Pawn, c4) &&
			moves[3].second == Piece(PieceType::Pawn, e5) &&
			moves[4].second == Piece(PieceType::Pawn, e5) &&
			moves[5].second == Piece(PieceType::Pawn, c6))
			return 0.0;
	}

	//Check checkmate/stalemate
	switch (position.GetGameStatus())
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

	//Piece type bonus according to number of pawns left
	int allPawnsCount = (position.GetWhitePawns() | position.GetBlackPawns()).CountSetBits();
	score += allPawnsCount * position.GetWhiteKnights().CountSetBits() * KnightPawnBonus;
	score -= allPawnsCount * position.GetBlackKnights().CountSetBits() * KnightPawnBonus;
	score += allPawnsCount * position.GetWhiteBishops().CountSetBits() * BishopPawnPunishment;
	score -= allPawnsCount * position.GetBlackBishops().CountSetBits() * BishopPawnPunishment;
	score += allPawnsCount * position.GetWhiteRooks().CountSetBits() * RookPawnPunishment;
	score -= allPawnsCount * position.GetBlackRooks().CountSetBits() * RookPawnPunishment;

	//Rook on open files
	std::pair<int, int> whiteRooksOnOpenFiles = PositionEvaluation::CountRooksOnOpenFiles(position, true);
	std::pair<int, int> blackRooksOnOpenFiles = PositionEvaluation::CountRooksOnOpenFiles(position, false);
	score += whiteRooksOnOpenFiles.first * RookOnOpenFileBonus;
	score += whiteRooksOnOpenFiles.second * RookOnSemiOpenFileBonus;
	score -= blackRooksOnOpenFiles.first * RookOnOpenFileBonus;
	score -= blackRooksOnOpenFiles.second * RookOnSemiOpenFileBonus;

	//penalty for moving same pieces twice
	//Removal of this makes tacticsTest fails (tactic1800)..., should investigate!
	if (position.GetMoves().size() > 3)
	{
		const int lastIdx = static_cast<int>(position.GetMoves().size()) - 1;
		if (position.GetMoves()[lastIdx].GetFromSquare() == position.GetMoves()[lastIdx - 2].GetToSquare())
			score += (position.IsWhiteToPlay() ? SamePieceTwicePunishment : -SamePieceTwicePunishment);
	}

	//Center pawns bonus
	score += CountCenterPawns(position, true) * CenterPawnBonus;
	score -= CountCenterPawns(position, false) * CenterPawnBonus;

	//Double pawn punishment
	int whiteDoubledPawns = CountDoubledPawns(position, true);
	int blackDoubledPawns = CountDoubledPawns(position, false);
	score += static_cast<double>(whiteDoubledPawns) * DoubledPawnPunishment;
	score -= static_cast<double>(blackDoubledPawns) * DoubledPawnPunishment;

	//Isolated pawn punishment
	int whiteIsolatedPawns = CountIsolatedPawns(position, true);
	int blackIsolatedPawns = CountIsolatedPawns(position, false);
	score += static_cast<double>(whiteDoubledPawns) * IsolatedPawnPunishment;
	score -= static_cast<double>(blackDoubledPawns) * IsolatedPawnPunishment;

	//Advanced pawns bonus
	score += GeAdvancedPawnsBonus(position, true);
	score -= GeAdvancedPawnsBonus(position, true);

	//Piece blocking d or e pawn punishment
	score += CountBlockedEorDPawns(position, true) * Blocking_d_or_ePawnPunishment;
	score -= CountBlockedEorDPawns(position, false) * Blocking_d_or_ePawnPunishment;

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
	//King in check for quiescence check..

	return score;
}

bool PositionEvaluation::IsPositionQuiet(const Position& position)
{
	return (!position.GetMoves().back().IsCapture() &&
		((position.GetMoves().size() < 2) || !(position.GetMoves().end() - 2)->IsCapture()) &&
		(position.GetMoves().size() < 3) || !(position.GetMoves().end() - 3)->IsCapture());// &&
		//!MoveSearcher::IsKingInCheckFromBitboards(position, position.IsWhiteToPlay()));
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

int PositionEvaluation::CountIsolatedPawns(const Position& position, bool isWhite)
{
	int count = 0;
	const Bitboard& pawns = isWhite ? position.GetWhitePawns() : position.GetBlackPawns();
	for (int i = 0; i < 7; i++)
	{
		Bitboard sideBySideFiles = _files[i]; //up to 3 files
		if (i > 0)
			sideBySideFiles |= _files[i - 1];
		if (i < 7)
			sideBySideFiles |= _files[i + 1];

		Bitboard pawnsOnSideBySideFiles = pawns & sideBySideFiles;
		int countOnFile = pawnsOnSideBySideFiles.CountSetBits();
		if (countOnFile == 1)
			count += countOnFile;
	}

	return count;
}

double PositionEvaluation::GeAdvancedPawnsBonus(const Position& position, bool isWhite)
{
	double bonus = 0.0;
	const Bitboard& pawns = isWhite ? position.GetWhitePawns() : position.GetBlackPawns();

	for (int i = 4; i < 7; i++)
	{
		int r = (isWhite ? i : 7 - i);
		bonus += (pawns & _rows[r]).CountSetBits() * (isWhite ? AdvancedPawnBonus[i - 4] : -AdvancedPawnBonus[i - 4]);
	}

	return bonus;
}

static const Bitboard WhiteCenterPawns = (Bitboard(d2) | Bitboard(e2));
static const Bitboard BlackCenterPawns = (Bitboard(d7) | Bitboard(e7));
static const Bitboard WhiteBlockingSquares = (Bitboard(d3) | Bitboard(e3));
static const Bitboard BlackBlockingSquares = (Bitboard(d6) | Bitboard(e6));

int PositionEvaluation::CountBlockedEorDPawns(const Position& position, bool isWhite)
{
	const Bitboard& pawns = isWhite ? position.GetWhitePawns() : position.GetBlackPawns();
	const Bitboard& pieces = isWhite ? position.GetWhitePieces() : position.GetBlackPieces();
	const Bitboard& blockedSquares = (isWhite ? WhiteCenterPawns : BlackCenterPawns);
	const Bitboard& blockingSquares = (isWhite ? WhiteBlockingSquares : BlackBlockingSquares);

	const Bitboard blockedPieces = ((blockingSquares & pieces) >> 8) & (blockedSquares & pawns);
	return blockedPieces.CountSetBits();
}

Bitboard PositionEvaluation::GetControlledSquares(Position& position, bool isWhite)
{
	bool pawnControlledSquares = true;
	return MoveSearcher::GetPseudoLegalSquaresFromBitboards(position, isWhite, pawnControlledSquares);
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