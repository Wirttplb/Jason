#pragma once
#include "Position.h"
#include <unordered_set>

class MoveSearcher
{
public:
	/// <returns>All legal moves for every piece for a given position, a move being the positions before and after of a piece (and type because of queening)</returns>
	static void GetLegalMovesFromBitboards(Position& position, MoveList<MaxMoves>& allLegalMoves);

	/// <summary>Returns legal moves for ONE piece, a move being the positions before and after of a piece (and type because of queening)</summary>
	/// <param name="append">true to append and keep</param>
	/// <param name="legalMoves">move list where new moves will be appended</param>
	static void GetLegalMovesFromBitboards(Position& position, PieceType type, Square square, bool isWhitePiece, MoveList<MaxMoves>& legalMoves);

	/// <summary>Returns Bitboard of accessible pseudo-legal squares, used for controlled squares enumeration</summary>
	/// <param name="pawnAttackSquares">if true will return only pawn attack squares without checking enemy presence ("controlled squares")</param>
	static Bitboard GetPseudoLegalSquaresFromBitboards(Position& position, bool isWhite, bool pawnControlledSquares);
	
	/// <returns>All pseudo legal moves to-squares for pieces of type whose position is described by bitboard</returns>
	/// <param name="pawnAttackSquares">if true will return only pawn attack squares without checking enemy presence ("controlled squares")</param>
	static Bitboard GetPseudoLegalBitboardMoves(const Position& position, PieceType type, const Bitboard& bitboard, bool isWhitePiece, bool pawnControlledSquares);
	
	/// <summary>Get all pseudo legal moves for pieces of type whose position is described by bitboard</summary>
	/// <param name="legalMoves">mves list where new pseudo legal moves will be appended</param>
	static void GetPseudoLegalMovesFromBitboards(const Position& position, PieceType type, const Bitboard& bitboard, bool isWhitePiece, MoveList<MaxMoves>& legalMoves);

	/// <summary>Returns number of nodes at given depth</summary>
	/// <param name = "moveLists">move lists (1 for each depth), should be statically allocated</param>
	static size_t Perft(Position& position, int depth, std::array<MoveList<MaxMoves>, PerftMaxDepth>& moveLists);

	/// <summary>Returns set of unique zobrist keys at given depth (unique positions)</summary>
	/// <param name = "moveLists">move lists (1 for each depth), should be statically allocated</param>
	static std::unordered_set<uint64_t> UniquePerft(Position& position, int depth, std::array<MoveList<MaxMoves>, PerftMaxDepth>& moveLists);

	static bool IsKingInCheckFromBitboards(const Position& position, bool isWhiteKing);

	/// <summary>Returns a random legal move from a given position (null if stalemate or checkmate)</summary>
	static std::optional<Move> GetRandomMove(const Position& position);
	static std::optional<Move> GetRandomMoveFromBitboards(Position& position);

	/// <returns>Move table (accessible squares on empty board) ; for pawns, single step moves</returns>
	static const std::vector<Bitboard>& GetMoveTable(PieceType type, bool isWhite = true);

private:
	/// <returns>All legal moves for every piece for a given position, a move being the positions before and after of a piece (and type because of queening)</returns>
	/// <remark>Very slow, not used at runtime, only for table generation</remark>
	static std::vector<Move> GetLegalMoves(const Position& position);
	/// <returns>All legal moves for ONE piece, a move being the positions before and after of a piece (and type because of queening)</returns>
	/// <remark>Very slow, not used at runtime</remark>
	static std::vector<Move> GetLegalMoves(const Position& position, const Piece& piece, bool isWhitePiece);

	static bool IsKingInCheck(const Position& position, bool isWhiteKing);

	/// <summary>
	/// Get all accessible squares for a piece (where they can move if there was no collision)
	/// </summary>
	/// <param name="piece">piece to move</param>
	/// <param name="isWhitePiece">true for a white piece</param>
	/// <returns>list of squares</returns>
	/// <remark>Very slow, only for table generation</remark>
	static std::vector<std::array<int, 2>> GetAccessibleSquares(const Position& position, const Piece& piece, bool isWhitePiece);
	static Bitboard GetAccessibleBitboard(const Position& position, const Piece& piece, bool isWhitePiece, bool isPawnDoubleStep);

	/// <summary>Returns pawn capture squares (according to enemy pieces position)</summary>
	/// <remark>public for testing</remark>
	static std::vector<std::array<int, 2>> GetPawnCaptureSquares(const Position& position, const Piece& piece, bool isWhitePiece);


	/// <summary>
	/// Returns true if move is blocked because of collision
	/// </summary>
	static bool IsMoveBlocked(const Piece& blockingPiece, const Piece& piece, const std::array<int, 2>& square);
	
	/// <summary>
	/// Returns true if move is illegal because of check
	/// <param name=="position">original position (before move), updated with move and undone</param>
	/// <param name=="move">move to check</param>
	/// <param name=="isWhitePiece">true if piece to move is white</param>
	/// </summary>
	static bool IsMoveIllegal(const Position& position, const Move& move, bool isWhitePiece);
	static bool IsMoveIllegalFromBitboards(Position& position, Move& move, bool isWhitePiece);

	friend static std::vector<Bitboard> GenerateMoves(PieceType type, bool isWhitePiece, bool isPawnDoubleStep);
	friend static std::vector<Bitboard> GeneratePawnCaptureMoves(bool isWhitePiece);
	friend static std::array<std::array<Bitboard, 256>, 8> GenerateRookAttacks();
};