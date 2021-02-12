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
	success = moveMaker.MakeMove(position, 3, score); //only one move draws and avoids mate in 1
	ASSERT(position.GetMoves().back().GetTo().m_Square == f8);
	ASSERT(abs(score - 0.0) < 0.0000001);

	position = Position("8/6Qk/8/8/8/8/8/K5R1 b - - 1 1");
	move = Move(PieceType::King, h7, h8);
	position.Update(move);
	move = Move(PieceType::Queen, g7, f8); //mate, we reverse it for this test
	position.Update(move);
	move = Move(PieceType::King, h8, h7);
	position.Update(move);
	move = Move(PieceType::Queen, f8, g7);
	position.Update(move);
	move = Move(PieceType::King, h7, h8);
	position.Update(move);
	move = Move(PieceType::Queen, g7, f8);
	position.Update(move);
	move = Move(PieceType::King, h8, h7);
	position.Update(move);
	success = moveMaker.MakeMove(position, 4, score); //should avoid f8=Q as it "draws"
	ASSERT(position.GetMoves().back().GetTo().m_Square != f8);

	position = Position("5Q2/7k/8/8/8/8/8/K5R1 w - - 1 1");
	move = Move(PieceType::Queen, f8, f7);
	position.Update(move);
	move = Move(PieceType::King, h7, h8);
	position.Update(move);
	move = Move(PieceType::Queen, f7, f8);
	position.Update(move);
	move = Move(PieceType::King, h8, h7);
	position.Update(move);
	move = Move(PieceType::Queen, PieceType::Pawn, f8, f7);
	position.Update(move);
	move = Move(PieceType::King, h7, h8);
	position.Update(move);
	success = moveMaker.MakeMove(position, 4, score); //will not avoid f8=Q because pawn move is irreversible, can't draw
	ASSERT(position.GetMoves().back().GetTo().m_Square == f8);

	//test undo backup flags
	//position = Position("1K2Qnk1/5ppp/5ppp/2B5/1P6/P7/8/8 w - - 0 1");
	//move = Move(PieceType::Bishop, c5, f8);
	//position.Update(move);
	//move2 = Move(PieceType::King, g8, h8);
	//position.Update(move2);
	//move3 = Move(PieceType::Queen, a4, e8);
	//position.Update(move3);
	//move4 = Move();
	//move4.SetNullMove();
	//position.Update(move4);
	//int PliesFromNull = position.m_PliesFromLastNullMove;
	//int PliesFromIrreversible = position.m_PliesFromLastIrreversibleMove;
	//position.Undo(move4);
	//position.Undo(move3);
	//position.Undo(move2);
	//position.Undo(move);

	position = Position("1K2Qnk1/5ppp/5ppp/2B5/1P6/P7/8/8 w - - 0 1");
	move = Move(PieceType::Queen, e8, a4);
	position.Update(move);
	move = Move(PieceType::Knight, f8, e6);
	position.Update(move);
	Position positionCopy = position;
	move = Move(PieceType::Queen, a4, e8);
	position.Update(move);
	move = Move(PieceType::Knight, e6, f8);
	position.Update(move);
	move = Move(PieceType::Queen, e8, a4);
	position.Update(move);
	move = Move(PieceType::Knight, f8, e6);
	position.Update(move);
	Position positionCopy2 = position;
	Position positionCopy3 = position;
	success = moveMaker.MakeMove(position, 4, score); //will avoid the best move Qe8 (M2) because only valid move for black "draws"
	ASSERT(position.GetMoves().back().GetTo() != Piece(PieceType::Queen, e8));
	moveMaker.m_TranspositionTable = {};
	success = moveMaker.MakeMove(positionCopy, 4, score); //will do the best move Qe8 (M2), no draw
	ASSERT(positionCopy.GetMoves().back().GetTo() == Piece(PieceType::Queen, e8));

	//Same with an available transposition table entry whose best move should be discarded
	moveMaker.m_TranspositionTable = {};
	const size_t transpositionTableKey = moveMaker.GetTranspositionTableKey(positionCopy2);
	moveMaker.m_TranspositionTable[transpositionTableKey].m_ZobristHash = positionCopy2.GetZobristHash();
	moveMaker.m_TranspositionTable[transpositionTableKey].m_Score = 10000;
	moveMaker.m_TranspositionTable[transpositionTableKey].m_Depth = 4;
	moveMaker.m_TranspositionTable[transpositionTableKey].m_BestMove = Move(PieceType::Queen, a4, e8);
	moveMaker.m_TranspositionTable[transpositionTableKey].m_Flag = TranspositionTableEntry::Flag::Exact;
	success = moveMaker.MakeMove(positionCopy2, 4, score);
	ASSERT(positionCopy2.GetMoves().back().GetTo() != Piece(PieceType::Queen, e8));

	//Same with null moves in between => count is reset, no draw ; best move is allowed
	moveMaker.m_TranspositionTable = {};
	move = Move();
	move.SetNullMove();
	positionCopy3.Update(move);
	positionCopy3.Update(move);
	success = moveMaker.MakeMove(positionCopy3, 4, score);
	Piece to = positionCopy3.GetMoves().back().GetTo();
	ASSERT(positionCopy3.GetMoves().back().GetTo() == Piece(PieceType::Queen, e8));
}