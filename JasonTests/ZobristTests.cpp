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

	//two paths reach same position = same hash
	newPosition = startingPosition;
	move.m_From = Piece(PieceType::Pawn, 4, 1); //e2
	move.m_To = Piece(PieceType::Pawn, 4, 2); //e3
	newPosition.UpdatePosition(move);
	move.m_From = Piece(PieceType::Pawn, 4, 6); //e7
	move.m_To = Piece(PieceType::Pawn, 4, 4); //e5
	newPosition.UpdatePosition(move);
	move.m_From = Piece(PieceType::Pawn, 3, 1); //d2
	move.m_To = Piece(PieceType::Pawn, 3, 2); //d3
	newPosition.UpdatePosition(move);
	uint64_t hash1 = newPosition.GetZobristHash();
	ASSERT(hash1 == newPosition.ComputeZobristHash());
	newPosition = startingPosition;
	move.m_From = Piece(PieceType::Pawn, 3, 1); //d2
	move.m_To = Piece(PieceType::Pawn, 3, 2); //d3
	newPosition.UpdatePosition(move);
	move.m_From = Piece(PieceType::Pawn, 4, 6); //e7
	move.m_To = Piece(PieceType::Pawn, 4, 4); //e5
	newPosition.UpdatePosition(move);
	move.m_From = Piece(PieceType::Pawn, 4, 1); //e2
	move.m_To = Piece(PieceType::Pawn, 4, 2); //e3
	newPosition.UpdatePosition(move);
	uint64_t hash2 = newPosition.GetZobristHash();
	ASSERT(hash2 == newPosition.ComputeZobristHash());
	ASSERT(hash1 == hash2);

	//two paths, same position BUT different en passant squares
	newPosition = startingPosition;
	move.m_From = Piece(PieceType::Pawn, 4, 1); //e4
	move.m_To = Piece(PieceType::Pawn, 4, 3); //e4
	newPosition.UpdatePosition(move);
	move.m_From = Piece(PieceType::Pawn, 4, 6); //e7
	move.m_To = Piece(PieceType::Pawn, 4, 4); //e5
	newPosition.UpdatePosition(move);
	move.m_From = Piece(PieceType::Pawn, 3, 1); //d2
	move.m_To = Piece(PieceType::Pawn, 3, 3); //d4
	newPosition.UpdatePosition(move);
	hash1 = newPosition.GetZobristHash();
	ASSERT(hash1 == newPosition.ComputeZobristHash());
	newPosition = startingPosition;
	move.m_From = Piece(PieceType::Pawn, 3, 1); //d2
	move.m_To = Piece(PieceType::Pawn, 3, 3); //d4
	newPosition.UpdatePosition(move);
	move.m_From = Piece(PieceType::Pawn, 4, 6); //e7
	move.m_To = Piece(PieceType::Pawn, 4, 4); //e5
	newPosition.UpdatePosition(move);
	move.m_From = Piece(PieceType::Pawn, 4, 1); //e4
	move.m_To = Piece(PieceType::Pawn, 4, 3); //e4
	newPosition.UpdatePosition(move);
	hash2 = newPosition.GetZobristHash();
	ASSERT(hash2 == newPosition.ComputeZobristHash());
	ASSERT(hash1 != hash2);

	//two paths with capture, same position
	newPosition = startingPosition;
	move.m_From = Piece(PieceType::Knight, 6, 0);
	move.m_To = Piece(PieceType::Knight, 7, 2); //Nh3
	newPosition.UpdatePosition(move);
	move.m_From = Piece(PieceType::Pawn, 6, 6); //g7
	move.m_To = Piece(PieceType::Pawn, 6, 4); //g5
	newPosition.UpdatePosition(move);
	move.m_From = Piece(PieceType::Knight, 7, 2);
	move.m_To = Piece(PieceType::Knight, 6, 4);
	newPosition.UpdatePosition(move);
	hash1 = newPosition.GetZobristHash();
	ASSERT(hash1 == newPosition.ComputeZobristHash());
	newPosition = startingPosition;
	move.m_From = Piece(PieceType::Knight, 6, 0);
	move.m_To = Piece(PieceType::Knight, 5, 2); //Nf3
	newPosition.UpdatePosition(move);
	move.m_From = Piece(PieceType::Pawn, 6, 6); //g7
	move.m_To = Piece(PieceType::Pawn, 6, 4); //g5
	newPosition.UpdatePosition(move);
	move.m_From = Piece(PieceType::Knight, 5, 2);
	move.m_To = Piece(PieceType::Knight, 6, 4);
	newPosition.UpdatePosition(move);
	hash2 = newPosition.GetZobristHash();
	ASSERT(hash2 == newPosition.ComputeZobristHash());
	ASSERT(hash1 == hash2);

	//to do: test Undo move
}