#pragma once
#include "Position.h"
#include "MoveMaker.h"
#include <unordered_set>

class MoveSearcher
{
public:
	/// <returns>All legal moves for every piece for a given position, a move being the positions before and after of a piece (and type because of queening)</returns>
	/// <remark>Not bitboards version is very slow, not used at runtime</remark>
	static std::vector<Move> GetLegalMoves(const Position& position);
	static std::vector<Move> GetLegalMovesFromBitboards(Position& position);
	/// <summary>Returns Bitboard of accessible pseudo-legal squares, used for controlled squares enumeration</summary>
	static Bitboard GetPseudoLegalSquaresFromBitboards(Position& position, bool isWhite);
	static std::vector<Move> GetPseudoLegalMovesFromBitboards(Position& position);

	/// <returns>All legal moves for ONE piece, a move being the positions before and after of a piece (and type because of queening)</returns>
	/// <remark>Very slow, not used at runtime</remark>
	static std::vector<Move> GetLegalMoves(const Position& position, const Piece& piece, bool isWhitePiece);

	/// <returns>All legal moves for ONE piece, a move being the positions before and after of a piece (and type because of queening)</returns>
	static std::vector<Move> GetLegalMovesFromBitboards(Position& position, const Piece& piece, bool isWhitePiece);

	/// <returns>All legal moves for pieces of type whose position is described by bitboard</returns>
	static Bitboard GetPseudoLegalBitboardMoves(const Position& position, PieceType type, const Bitboard& bitboard, bool isWhitePiece);
	static std::vector<Move> GetPseudoLegalMovesFromBitboards(const Position& position, PieceType type, const Bitboard& bitboard, bool isWhitePiece);

	/// <summary>Returns number of nodes at given depth</summary>
	static size_t Perft(Position& position, int depth);
	/// <summary>Returns set of unique zobrist keys at given depth (unique positions)</summary>
	static std::unordered_set<uint64_t> UniquePerft(Position& position, int depth);

	static bool IsKingInCheck(const Position& position, bool isWhiteKing);
	static bool IsKingInCheckFromBitboards(const Position& position, bool isWhiteKing);

	/// <summary>
	/// Return a random legal move from a given position (null if stalemate or checkmate)
	/// </summary>
	static std::optional<Move> GetRandomMove(const Position& position);

	/// <returns>Move table (accessible squares on empty board) ; for pawns, single step moves</returns>
	static const std::vector<Bitboard>& GetMoveTable(PieceType type, bool isWhite = true);

private:
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
};