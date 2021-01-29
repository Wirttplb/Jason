#pragma once
#include "Position.h"
#include "MoveMaker.h"
#include <unordered_set>

class MoveSearcher
{
public:
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

	/// <returns>All legal moves for every piece for a given position, a move being the positions before and after of a piece (and type because of queening)</returns>
	static std::vector<Move> GetLegalMoves(const Position& position);
	static std::vector<Move> GetLegalMovesFromBitboards(const Position& position);
	/// <summary>Returns Bitboard of accessible pseudo-legal squares, used for controlled squares enumeration</summary>
	static Bitboard GetPseudoLegalSquaresFromBitboards(const Position& position, bool isWhite);

	/// <returns>All legal moves for ONE piece, a move being the positions before and after of a piece (and type because of queening)</returns>
	static std::vector<Move> GetLegalMoves(const Position& position, const Piece& piece, bool isWhitePiece);

	/// <returns>All legal moves for ONE piece, a move being the positions before and after of a piece (and type because of queening)</returns>
	static std::vector<Move> GetLegalMovesFromBitboards(const Position& position, const Piece& piece, bool isWhitePiece);

	/// <returns>All legal moves for pieces of type whose position is described by bitboard</returns>
	static Bitboard GetPseudoLegalBitboardMoves(const Position& position, PieceType type, const Bitboard& bitboard, bool isWhitePiece);
	static std::vector<Move> GetPseudoLegalMoves(const Position& position, PieceType type, const Bitboard& bitboard, bool isWhitePiece);

	/// <summary>
	/// Compute moves from one position, reducing search domain to captures/tactical moves
	/// </summary>
	static std::vector<Move> GetAllLineMoves(const Position& position);

	/// <summary>
	/// Compute all possible positions from one position (lots of copy, SLOW!)
	/// </summary>
	static std::vector<Position> GetAllPossiblePositions(const Position& position);

	/// <summary>
	/// Compute all possible positions from one position, up to given depth (lots of copy, SLOW!)
	/// </summary>
	static std::vector<Position> GetAllPossiblePositions(const Position& position, int depth);
	/// <summary> Much faster, does not make copies of position</summary>
	static size_t CountNodes(Position& position, int depth);

	/// <summary>
	/// Compute all unique positions from one position, up to given depth (lots of copy, SLOW!)
	/// </summary>
	static std::unordered_set<Position> GetAllUniquePositions(const Position& position, int depth);

	static bool IsKingInCheck(const Position& position, bool isWhiteKing);
	static bool IsKingInCheckFromBitboards(const Position& position, bool isWhiteKing);

	/// <summary>
	/// Return a random legal move from a given position (null if stalemate or checkmate)
	/// </summary>
	static std::optional<Move> GetRandomMove(const Position& position);

private:
	/// <summary>
	/// Returns true if move is blocked because of collision
	/// </summary>
	static bool IsMoveBlocked(const Piece& blockingPiece, const Piece& piece, const std::array<int, 2>& square);
	
	/// <summary>
	/// Returns true if move is illegal because of check
	/// <param name=="position">original position (before move)</param>
	/// <param name=="move">move to check</param>
	/// <param name=="isWhitePiece">true if piece to move is white</param>
	/// </summary>
	static bool IsMoveIllegal(const Position& position, const Move& move, bool isWhitePiece);
	static bool IsMoveIllegalFromBitboards(const Position& position, const Move& move, bool isWhitePiece);
};