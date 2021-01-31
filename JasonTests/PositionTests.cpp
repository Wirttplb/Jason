#include "TestsUtility.h"
#include "PositionTests.h"
#include "Position.h"

void PositionTests::Run()
{
	static const Position startingPosition;

	//Testing move undo
	Position position = startingPosition;
	position.SetMaintainPiecesList(true);
	ASSERT(position.GetZobristHash() == position.ComputeZobristHash());
	Move move(PieceType::Pawn, e2, e4);
	position.Update(move);
	ASSERT(position.GetZobristHash() == position.ComputeZobristHash());
	move = Move(PieceType::Knight, g8, f6);
	position.Update(move);
	ASSERT(position.GetZobristHash() == position.ComputeZobristHash());
	move = Move(PieceType::Pawn, e4, e5);
	position.Update(move);
	ASSERT(position.GetZobristHash() == position.ComputeZobristHash());
	move = Move(PieceType::Pawn, d7, d5);
	position.Update(move);
	ASSERT(position.GetZobristHash() == position.ComputeZobristHash());
	move = Move(PieceType::Pawn, e5, f6);
	position.Update(move);
	ASSERT(position.GetZobristHash() == position.ComputeZobristHash());

	std::vector<Move> moves = position.GetMoves();
	for (std::vector<Move>::reverse_iterator rit = moves.rbegin(); rit != moves.rend(); ++rit)
	{
		position.Undo(*rit);

		ASSERT(position.GetZobristHash() == position.ComputeZobristHash());
	}
	ASSERT(position.GetZobristHash() == position.GetZobristHash());
	ASSERT(position == startingPosition);
	ASSERT(position.AreEqual(startingPosition));
}