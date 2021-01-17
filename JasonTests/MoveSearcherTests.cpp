#include "MoveSearcherTests.h"
#include "Position.h"
#include "MoveSearcher.h"
#include "TestsUtility.h"
#include <unordered_set>

Position staleMateWhiteToPlay("8/8/8/8/8/kq6/8/K7 w - - 0 1");
Position staleMateBlackToPlay("8/8/8/8/8/KQ6/8/k7 b - - 0 1");

void MoveSearcherTests::Run()
{
	std::vector<Position> positions = MoveSearcher::GetAllPossiblePositions(staleMateWhiteToPlay);
	ASSERT(positions.empty());

	positions = MoveSearcher::GetAllPossiblePositions(staleMateBlackToPlay);
	ASSERT(positions.empty());

	Position startingPosition;
	positions = MoveSearcher::GetAllPossiblePositions(startingPosition);
	ASSERT(positions.size() == 20);

	positions = MoveSearcher::GetAllPossiblePositions(startingPosition, 1);
	ASSERT(positions.size() == 20);

	std::unordered_set<uint64_t> uniqueHashes;
	for (const Position& position : positions)
	{
		uniqueHashes.insert(position.GetZobristHash());
	}

	ASSERT(uniqueHashes.size() == 20);

	positions = MoveSearcher::GetAllPossiblePositions(startingPosition, 2);
	ASSERT(positions.size() == 20*20);

	uniqueHashes.clear();
	for (const Position& position : positions)
	{
		uniqueHashes.insert(position.GetZobristHash());
	}

	ASSERT(uniqueHashes.size() == 400);

	positions = MoveSearcher::GetAllPossiblePositions(startingPosition, 3);
	ASSERT(positions.size() == 8902);

	uniqueHashes.clear();
	for (const Position& position : positions)
	{
		uniqueHashes.insert(position.GetZobristHash());
	}

	ASSERT(uniqueHashes.size() == 5362);

	positions = MoveSearcher::GetAllPossiblePositions(startingPosition, 4);
	ASSERT(positions.size() == 197281);

	uniqueHashes.clear();
	for (const Position& position : positions)
	{
		uniqueHashes.insert(position.GetZobristHash());
	}

	ASSERT(uniqueHashes.size() == 72084);
}