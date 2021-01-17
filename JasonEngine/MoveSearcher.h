#pragma once
#include "Position.h"
#include "MoveMaker.h"

class MoveSearcher
{
public:
	/// <summary>
	/// Get all accessible squares for a piece (where they can move if there was no collision)
	/// </summary>
	/// <param name="piece">piece to move</param>
	/// <param name="isWhitePiece">true for a white piece</param>
	/// <param name="enPassantSquare">square of enemy pawn that has just moved two squares</param>
	/// <returns>list of squares</returns>
	static std::vector<std::array<int, 2>> GetAccessibleSquares(const Position& position, const Piece& piece, bool isWhitePiece);


	/// <returns>All legal moves for ONE piece, a move being the positions before and after of a piece (and type because of queening)</returns>
	static std::vector<Move> GetLegalMoves(const Position& position, const Piece& piece, bool isWhitePiece);

	/// <summary>
	/// Compute all possible legal positions from one position
	/// </summary>
	static std::vector<Position> GetAllPossiblePositions(const Position& position);

	/// <summary>
	/// Compute all possible legal positions from one position, up to given depth (0 = only input position) (high memory/cpu usage!)
	/// </summary>
	/// <remark>Returned positions are not unique</remark>
	static std::vector<Position> GetAllPossiblePositions(const Position& position, int depth);

	/// <summary>
	/// Compute positions from one position, reducing search domain to capture/tactical moves
	/// </summary>
	static std::vector<Position> GetAllLinesPositions(const Position& position, int depth);

	static bool IsKingInCheck(const Position& position, bool isWhiteKing);

private:
	/// <summary>
	/// Returns true if move is blocked because of collision
	/// </summary>
	static bool IsMoveBlocked(const Piece& blockingPiece, const Piece& piece, const std::array<int, 2>& square);
	
	/// <summary>
	/// Returns true if move is illegal because of check
	/// <param name=="position">original position (before move)</param>
	/// <param name=="piece">piece to move and its original position</param>
	/// <param name=="isWhitePiece">true if piece to move is white</param>
	/// <param name=="square">target square</param>
	/// </summary>
	static bool IsMoveIllegal(const Position& position, const Piece& piece, bool isWhitePiece, const std::array<int, 2>& square);
};