#include "pch.h"
#include "PositionEvaluation.h"
#include "MoveMaker.h"
#include "MoveSearcher.h"
#include "BitboardUtility.h"
#include <assert.h>

/// <summary>Absolute score for a mate</summary>
static constexpr int BishopPairBonus = 15;
static constexpr int CastlingBonus = 50;

static constexpr int CenterPawnBonus = 40;
static constexpr int DoubledPawnPunishment = -20; //40 for a pair
static constexpr int IsolatedPawnPunishment = -40;
static constexpr int BackwardsPawnPunishment = -20;
static constexpr int PassedPawnBonus = 40;
static constexpr std::array<int, 3> AdvancedPawnBonus = {30, 40, 50}; //on rows 5, 6, 7 or 4, 3, 2
static constexpr int SquareBehindPawnBonus = 1;

static constexpr int KnightEndgamePunishment = -10;
static constexpr int BishopEndgamePunishment = 10;

static constexpr int RookOnSemiOpenFileBonus = 20;
static constexpr int RookOnOpenFileBonus = 30;

static constexpr int Blocking_d_or_ePawnPunishment = -40; //Punishment for blocking unmoved pawns on d and e files

static constexpr int KnightPawnBonus = 2; //Knights better with lots of pawns
static constexpr int BishopPawnPunishment = -2; //Bishops worse with lots of pawns
static constexpr int RookPawnPunishment = -2; //Rooks worse with lots of pawns

static constexpr int AttackedSquareBonusFactor = 0;
static constexpr int CenterAttackedBonusFactor = 1; //Factor to multiply with how many center squares are attacked by own pieces
static constexpr int KingSquaresAttackBonusFactor = 5; //Factor to multiply with how many squares around enemy king that are attacked by own pieces

static constexpr int SamePieceTwicePunishment = -50; //Penaly for moving same piece twice in opening
static constexpr int TempoBonus = 30;

int PositionEvaluation::EvaluatePosition(Position& position, int ply)
{
	//Check checkmate/stalemate
	switch (position.GetGameStatus())
	{
		case Position::GameStatus::Draw:
			return 0;
		case Position::GameStatus::CheckMate:
		{
			int score = position.IsWhiteToPlay() ? -Mate : Mate;
			//add correction so M1 > M2 etc
			score += (position.IsWhiteToPlay() ? 1 : -1) * ply;
			return score;
		}
		default:
			break;
	}

	//Tempo bonus
	int score = (position.IsWhiteToPlay() ? 1 : -1) * TempoBonus;

	//Check material
	score += CountMaterial(position, true) - CountMaterial(position, false);

	//Check development
	if (position.GetMoves().size() < 25)
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
	score += CountDoubledPawns(position, true) * DoubledPawnPunishment;
	score -= CountDoubledPawns(position, false) * DoubledPawnPunishment;

	//Isolated pawn punishment
	score += CountIsolatedPawns(position, true) * IsolatedPawnPunishment;
	score -= CountIsolatedPawns(position, false) * IsolatedPawnPunishment;

	//Backwards pawn punishment
	score += CountBackwardsPawns(position, true) * BackwardsPawnPunishment;
	score -= CountBackwardsPawns(position, false) * BackwardsPawnPunishment;

	//Passed pawn bonus
	score += CountPassedPawns(position, true) * PassedPawnBonus;
	score -= CountPassedPawns(position, false) * PassedPawnBonus;

	//Advanced pawns bonus
	score += GetAdvancedPawnsBonus(position, true);
	score -= GetAdvancedPawnsBonus(position, false);

	//Piece blocking d or e pawn punishment
	score += CountBlockedEorDPawns(position, true) * Blocking_d_or_ePawnPunishment;
	score -= CountBlockedEorDPawns(position, false) * Blocking_d_or_ePawnPunishment;

	//Space
	score += GetSpaceBehindPawns(position, true) * SquareBehindPawnBonus;
	score -= GetSpaceBehindPawns(position, false) * SquareBehindPawnBonus;

	//Castling bonus: castling improves score during opening, importance of castling decays during the game
	const int castleBonus = CastlingBonus * std::max(0, 40 - static_cast<int>(position.GetMoves().size())) / 40;
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
	return false;
}

std::optional<int> PositionEvaluation::GetMovesToMate(int score)
{
	std::optional<int> count;
	const double diff = (Mate - abs(score));
	if (diff < MaxPly)
		count = static_cast<int>(ceil(diff / 2));
	return count;
}

int PositionEvaluation::CountMaterial(const Position& position, bool isWhite)
{
	int material = 0;
	material += GetPieceValue(PieceType::Pawn) * (isWhite ? position.GetWhitePawns().CountSetBits() : position.GetBlackPawns().CountSetBits());
	material += GetPieceValue(PieceType::Knight) * (isWhite ? position.GetWhiteKnights().CountSetBits() : position.GetBlackKnights().CountSetBits());
	material += GetPieceValue(PieceType::Bishop) * (isWhite ? position.GetWhiteBishops().CountSetBits() : position.GetBlackBishops().CountSetBits());
	material += GetPieceValue(PieceType::Rook) * (isWhite ? position.GetWhiteRooks().CountSetBits() : position.GetBlackRooks().CountSetBits());
	material += GetPieceValue(PieceType::Queen) * (isWhite ? position.GetWhiteQueens().CountSetBits() : position.GetBlackQueens().CountSetBits());
	return material;
}

