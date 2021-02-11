#include "TestsUtility.h"
#include "MoveMakerTests.h"

void MoveMakerTests::Run()
{
	MoveMakerTests moveMakerTester;
	moveMakerTester.RunPrivate();
}

void MoveMakerTests::RunPrivate()
{
	//MVV-LVA tests
	Position position("4k3/8/8/6n1/8/5N2/8/4K3 w - - 0 1");
	ASSERT(MoveMakerTests::MovesSorter(position, 0, Move(PieceType::Knight, f3, g5), Move(PieceType::Knight, f3, d4)));
	ASSERT(!MoveMakerTests::MovesSorter(position, 0, Move(PieceType::Knight, f3, d4), Move(PieceType::Knight, f3, g5)));

	position = Position("4k3/8/8/6n1/3q4/5N2/8/4K3 w - - 0 1");
	ASSERT(MoveMakerTests::MovesSorter(position, 0, Move(PieceType::Knight, f3, d4), Move(PieceType::Knight, f3, g5)));
	ASSERT(!MoveMakerTests::MovesSorter(position, 0, Move(PieceType::Knight, f3, g5), Move(PieceType::Knight, f3, d4)));

	position = Position("4k3/8/8/6n1/3q4/2P2N2/8/4K3 w - - 0 1");
	ASSERT(MoveMakerTests::MovesSorter(position, 0, Move(PieceType::Pawn, c3, d4), Move(PieceType::Knight, f3, d4)));
	ASSERT(!MoveMakerTests::MovesSorter(position, 0, Move(PieceType::Knight, f3, d4), Move(PieceType::Pawn, c3, d4)));

	position = Position("4k3/8/8/6n1/1r1n4/2P2N2/8/4K3 w - - 0 1");
	ASSERT(MoveMakerTests::MovesSorter(position, 0, Move(PieceType::Pawn, c3, b4), Move(PieceType::Pawn, c3, d4)));
	ASSERT(!MoveMakerTests::MovesSorter(position, 0, Move(PieceType::Pawn, c3, d4), Move(PieceType::Pawn, c3, b4)));

	position = Position("4k3/8/8/6n1/1p6/2P2N2/8/4K3 w - - 0 1");
	ASSERT(MoveMakerTests::MovesSorter(position, 0, Move(PieceType::Pawn, c3, b4), Move(PieceType::Pawn, c3, c4)));
	ASSERT(!MoveMakerTests::MovesSorter(position, 0, Move(PieceType::Pawn, c3, c4), Move(PieceType::Pawn, c3, b4)));

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
	MoveList<MaxMoves> moves{ move5, move3, move1, move8, move6, move2, move4, move7 };
	MoveMakerTests moveMaker;
	moveMaker.SortMoves(position, 0, moves);
	ASSERT(moves[0] == move1);
	ASSERT(moves[1] == move7);
	ASSERT((moves[2] == move3) || (moves[2] == move4));
	//With transposition table lookup
	moveMaker.m_TranspositionTable[moveMaker.GetTranspositionTableKey(position)].m_ZobristHash = position.GetZobristHash();
	moveMaker.m_TranspositionTable[moveMaker.GetTranspositionTableKey(position)].m_BestMove = move8;
	moveMaker.SortMoves(position, 0, moves);
	ASSERT(moves[0] == move8);
	ASSERT(moves[1] == move1);
	ASSERT(moves[2] == move7);
	ASSERT((moves[3] == move3) || (moves[3] == move4));

	//Draw by repetition
	position = Position("5k2/Q7/5K2/3N4/8/2n5/8/8 w - - 0 1");
	Move move(PieceType::King, f6, e6);
	bool success = moveMaker.MakeMove(position, move);
	move = Move(PieceType::King, f8, e8);
	success = moveMaker.MakeMove(position, move);
	move = Move(PieceType::King, e6, f6);
	success = moveMaker.MakeMove(position, move);
	move = Move(PieceType::King, e8, f8);
	success = moveMaker.MakeMove(position, move);
	move = Move(PieceType::King, f6, e6);
	success = moveMaker.MakeMove(position, move);
	move = Move(PieceType::King, f8, e8);
	success = moveMaker.MakeMove(position, move);
	move = Move(PieceType::King, e6, f6);
	success = moveMaker.MakeMove(position, move);
	double score = 0.0;
	success = moveMaker.MakeMove(position, 2, score); //only one move draws and avoids mate in 1
	ASSERT(position.GetMoves().back().GetTo().m_Square == f8);
	ASSERT(abs(score - 0.0) < 0.0000001);

	position = Position("3n2K1/6q1/6k1/8/8/8/8/8 w - - 0 1"); //This is a checkmate
	position.CommitToHistory(); //sets count to 2
	move = Move(PieceType::King, g8, a8); //illegal move, but just to get out of the checkmate
	position.Update(move);
	move = Move(PieceType::Queen, g7, f6);
	success = moveMaker.MakeMove(position, move);
	move = Move(PieceType::King, a8, g8);
	position.Update(move);
	success = moveMaker.MakeMove(position, 3, score); //should avoid Qg7# as it "draws"
	ASSERT(position.GetMoves().back().GetTo().m_Square != g7);

	const Position position2("5Q2/7k/8/8/8/8/8/K5R1 w - - 0 1");
	position = Position("7k/5P2/8/8/8/8/8/K5R1 w - - 0 1");
	position.CommitToHistory(position2.GetZobristHash()); //commit twice
	position.CommitToHistory(position2.GetZobristHash());
	success = moveMaker.MakeMove(position, 3, score); //should avoid f8=Q as it "draws"
	ASSERT(position.GetMoves().back().GetTo().m_Square != f8);

	//Same with an available transposition table entry whose best move should be discarded
	position = Position("7k/5P2/8/8/8/8/8/K5R1 w - - 0 1");
	position.CommitToHistory(position.GetZobristHash()); //The correct test would ommit this additional commit...
	move = Move(PieceType::Pawn, PieceType::Queen, f7, f8);
	position.CommitToHistory(position2.GetZobristHash());
	position.CommitToHistory(position2.GetZobristHash());
	const size_t transpositionTableKey = moveMaker.GetTranspositionTableKey(position);
	moveMaker.m_TranspositionTable[transpositionTableKey].m_ZobristHash = position.GetZobristHash();
	moveMaker.m_TranspositionTable[transpositionTableKey].m_Score = 10000;
	moveMaker.m_TranspositionTable[transpositionTableKey].m_Depth = 3;
	moveMaker.m_TranspositionTable[transpositionTableKey].m_BestMove = Move(PieceType::Pawn, PieceType::Queen, f7, f8);
	moveMaker.m_TranspositionTable[transpositionTableKey].m_Flag = TranspositionTableEntry::Flag::Exact;
	success = moveMaker.MakeMove(position, 3, score);
	ASSERT(position.GetMoves().back().GetTo().m_Square != f8);
}