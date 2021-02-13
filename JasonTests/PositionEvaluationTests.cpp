#include "PositionEvaluationTests.h"
#include "PositionEvaluation.h"
#include "TestsUtility.h"

void TestMovesToMate()
{
	std::optional<int> movesToMate;
	movesToMate = PositionEvaluation::GetMovesToMate(Mate);
	ASSERT(*movesToMate == 0);

	movesToMate = PositionEvaluation::GetMovesToMate(Mate - 1);
	ASSERT(*movesToMate == 1);

	movesToMate = PositionEvaluation::GetMovesToMate(Mate - 2);
	ASSERT(*movesToMate == 1);

	movesToMate = PositionEvaluation::GetMovesToMate(Mate - 3);
	ASSERT(*movesToMate == 2);

	movesToMate = PositionEvaluation::GetMovesToMate(Mate - 4);
	ASSERT(*movesToMate == 2);

	movesToMate = PositionEvaluation::GetMovesToMate(Mate - 5);
	ASSERT(*movesToMate == 3);

	movesToMate = PositionEvaluation::GetMovesToMate(12345);
	ASSERT(!movesToMate.has_value());
}

void PositionEvaluationTests::Run()
{
	TestMovesToMate();

	static Position position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	ASSERT(PositionEvaluation::CountDoubledPawns(position, true) == 0);
	ASSERT(PositionEvaluation::CountDoubledPawns(position, false) == 0);

	position = Position("rnbqkbnr/pppp1ppp/8/8/3pP3/8/PPP2PPP/RNBQKBNR w KQkq - 0 1");
	ASSERT(PositionEvaluation::CountDoubledPawns(position, true) == 0);
	ASSERT(PositionEvaluation::CountDoubledPawns(position, false) == 2);

	position = Position("rnbqk1nr/p1pp2pp/8/5P2/2pp1P2/8/PP3P1P/RNBQKBNR w KQkq - 0 1");
	ASSERT(PositionEvaluation::CountDoubledPawns(position, true) == 3);
	ASSERT(PositionEvaluation::CountDoubledPawns(position, false) == 4);

	position = Position("rn1qkb1r/p1ppn1p1/Pp3p2/4p2P/P6P/8/2PPPP2/RNBQKBNR w KQkq - 0 1");
	ASSERT(PositionEvaluation::CountDoubledPawns(position, true) == 4);
	ASSERT(PositionEvaluation::CountDoubledPawns(position, false) == 0);

	position = Position("rnb2rk1/pp3p2/3b1p1p/2pPp3/8/8/PP3PPP/RN1QKBNR w KQq - 0 1");
	ASSERT(PositionEvaluation::CountIsolatedPawns(position, true) == 1);
	ASSERT(PositionEvaluation::CountIsolatedPawns(position, false) == 1);

	position = Position("rnb2rk1/pp3p2/3b3p/2pP4/5p2/8/PP3P1P/RN1QKBNR w KQq - 0 1");
	ASSERT(PositionEvaluation::CountIsolatedPawns(position, true) == 3);
	ASSERT(PositionEvaluation::CountIsolatedPawns(position, false) == 3); //doubled isolated pawns count also as isolated

	ASSERT(PositionEvaluation::CountCenterPawns(position, true) == 0);
	ASSERT(PositionEvaluation::CountCenterPawns(position, false) == 0);
	position = Position("rnb2rk1/pp3p2/3b3p/3pp3/3PPp2/8/PP3P1P/RN1QKBNR w KQq - 0 1");
	ASSERT(PositionEvaluation::CountCenterPawns(position, true) == 2);
	ASSERT(PositionEvaluation::CountCenterPawns(position, false) == 2);

	ASSERT(PositionEvaluation::CountBlockedEorDPawns(position, true) == 0);
	ASSERT(PositionEvaluation::CountBlockedEorDPawns(position, false) == 0);

	position = Position("rnb2rk1/pp1ppp2/7p/2b5/5p2/8/PP1PPP1P/RN1QKBNR w KQq - 0 1");
	ASSERT(PositionEvaluation::CountBlockedEorDPawns(position, true) == 0);
	ASSERT(PositionEvaluation::CountBlockedEorDPawns(position, false) == 0);

	position = Position("rnb2rk1/pp1ppp2/3bn2p/8/5p2/3NN3/PP1PPP1P/R2QKB1R w KQq - 0 1");
	ASSERT(PositionEvaluation::CountBlockedEorDPawns(position, true) == 2);
	ASSERT(PositionEvaluation::CountBlockedEorDPawns(position, false) == 2);

	Bitboard whiteAttackedSquares = PositionEvaluation::GetAttackedSquares(position, true);
	Bitboard blackAttackedSquares = PositionEvaluation::GetAttackedSquares(position, false);
	Bitboard attackedSquaresAroundBlackKing = PositionEvaluation::GetAttackedSquaresAroundKing(position, whiteAttackedSquares,false);
	Bitboard attackedSquaresAroundWhiteKing = PositionEvaluation::GetAttackedSquaresAroundKing(position, blackAttackedSquares, true);
	ASSERT(attackedSquaresAroundBlackKing.CountSetBits() == 0);
	ASSERT(attackedSquaresAroundWhiteKing.CountSetBits() == 0);

	position = Position("rnb2rk1/pp1ppp2/4N3/7Q/5p2/2nN2b1/PP1PPP1P/R3KB1R w KQq - 0 1");
	whiteAttackedSquares = PositionEvaluation::GetAttackedSquares(position, true);
	blackAttackedSquares = PositionEvaluation::GetAttackedSquares(position, false);
	attackedSquaresAroundBlackKing = PositionEvaluation::GetAttackedSquaresAroundKing(position, whiteAttackedSquares, false);
	attackedSquaresAroundWhiteKing = PositionEvaluation::GetAttackedSquaresAroundKing(position, blackAttackedSquares, true);
	ASSERT(attackedSquaresAroundBlackKing.CountSetBits() == 5);
	ASSERT(attackedSquaresAroundWhiteKing.CountSetBits() == 3);

	position = Position("rnb2rk1/pp1ppp2/4N1P1/8/5pQ1/2nN2b1/PP1PPP2/R3KBR1 w Qq - 0 1");
	whiteAttackedSquares = PositionEvaluation::GetAttackedSquares(position, true);
	attackedSquaresAroundBlackKing = PositionEvaluation::GetAttackedSquaresAroundKing(position, whiteAttackedSquares, false);
	ASSERT(attackedSquaresAroundBlackKing.CountSetBits() == 4);

	position = Position("rnb2rk1/pp1ppp1p/4N1P1/8/5pQ1/2nN2b1/PP1PPP2/2R1KB1R w q - 0 1");
	std::pair<int, int> whiteCount = PositionEvaluation::CountRooksOnOpenFiles(position, true);
	std::pair<int, int> blackCount = PositionEvaluation::CountRooksOnOpenFiles(position, false);
	ASSERT(whiteCount.first == 1);
	ASSERT(whiteCount.second == 1);
	ASSERT(blackCount.first == 0);
	ASSERT(blackCount.second == 0);
	position = Position("rnb2rk1/1pppp3/4N1P1/8/6Q1/2nN2b1/1P1PP3/2R1K1R1 w q - 0 1");
	whiteCount = PositionEvaluation::CountRooksOnOpenFiles(position, true);
	blackCount = PositionEvaluation::CountRooksOnOpenFiles(position, false);
	ASSERT(whiteCount.first == 0);
	ASSERT(whiteCount.second == 1);
	ASSERT(blackCount.first == 2);
	ASSERT(blackCount.second == 0);

	position = Position();
	int whiteBackwardsPawn = PositionEvaluation::CountBackwardsPawns(position, true);
	int blackBackwardsPawn = PositionEvaluation::CountBackwardsPawns(position, false);
	ASSERT(whiteBackwardsPawn == 0);
	ASSERT(blackBackwardsPawn == 0);
	position = Position("rnbqkbnr/ppp1pppp/8/8/3P4/2P5/PP4PP/RNBQKBNR w KQkq - 0 1");
	whiteBackwardsPawn = PositionEvaluation::CountBackwardsPawns(position, true);
	blackBackwardsPawn = PositionEvaluation::CountBackwardsPawns(position, false);
	ASSERT(whiteBackwardsPawn == 0);
	ASSERT(blackBackwardsPawn == 0);
	position = Position("rnbqkbnr/ppp1pppp/8/8/3P4/8/PP4PP/RNBQKBNR w KQkq - 0 1");
	whiteBackwardsPawn = PositionEvaluation::CountBackwardsPawns(position, true);
	blackBackwardsPawn = PositionEvaluation::CountBackwardsPawns(position, false);
	ASSERT(whiteBackwardsPawn == 1);
	ASSERT(blackBackwardsPawn == 0);
	position = Position("rnbqkbnr/ppp1pppp/8/2P5/3P4/8/PP4PP/RNBQKBNR w KQkq - 0 1");
	whiteBackwardsPawn = PositionEvaluation::CountBackwardsPawns(position, true);
	blackBackwardsPawn = PositionEvaluation::CountBackwardsPawns(position, false);
	ASSERT(whiteBackwardsPawn == 1);
	ASSERT(blackBackwardsPawn == 0);
	position = Position("rnbqkbnr/1p6/p1p1pp1p/2P3p1/3P3P/5NP1/P7/RNBQKB1R w KQkq - 0 1");
	whiteBackwardsPawn = PositionEvaluation::CountBackwardsPawns(position, true);
	blackBackwardsPawn = PositionEvaluation::CountBackwardsPawns(position, false);
	ASSERT(whiteBackwardsPawn == 1);
	ASSERT(blackBackwardsPawn == 1);
	position = Position("rnbqkbnr/1p6/p1p1pp1p/2P3p1/3P4/5N2/P7/RNBQKB1R w KQkq - 0 1");
	whiteBackwardsPawn = PositionEvaluation::CountBackwardsPawns(position, true);
	blackBackwardsPawn = PositionEvaluation::CountBackwardsPawns(position, false);
	ASSERT(whiteBackwardsPawn == 1);
	ASSERT(blackBackwardsPawn == 2);
	position = Position("rnbqkbnr/1p2p3/2p2p2/2P3p1/3P4/1P3N1P/P7/RNBQKB1R w KQkq - 0 1");
	whiteBackwardsPawn = PositionEvaluation::CountBackwardsPawns(position, true);
	blackBackwardsPawn = PositionEvaluation::CountBackwardsPawns(position, false);
	ASSERT(whiteBackwardsPawn == 3);
	ASSERT(blackBackwardsPawn == 1);
}