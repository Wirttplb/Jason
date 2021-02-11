#include "SpeedTest.h"
#include "Position.h"
#include "MoveMaker.h"
#include "MoveSearcher.h"
#include "TestsUtility.h"
#include "NotationParser.h"

static MoveMaker moveMaker;
static void RunPosition(Position& position, int maxMoves, int evaluationDepth)
{
	int moveCount = 0;
	bool moveFound = true;
	double score = 0.0; //ignored
	while (moveFound && moveCount < maxMoves)
	{
		moveFound = moveMaker.MakeMove(position, evaluationDepth, score);
		moveCount++;
	}

	moveMaker.CheckGameOver(position);
}

void SpeedTest::Run()
{
	Position position1("r1bqk1nr/ppp2ppp/2n1p3/3p4/1bPP4/2N2N2/PP2PPPP/R1BQKB1R w KQkq - 0 1");
	Position position2("r1bqk2r/2p1ppbp/p1np1np1/1p6/3PPP2/2NBBN2/PPP3PP/R2QK2R w KQkq - 0 1");
	Position position3("r3r1k1/3q2bp/p1Np2p1/P2Pp1Pn/1R3p1P/KP3P2/6Q1/3RB3 w - - 0 1");
	Position position4("r1b1nrk1/pp1nq3/3p1b2/2pPp1pP/2P1PpP1/2NQ4/PP1NBB2/R3K1R1 b Q - 0 1");
	Position position5("8/6pk/pB5p/P7/2q4P/2b2QP1/5PK1/8 w - -");
	Position position6("3r4/3r1k2/1n2p3/1p2PpP1/pP3P2/P6p/2K4B/3RR3 b - -");
	Position position7("6k1/5p2/6p1/8/7p/8/6PP/6K1 b - - 0 0");
	Position position8("5k2/2n5/8/1pP5/2pP4/8/5B2/1K6 w - -");

	time_t start;
	time_t end;

	time(&start);
	RunPosition(position1, 1, 7);
	time(&end);
	PrintTestDuration(start, end, "SpeedTest1: %.2lf seconds.");
	ASSERT(difftime(end, start) < 10.0);

	time(&start);
	RunPosition(position2, 1, 6);
	time(&end);
	PrintTestDuration(start, end, "SpeedTest2: %.2lf seconds.");
	ASSERT(difftime(end, start) < 25.0);

	time(&start);
	RunPosition(position3, 1, 6);
	time(&end);
	PrintTestDuration(start, end, "SpeedTest3: %.2lf seconds.");
	ASSERT(difftime(end, start) < 15.0);

	time(&start);
	RunPosition(position4, 1, 7);
	time(&end);
	PrintTestDuration(start, end, "SpeedTest4: %.2lf seconds.");
	ASSERT(difftime(end, start) < 15.0);

	time(&start);
	RunPosition(position5, 1, 7);
	time(&end);
	PrintTestDuration(start, end, "SpeedTest5: %.2lf seconds.");
	ASSERT(difftime(end, start) < 10.0);

	time(&start);
	RunPosition(position6, 1, 8);
	time(&end);
	PrintTestDuration(start, end, "SpeedTest6: %.2lf seconds.");
	ASSERT(difftime(end, start) < 3.0);

	time(&start);
	RunPosition(position7, 1, 14);
	time(&end);
	PrintTestDuration(start, end, "SpeedTest7: %.2lf seconds.");
	ASSERT(difftime(end, start) < 3.0);

	time(&start);
	RunPosition(position8, 1, 10);
	time(&end);
	PrintTestDuration(start, end, "SpeedTest8: %.2lf seconds.");
	ASSERT(difftime(end, start) < 3.0);
}