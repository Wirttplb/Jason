#include "TestsUtility.h"
#include "MoveMakerTests.h"

void MoveMakerTests::Run()
{
	//MVV-LVA tests
	Position position("4k3/8/8/6n1/8/5N2/8/4K3 w - - 0 1");
	ASSERT(MoveMakerTests::MovesSorter(position, Move(PieceType::Knight, f3, g5), Move(PieceType::Knight, f3, d4)));
	ASSERT(!MoveMakerTests::MovesSorter(position, Move(PieceType::Knight, f3, d4), Move(PieceType::Knight, f3, g5)));

	position = Position("4k3/8/8/6n1/3q4/5N2/8/4K3 w - - 0 1");
	ASSERT(MoveMakerTests::MovesSorter(position, Move(PieceType::Knight, f3, d4), Move(PieceType::Knight, f3, g5)));
	ASSERT(!MoveMakerTests::MovesSorter(position, Move(PieceType::Knight, f3, g5), Move(PieceType::Knight, f3, d4)));

	position = Position("4k3/8/8/6n1/3q4/2P2N2/8/4K3 w - - 0 1");
	ASSERT(MoveMakerTests::MovesSorter(position, Move(PieceType::Pawn, c3, d4), Move(PieceType::Knight, f3, d4)));
	ASSERT(!MoveMakerTests::MovesSorter(position, Move(PieceType::Knight, f3, d4), Move(PieceType::Pawn, c3, d4)));

	position = Position("4k3/8/8/6n1/1r1n4/2P2N2/8/4K3 w - - 0 1");
	ASSERT(MoveMakerTests::MovesSorter(position, Move(PieceType::Pawn, c3, b4), Move(PieceType::Pawn, c3, d4)));
	ASSERT(!MoveMakerTests::MovesSorter(position, Move(PieceType::Pawn, c3, d4), Move(PieceType::Pawn, c3, b4)));

	position = Position("4k3/8/8/6n1/1p6/2P2N2/8/4K3 w - - 0 1");
	ASSERT(MoveMakerTests::MovesSorter(position, Move(PieceType::Pawn, c3, b4), Move(PieceType::Pawn, c3, c4)));
	ASSERT(!MoveMakerTests::MovesSorter(position, Move(PieceType::Pawn, c3, c4), Move(PieceType::Pawn, c3, b4)));

	//SortMoves
	position = Position("4k3/2Q5/5P2/6n1/1r1n4/2P2N2/8/4K3 w - - 0 1");
	Move move1(PieceType::Pawn, c3, b4);
	Move move2(PieceType::Pawn, c3, c4);
	Move move3(PieceType::Knight, f3, d4);
	Move move4(PieceType::Knight, f3, g5);
	Move move5(PieceType::King, e1, d1);
	Move move6(PieceType::King, e1, d2);
	Move move7(PieceType::Pawn, c3, d4);
	Move move8(PieceType::Queen, c7, e7);
	std::vector<Move> moves = { move5, move3, move1, move8, move6, move2, move4, move7 };
	MoveMakerTests moveMaker;
	moveMaker.SortMoves(position, moves);
	ASSERT(moves[0] == move1);
	ASSERT(moves[1] == move7);
	ASSERT((moves[2] == move3) || (moves[2] == move4));
	//With transposition table lookup
	moveMaker.m_TranspositionTable[moveMaker.GetTranspositionTableKey(position)].m_ZobristHash = position.GetZobristHash();
	moveMaker.m_TranspositionTable[moveMaker.GetTranspositionTableKey(position)].m_BestMove = move8;
	moveMaker.SortMoves(position, moves);
	ASSERT(moves[0] == move8);
	ASSERT(moves[1] == move1);
	ASSERT(moves[2] == move7);
	ASSERT((moves[3] == move3) || (moves[3] == move4));
}