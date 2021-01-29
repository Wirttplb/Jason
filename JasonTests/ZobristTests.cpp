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
	move.m_From = Piece(PieceType::Pawn, e2);
	move.m_To = Piece(PieceType::Pawn, e4);
	newPosition.Update(move);
	move.m_From = Piece(PieceType::Pawn, e7);
	move.m_To = Piece(PieceType::Pawn, e5);
	newPosition.Update(move);
	move.m_From = Piece(PieceType::Pawn, d2);
	move.m_To = Piece(PieceType::Pawn, d4);
	newPosition.Update(move);
	move.m_From = Piece(PieceType::Pawn, e5);
	move.m_To = Piece(PieceType::Pawn, d4);
	newPosition.Update(move);
	ASSERT(newPosition.GetZobristHash() == newPosition.ComputeZobristHash());

	//test a random position reached from random moves
	std::srand(0);
	newPosition = startingPosition;
	int count = 0;
	for (int i = 0; i < 400; i++)//217 half-moves is end of game
	{
		std::optional<Move> move = MoveSearcher::GetRandomMove(newPosition);
		if (!move.has_value())
			break;

		newPosition.Update(*move);
		count++;

		ASSERT(newPosition.GetZobristHash() == newPosition.ComputeZobristHash());
	}
	ASSERT(count == 217 && MoveSearcher::IsKingInCheck(newPosition, false));

	//undo all moves
	std::vector<Move> moves = newPosition.GetMoves();
	for (std::vector<Move>::reverse_iterator rit = moves.rbegin(); rit != moves.rend(); ++rit)
	{
		newPosition.Undo(*rit);

		ASSERT(newPosition.GetZobristHash() == newPosition.ComputeZobristHash());
	}
	ASSERT(startingPosition.GetZobristHash() == newPosition.GetZobristHash());

	//random game #2
	std::srand(1);
	newPosition = startingPosition;
	count = 0;
	for (int i = 0; i < 400; i++)
	{
		std::optional<Move> move = MoveSearcher::GetRandomMove(newPosition);
		if (!move.has_value())
			break;

		newPosition.Update(*move);
		count++;

		ASSERT(newPosition.GetZobristHash() == newPosition.ComputeZobristHash());

		if (newPosition.IsInsufficientMaterial())
			break;
	}
	ASSERT(count == 244 && !MoveSearcher::IsKingInCheck(newPosition, false) && !MoveSearcher::IsKingInCheck(newPosition, true));

	//undo all moves
	moves = newPosition.GetMoves();
	for (std::vector<Move>::reverse_iterator rit = moves.rbegin(); rit != moves.rend(); ++rit)
	{
		newPosition.Undo(*rit);

		ASSERT(newPosition.GetZobristHash() == newPosition.ComputeZobristHash());
	}
	ASSERT(startingPosition.GetZobristHash() == newPosition.GetZobristHash());

	//random game #3
	std::srand(2);
	newPosition = startingPosition;
	count = 0;
	for (int i = 0; i < 1000; i++)
	{
		std::optional<Move> move = MoveSearcher::GetRandomMove(newPosition);
		if (!move.has_value())
			break;

		newPosition.Update(*move);
		count++;

		ASSERT(newPosition.GetZobristHash() == newPosition.ComputeZobristHash());

		if (newPosition.IsInsufficientMaterial())
			break;
	}
	ASSERT(count == 514 && !MoveSearcher::IsKingInCheck(newPosition, false) && !MoveSearcher::IsKingInCheck(newPosition, true));

	//undo all moves
	moves = newPosition.GetMoves();
	for (std::vector<Move>::reverse_iterator rit = moves.rbegin(); rit != moves.rend(); ++rit)
	{
		newPosition.Undo(*rit);

		ASSERT(newPosition.GetZobristHash() == newPosition.ComputeZobristHash());
	}
	ASSERT(startingPosition.GetZobristHash() == newPosition.GetZobristHash());

	//two paths reach same position = same hash
	newPosition = startingPosition;
	move.m_From = Piece(PieceType::Pawn, e2);
	move.m_To = Piece(PieceType::Pawn, e3);
	newPosition.Update(move);
	move.m_From = Piece(PieceType::Pawn, e7);
	move.m_To = Piece(PieceType::Pawn, e5);
	newPosition.Update(move);
	move.m_From = Piece(PieceType::Pawn, d2);
	move.m_To = Piece(PieceType::Pawn, d3);
	newPosition.Update(move);
	uint64_t hash1 = newPosition.GetZobristHash();
	ASSERT(hash1 == newPosition.ComputeZobristHash());
	newPosition = startingPosition;
	move.m_From = Piece(PieceType::Pawn, d2);
	move.m_To = Piece(PieceType::Pawn, d3);
	newPosition.Update(move);
	move.m_From = Piece(PieceType::Pawn, e7);
	move.m_To = Piece(PieceType::Pawn, e5);
	newPosition.Update(move);
	move.m_From = Piece(PieceType::Pawn, e2);
	move.m_To = Piece(PieceType::Pawn, e3);
	newPosition.Update(move);
	uint64_t hash2 = newPosition.GetZobristHash();
	ASSERT(hash2 == newPosition.ComputeZobristHash());
	ASSERT(hash1 == hash2);

	//two paths, same position BUT different en passant squares
	newPosition = startingPosition;
	move.m_From = Piece(PieceType::Pawn, e2);
	move.m_To = Piece(PieceType::Pawn, e4);
	newPosition.Update(move);
	move.m_From = Piece(PieceType::Pawn, e7);
	move.m_To = Piece(PieceType::Pawn, e5);
	newPosition.Update(move);
	move.m_From = Piece(PieceType::Pawn, d2);
	move.m_To = Piece(PieceType::Pawn, d4);
	newPosition.Update(move);
	hash1 = newPosition.GetZobristHash();
	ASSERT(hash1 == newPosition.ComputeZobristHash());
	newPosition = startingPosition;
	move.m_From = Piece(PieceType::Pawn, d2);
	move.m_To = Piece(PieceType::Pawn, d4);
	newPosition.Update(move);
	move.m_From = Piece(PieceType::Pawn, e7);
	move.m_To = Piece(PieceType::Pawn, e5);
	newPosition.Update(move);
	move.m_From = Piece(PieceType::Pawn, e2);
	move.m_To = Piece(PieceType::Pawn, e4); 
	newPosition.Update(move);
	hash2 = newPosition.GetZobristHash();
	ASSERT(hash2 == newPosition.ComputeZobristHash());
	ASSERT(hash1 != hash2);

	//two paths with capture, same position
	newPosition = startingPosition;
	move.m_From = Piece(PieceType::Knight, g1);
	move.m_To = Piece(PieceType::Knight, h3);
	newPosition.Update(move);
	move.m_From = Piece(PieceType::Pawn, g7);
	move.m_To = Piece(PieceType::Pawn, g5);
	newPosition.Update(move);
	move.m_From = Piece(PieceType::Knight, h3);
	move.m_To = Piece(PieceType::Knight, g5);
	newPosition.Update(move);
	hash1 = newPosition.GetZobristHash();
	ASSERT(hash1 == newPosition.ComputeZobristHash());
	newPosition = startingPosition;
	move.m_From = Piece(PieceType::Knight, g1);
	move.m_To = Piece(PieceType::Knight, f3);
	newPosition.Update(move);
	move.m_From = Piece(PieceType::Pawn, g7);
	move.m_To = Piece(PieceType::Pawn, g5);
	newPosition.Update(move);
	move.m_From = Piece(PieceType::Knight, f3);
	move.m_To = Piece(PieceType::Knight, g5);
	newPosition.Update(move);
	hash2 = newPosition.GetZobristHash();
	ASSERT(hash2 == newPosition.ComputeZobristHash());
	ASSERT(hash1 == hash2);

	//to do: test Undo move
}