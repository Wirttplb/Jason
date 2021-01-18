#include "ZobristTests.h"
#include "TestsUtility.h"
#include "Position.h"
#include "MoveSearcher.h"

void ZobristTests::Run()
{
	Position startingPosition; //zobrist should be init
	ASSERT(startingPosition.GetZobristHash() == startingPosition.ComputeZobristHash());

	Position newPosition = startingPosition;
	Move move;
	move.m_From = Piece(PieceType::Pawn, 4, 1); //e2
	move.m_To = Piece(PieceType::Pawn, 4, 3); //e4
	newPosition.UpdatePosition(move);
	move.m_From = Piece(PieceType::Pawn, 4, 6); //e7
	move.m_To = Piece(PieceType::Pawn, 4, 4); //e5
	newPosition.UpdatePosition(move);
	move.m_From = Piece(PieceType::Pawn, 3, 1); //d2
	move.m_To = Piece(PieceType::Pawn, 3, 3); //d4
	newPosition.UpdatePosition(move);
	move.m_From = Piece(PieceType::Pawn, 4, 4); //exd
	move.m_To = Piece(PieceType::Pawn, 3, 3);
	newPosition.UpdatePosition(move);
	ASSERT(newPosition.GetZobristHash() == newPosition.ComputeZobristHash());

	//test a random position reached from random moves
	std::srand(0);
	newPosition = startingPosition;
	int count = 0;
	for (int i = 0; i < 400; i++)//217 half-moves is end of game 400
	{
		std::optional<Move> move = MoveSearcher::GetRandomMove(newPosition);
		if (!move.has_value())
			break;

		newPosition.UpdatePosition(*move);
		count++;

		ASSERT(newPosition.GetZobristHash() == newPosition.ComputeZobristHash());
	}
	ASSERT(count == 217 && MoveSearcher::IsKingInCheck(newPosition, false));

	//random game #2
	std::srand(1);
	newPosition = startingPosition;
	count = 0;
	for (int i = 0; i < 400; i++)
	{
		std::optional<Move> move = MoveSearcher::GetRandomMove(newPosition);
		if (!move.has_value())
			break;

		newPosition.UpdatePosition(*move);
		count++;

		ASSERT(newPosition.GetZobristHash() == newPosition.ComputeZobristHash());

		if (newPosition.IsInsufficientMaterial())
			break;
	}
	ASSERT(count == 244 && !MoveSearcher::IsKingInCheck(newPosition, false) && !MoveSearcher::IsKingInCheck(newPosition, true));

	//random game #3
	std::srand(2);
	newPosition = startingPosition;
	count = 0;
	for (int i = 0; i < 1000; i++)
	{
		std::optional<Move> move = MoveSearcher::GetRandomMove(newPosition);
		if (!move.has_value())
			break;

		newPosition.UpdatePosition(*move);
		count++;

		ASSERT(newPosition.GetZobristHash() == newPosition.ComputeZobristHash());

		if (newPosition.IsInsufficientMaterial())
			break;
	}
	ASSERT(count == 514 && !MoveSearcher::IsKingInCheck(newPosition, false) && !MoveSearcher::IsKingInCheck(newPosition, true));

	//starting position depth 3
	std::unordered_set<Position> uniquePositions = MoveSearcher::GetAllUniquePositions(startingPosition, 3);
	//ASSERT(uniquePositions.size() == 5362);

	//to do: test Undo move
}