#include "ZobristTests.h"
#include "TestsUtility.h"
#include "Position.h"
#include "MoveSearcher.h"

void ZobristTests::Run()
{
	Position startingPosition; //zobrist should be init
	startingPosition.SetMaintainPiecesList(true);
	ASSERT(startingPosition.GetZobristHash() == startingPosition.ComputeZobristHash());

	Position newPosition = startingPosition;
	Move move;
	move.SetFrom(PieceType::Pawn, e2);
	move.SetTo(PieceType::Pawn, e4);
	newPosition.Update(move);
	move.SetFrom(PieceType::Pawn, e7);
	move.SetTo(PieceType::Pawn, e5);
	newPosition.Update(move);
	move.SetFrom(PieceType::Pawn, d2);
	move.SetTo(PieceType::Pawn, d4);
	newPosition.Update(move);
	move.SetFrom(PieceType::Pawn, e5);
	move.SetTo(PieceType::Pawn, d4);
	newPosition.Update(move);
	ASSERT(newPosition.GetZobristHash() == newPosition.ComputeZobristHash());

	//test a random position reached from random moves
	std::srand(0);
	newPosition = startingPosition;
	int count = 0;
	for (int i = 0; i < MaxPly; i++)
	{
		std::optional<Move> move = MoveSearcher::GetRandomMove(newPosition);
		if (!move.has_value())
			break;

		newPosition.Update(*move);
		count++;

		ASSERT(newPosition.GetZobristHash() == newPosition.ComputeZobristHash());
	}

	//undo all moves
	MoveList moves = newPosition.GetMoves();
	for (std::array<Move, MaxPly>::const_reverse_iterator rit = moves.rbegin(); rit != moves.rend(); ++rit)
	{
		newPosition.Undo(*rit);

		ASSERT(newPosition.GetZobristHash() == newPosition.ComputeZobristHash());
	}
	ASSERT(startingPosition.GetZobristHash() == newPosition.GetZobristHash());

	//random game #2
	std::srand(1);
	newPosition = startingPosition;
	count = 0;
	for (int i = 0; i < MaxPly; i++)
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

	//undo all moves
	moves = newPosition.GetMoves();
	for (std::array<Move, MaxPly>::const_reverse_iterator rit = moves.rbegin(); rit != moves.rend(); ++rit)
	{
		newPosition.Undo(*rit);

		ASSERT(newPosition.GetZobristHash() == newPosition.ComputeZobristHash());
	}
	ASSERT(startingPosition.GetZobristHash() == newPosition.GetZobristHash());

	//random game #3
	std::srand(2);
	newPosition = startingPosition;
	count = 0;
	for (int i = 0; i < MaxPly; i++)
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

	//undo all moves
	moves = newPosition.GetMoves();
	for (std::array<Move, MaxPly>::const_reverse_iterator rit = moves.rbegin(); rit != moves.rend(); ++rit)
	{
		newPosition.Undo(*rit);

		ASSERT(newPosition.GetZobristHash() == newPosition.ComputeZobristHash());
	}
	ASSERT(startingPosition.GetZobristHash() == newPosition.GetZobristHash());

	//two paths reach same position = same hash
	newPosition = startingPosition;
	move.SetFrom(PieceType::Pawn, e2);
	move.SetTo(PieceType::Pawn, e3);
	newPosition.Update(move);
	move.SetFrom(PieceType::Pawn, e7);
	move.SetTo(PieceType::Pawn, e5);
	newPosition.Update(move);
	move.SetFrom(PieceType::Pawn, d2);
	move.SetTo(PieceType::Pawn, d3);
	newPosition.Update(move);
	uint64_t hash1 = newPosition.GetZobristHash();
	ASSERT(hash1 == newPosition.ComputeZobristHash());
	newPosition = startingPosition;
	move.SetFrom(PieceType::Pawn, d2);
	move.SetTo(PieceType::Pawn, d3);
	newPosition.Update(move);
	move.SetFrom(PieceType::Pawn, e7);
	move.SetTo(PieceType::Pawn, e5);
	newPosition.Update(move);
	move.SetFrom(PieceType::Pawn, e2);
	move.SetTo(PieceType::Pawn, e3);
	newPosition.Update(move);
	uint64_t hash2 = newPosition.GetZobristHash();
	ASSERT(hash2 == newPosition.ComputeZobristHash());
	ASSERT(hash1 == hash2);

	//two paths, same position BUT different en passant squares
	newPosition = startingPosition;
	move.SetFrom(PieceType::Pawn, e2);
	move.SetTo(PieceType::Pawn, e4);
	newPosition.Update(move);
	move.SetFrom(PieceType::Pawn, e7);
	move.SetTo(PieceType::Pawn, e5);
	newPosition.Update(move);
	move.SetFrom(PieceType::Pawn, d2);
	move.SetTo(PieceType::Pawn, d4);
	newPosition.Update(move);
	hash1 = newPosition.GetZobristHash();
	ASSERT(hash1 == newPosition.ComputeZobristHash());
	newPosition = startingPosition;
	move.SetFrom(PieceType::Pawn, d2);
	move.SetTo(PieceType::Pawn, d4);
	newPosition.Update(move);
	move.SetFrom(PieceType::Pawn, e7);
	move.SetTo(PieceType::Pawn, e5);
	newPosition.Update(move);
	move.SetFrom(PieceType::Pawn, e2);
	move.SetTo(PieceType::Pawn, e4); 
	newPosition.Update(move);
	hash2 = newPosition.GetZobristHash();
	ASSERT(hash2 == newPosition.ComputeZobristHash());
	ASSERT(hash1 != hash2);

	//two paths with capture, same position
	newPosition = startingPosition;
	move.SetFrom(PieceType::Knight, g1);
	move.SetTo(PieceType::Knight, h3);
	newPosition.Update(move);
	move.SetFrom(PieceType::Pawn, g7);
	move.SetTo(PieceType::Pawn, g5);
	newPosition.Update(move);
	move.SetFrom(PieceType::Knight, h3);
	move.SetTo(PieceType::Knight, g5);
	newPosition.Update(move);
	hash1 = newPosition.GetZobristHash();
	ASSERT(hash1 == newPosition.ComputeZobristHash());
	newPosition = startingPosition;
	move.SetFrom(PieceType::Knight, g1);
	move.SetTo(PieceType::Knight, f3);
	newPosition.Update(move);
	move.SetFrom(PieceType::Pawn, g7);
	move.SetTo(PieceType::Pawn, g5);
	newPosition.Update(move);
	move.SetFrom(PieceType::Knight, f3);
	move.SetTo(PieceType::Knight, g5);
	newPosition.Update(move);
	hash2 = newPosition.GetZobristHash();
	ASSERT(hash2 == newPosition.ComputeZobristHash());
	ASSERT(hash1 == hash2);

	//castles
	Position castlingPosition("r3k2r/1ppqnpbp/p1bp2p1/4p3/4P3/N1PP1N2/PPQB1PPP/R3K2R w KQkq - 0 1");
	castlingPosition.SetMaintainPiecesList(true);
	Position backupPosition = castlingPosition;
	Move K(PieceType::King, e1, g1);
	Move k(PieceType::King, e8, g8);
	Move Q(PieceType::King, e1, c1);
	Move q(PieceType::King, e8, c8);
	castlingPosition.Update(K);
	castlingPosition.Update(k);
	castlingPosition.Undo(k);
	castlingPosition.Undo(K);
	ASSERT(backupPosition.GetZobristHash() == castlingPosition.GetZobristHash());
	castlingPosition.Update(Q);
	castlingPosition.Update(q);
	castlingPosition.Undo(q);
	castlingPosition.Undo(Q);
	ASSERT(backupPosition.GetZobristHash() == castlingPosition.GetZobristHash());

	//queening
	Position queeningPosition("8/1P2k3/8/8/8/8/1p2K3/8 w - - 0 1");
	queeningPosition.SetMaintainPiecesList(true);
	backupPosition = queeningPosition;
	Move a(PieceType::Pawn, PieceType::Queen, b7, b8);
	Move b(PieceType::Pawn, PieceType::Queen, b2, b1);
	queeningPosition.Update(a);
	queeningPosition.Update(b);
	queeningPosition.Undo(b);
	queeningPosition.Undo(a);
	ASSERT(backupPosition.GetZobristHash() == queeningPosition.GetZobristHash());

	//null move
	newPosition = Position();
	Move move1(PieceType::Pawn, e2, e4);
	newPosition.Update(move1);
	Move move2;
	move2.SetNullMove();
	newPosition.Update(move2);
	std::optional<Move> move3 = MoveSearcher::GetRandomMoveFromBitboards(newPosition);
	newPosition.Update(*move3);
	std::optional<Move> move4 = MoveSearcher::GetRandomMoveFromBitboards(newPosition);
	newPosition.Update(*move4);
	Move move5;
	move5.SetNullMove();
	newPosition.Update(move5);
	std::optional<Move> move6 = MoveSearcher::GetRandomMoveFromBitboards(newPosition);
	newPosition.Update(*move6);
	newPosition.Undo(*move6);
	newPosition.Undo(move5);
	newPosition.Undo(*move4);
	newPosition.Undo(*move3);
	newPosition.Undo(move2);
	newPosition.Undo(move1);
	ASSERT(newPosition.GetZobristHash() == startingPosition.GetZobristHash());
}