constexpr int PositionEvaluation::GetPieceValue(PieceType type)
{
	int value = 0;
	switch (type)
	{
	case PieceType::Queen:
		value = 900;
		break;
	case PieceType::Rook:
		value = 490;
		break;
	case PieceType::Bishop:
		value = 320;
		break;
	case PieceType::Knight:
		value = 290;
		break;
	case PieceType::Pawn:
		value = 100;
		break;
	default:
		break;
	}

	return value;
}

int PositionEvaluation::GetUndevelopedPiecesPunishment(const Position& position, bool isWhite)
{
	const Bitboard undevelopedKnights = (isWhite ? (position.GetWhiteKnights() & (_b1 | _g1)) : (position.GetBlackKnights() & (_b8 | _g8)));
	const Bitboard undevelopedBishops = (isWhite ? (position.GetWhiteBishops() & (_c1 | _f1)) : (position.GetBlackBishops() & (_c8 | _f8)));
	const Bitboard undevelopedRooks = (isWhite ? (position.GetWhiteRooks() & (_a1 | _h1)) : (position.GetBlackRooks() & (_a8 | _h8)));
	const Bitboard undevelopedQueen = (isWhite ? (position.GetWhiteQueens() & _d1) : (position.GetBlackQueens() & _d8));
	return -(undevelopedKnights.CountSetBits() * 10 + undevelopedBishops.CountSetBits() * 10 + undevelopedRooks.CountSetBits() * 5 + undevelopedQueen.CountSetBits() * 5);
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

int PositionEvaluation::CountBackwardsPawns(const Position& position, bool isWhite)
{
	int count = 0;
	const Bitboard& pawns = isWhite ? position.GetWhitePawns() : position.GetBlackPawns();
	const Bitboard& enemyPawns = isWhite ? position.GetBlackPawns() : position.GetWhitePawns();
	for (int i = 0; i <= 7; i++)
	{
		const Bitboard pawnsOnSemiOpenFile = pawns & (((enemyPawns & _files[i]) > 0) ? Bitboard() : _files[i]);
		//check absence of friend pawns behind and on the sides
		Bitboard sideFiles;
		if (i > 0)
			sideFiles |= _files[i - 1];
		if (i < 7)
			sideFiles |= _files[i + 1];

		//loop over set bits
		uint64_t bitset = pawnsOnSemiOpenFile;
		while (bitset != 0)
		{
			const uint64_t t = bitset & (~bitset + 1);
			const int idx = static_cast<int>(_tzcnt_u64(bitset));
			const int row = idx >> 3;// or / 8

			const Bitboard rowsBehind = (isWhite ? RowsUnder[row] : RowsAbove[row]); //includes pawns on same rank
			const Bitboard pawnsBehind = (pawns & sideFiles & rowsBehind);
			if (!pawnsBehind)
				count++;

			bitset ^= t;
		}
	}

	return count;
}

int PositionEvaluation::CountPassedPawns(const Position& position, bool isWhite)
{
	int count = 0;
	const Bitboard& pawns = isWhite ? position.GetWhitePawns() : position.GetBlackPawns();
	const Bitboard& enemyPawns = isWhite ? position.GetBlackPawns() : position.GetWhitePawns();
	for (int i = 0; i <= 7; i++)
	{
		//check absence of enemy pawns in front and on the sides
		const Bitboard pawnsOnFile = (pawns & _files[i]);
		Bitboard sideBySideFiles = _files[i];
		if (i > 0)
			sideBySideFiles |= _files[i - 1];
		if (i < 7)
			sideBySideFiles |= _files[i + 1];
		
		//loop over set bits
		uint64_t bitset = pawnsOnFile;
		while (bitset != 0)
		{
			const uint64_t t = bitset & (~bitset + 1);
			const int idx = static_cast<int>(_tzcnt_u64(bitset));
			const int row = idx >> 3;// or / 8

			assert((row + 1 < 8) && (row - 1 >= 0));
			const Bitboard rowsInFront = (isWhite ? RowsAbove[row + 1] : RowsUnder[row - 1]);
			const Bitboard blockingPawns = (sideBySideFiles & rowsInFront & enemyPawns); //doesn't include pawns on same rank
			if (!blockingPawns)
				count++;

			bitset ^= t;
		}
	}

	return count;
}

int PositionEvaluation::GetAdvancedPawnsBonus(const Position& position, bool isWhite)
{
	int bonus = 0;
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

int PositionEvaluation::GetSpaceBehindPawns(const Position& position, bool isWhite)
{
	int count = 0;
	const Bitboard& allPawns = position.GetWhitePawns() | position.GetBlackPawns();
	for (int i = 0; i <= 7; i++)
	{
		const Bitboard pawnsOnFile = (allPawns & _files[i]);

		//loop over set bits
		uint64_t bitset = pawnsOnFile;
		int lastPawnOnFile = (isWhite ? 7 : 0);
		while (bitset != 0)
		{
			const uint64_t t = bitset & (~bitset + 1);
			const int idx = static_cast<int>(_tzcnt_u64(bitset));
			const int row = idx >> 3;// or / 8

			lastPawnOnFile = (isWhite ? std::min(lastPawnOnFile, row) : std::max(lastPawnOnFile, row));

			bitset ^= t;
		}

		if (lastPawnOnFile > 0 && lastPawnOnFile < 7)
			count += (isWhite ? lastPawnOnFile : (7 - lastPawnOnFile));
	}

	return count;
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
	std::pair<int, int> result = { 0, 0 };
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