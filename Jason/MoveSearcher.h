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
	static std::vector<std::array<int, 2>> GetAccessibleSquares(const Position& position, const Position::Piece& piece, bool isWhitePiece);


	/// <returns>All legal moves for ONE piece, a move being the positions before and after of a piece (and type because of queening)</returns>
	static std::vector<Position::Move> GetLegalMoves(const Position& position, const Position::Piece& piece, bool isWhitePiece);

	static bool IsKingInCheck(const Position& position, bool isWhiteKing);

private:
	/// <summary>
	/// Returns true if move is blocked because of collision
	/// </summary>
	static bool IsMoveBlocked(const Position::Piece& blockingPiece, const Position::Piece& piece, const std::array<int, 2>& square);
	
	/// <summary>
	/// Returns true if move is illegal because of check
	/// <param name=="position">original position (before move)</param>
	/// <param name=="piece">piece to move and its original position</param>
	/// <param name=="isWhitePiece">true if piece to move is white</param>
	/// <param name=="square">target square</param>
	/// </summary>
	static bool IsMoveIllegal(const Position& position, const Position::Piece& piece, bool isWhitePiece, const std::array<int, 2>& square);
};