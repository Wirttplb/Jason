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
static constexpr std::array<double, 3> AdvancedPawnBonus = {30.0, 50.0, 60.0}; //on rows 5, 6, 7 or 4, 3, 2

static constexpr double KnightEndgamePunishment = -10.0;
static constexpr double BishopEndgamePunishment = 10.0;

static constexpr double RookOnSemiOpenFileBonus = 20.0;
static constexpr double RookOnOpenFileBonus = 30.0;

static constexpr double Blocking_d_or_ePawnPunishment = -40.0; //Punishment for blocking unmoved pawns on d and e files

static constexpr double KnightPawnBonus = 2.0; //Knights better with lots of pawns
static constexpr double BishopPawnPunishment = -2.0; //Bishops worse with lots of pawns
static constexpr double RookPawnPunishment = -2.0; //Rooks worse with lots of pawns

static constexpr double AttackedSquareBonusFactor = 0.3;
static constexpr double CenterAttackedBonusFactor = 1.0; //Factor to multiply with how many center squares are attacked by own pieces
static constexpr double KingSquaresAttackBonusFactor = 5.0; //Factor to multiply with how many squares around enemy king that are attacked by own pieces

static constexpr double SamePieceTwicePunishment = -50.0; //Penaly for moving same piece twice in opening

double PositionEvaluation::EvaluatePosition(Position& position)
{
	//Check checkmate/stalemate
	switch (position.GetGameStatus())
	{
		case Position::GameStatus::Draw:
			return 0.0;
		case Position::GameStatus::CheckMate:
		{
			double score = position.IsWhiteToPlay() ? -Mate : Mate;
			//add correction so M1 > M2 etc
			score += (position.IsWhiteToPlay() ? 1.0 : -1.0) * static_cast<int>(position.GetNumberOfMoves());
			return score;
		}
		default:
			break;
	}

	//Check material
	double score = CountMaterial(position, true) - CountMaterial(position, false);

	//Check development
	if (position.GetNumberOfMoves() < 25)
	{
		score += GetUndevelopedPiecesPunishment(position, true);
		score -= GetUndevelopedPiecesPunishment(position, false);
	}

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

	//Penalty for moving same pieces twice
	//Removal of this makes tacticsTest fails (tactic2200)... should investigate!
	if (position.GetNumberOfMoves() > 3)
	{
		const int lastIdx = static_cast<int>(position.GetNumberOfMoves()) - 1;
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
	const double castleBonus = CastlingBonus * std::max(0.0, 40.0 - static_cast<double>(position.GetNumberOfMoves())) * 0.025; //0.025 = 1/40
	if (position.HasWhiteCastled())
		score += castleBonus;
	if (position.HasBlackCastled())
		score -= castleBonus;

	Bitboard whiteAttackedSquares = GetAttackedSquares(position, true);
	Bitboard blackAttackedSquares = GetAttackedSquares(position, false);
	score += whiteAttackedSquares.CountSetBits() * AttackedSquareBonusFactor;
	score -= blackAttackedSquares.CountSetBits() * AttackedSquareBonusFactor;

	Bitboard attackedSquaresAroundWhiteKing = GetAttackedSquaresAroundKing(position, blackAttackedSquares, true);
	Bitboard attackedSquaresAroundBlackKing = GetAttackedSquaresAroundKing(position, whiteAttackedSquares, false);
	score += attackedSquaresAroundBlackKing.CountSetBits() * KingSquaresAttackBonusFactor;
	score -= attackedSquaresAroundWhiteKing.CountSetBits() * KingSquaresAttackBonusFactor;

	//////////////////////////
	//Check space behind pawns

	return score;
}

bool PositionEvaluation::IsPositionQuiet(const Position& position)
{
	//THIS IS COMPLETELY WRONG! SHOULD CHECK UNDEFENDED PIECES ETC...
	//return (!position.GetMoves().back().IsCapture() &&
	//	((position.GetMoves().size() < 2) || !(position.GetMoves().end() - 2)->IsCapture()) &&
	//	(position.GetMoves().size() < 3) || !(position.GetMoves().end() - 3)->IsCapture());// &&
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

double PositionEvaluation::GetUndevelopedPiecesPunishment(const Position& position, bool isWhite)
{
	const Bitboard undevelopedKnights = (isWhite ? (position.GetWhiteKnights() & (_b1 | _g1)) : (position.GetBlackKnights() & (_b8 | _g8)));
	const Bitboard undevelopedBishops = (isWhite ? (position.GetWhiteBishops() & (_c1 | _f1)) : (position.GetBlackBishops() & (_c8 | _f8)));
	const Bitboard undevelopedRooks = (isWhite ? (position.GetWhiteRooks() & (_a1 | _h1)) : (position.GetBlackRooks() & (_a8 | _h8)));
	const Bitboard undevelopedQueen = (isWhite ? (position.GetWhiteQueens() & _d1) : (position.GetBlackQueens() & _d8));
	return -(undevelopedKnights.CountSetBits() * 10.0 + undevelopedBishops.CountSetBits() * 10.0 + undevelopedRooks.CountSetBits() * 5.0 + undevelopedQueen.CountSetBits() * 5.0);
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
	const Bitboard centerPawns = (isWhite ? position.GetWhitePawns() : position.GetBlackPawns()) &
		(isWhite ? whiteCenter : blackCenter);

	return centerPawns.CountSetBits();
}

int PositionEvaluation::CountIsolatedPawns(const Position& position, bool isWhite)
{
	int count = 0;
	const Bitboard& pawns = isWhite ? position.GetWhitePawns() : position.GetBlackPawns();
	for (int i = 0; i <= 7; i++)
	{
		Bitboard sideBySideFiles = _files[i]; //up to 3 files
		if (i > 0)
			sideBySideFiles |= _files[i - 1];
		if (i < 7)
			sideBySideFiles |= _files[i + 1];

		const Bitboard pawnsOnFile = pawns & _files[i];
		const Bitboard pawnsOnSideBySideFiles = pawns & sideBySideFiles;
		const int countOnFile = pawnsOnFile.CountSetBits();
		if (countOnFile == pawnsOnSideBySideFiles.CountSetBits())
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
		const int r = (isWhite ? i : 7 - i);
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
	const Bitboard& centerPawns = (isWhite ? WhiteCenterPawns : BlackCenterPawns);
	const Bitboard& blockingSquares = (isWhite ? WhiteBlockingSquares : BlackBlockingSquares);
	if (isWhite)
		return (((blockingSquares & pieces) >> 8) & (centerPawns & pawns)).CountSetBits();
	else
		return (((blockingSquares & pieces) << 8) & (centerPawns & pawns)).CountSetBits();
}

Bitboard PositionEvaluation::GetAttackedSquares(Position& position, bool isWhite)
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