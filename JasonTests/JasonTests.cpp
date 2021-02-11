#include <iostream>
#include "TestsUtility.h"
#include "NotationParserTests.h"
#include "PositionTests.h"
#include "ZobristTests.h"
#include "MoveMakerTests.h"
#include "MoveSearcherTests.h"
#include "PositionEvaluationTests.h"
#include "TacticsTests.h"
#include "SpeedTest.h"

int main()
{
    std::cout << "Tests are running..." << std::endl;

    time_t start;
    time_t end;
    
    time(&start);
    NotationParserTests::Run();
    PositionTests::Run();
    ZobristTests::Run();
    //MoveMakerTests::Run();
    //MoveSearcherTests::Run();
    PositionEvaluationTests::Run();
    TacticsTests::Run();
    SpeedTest::Run();
    time(&end);

    PrintTestDuration(start, end, "Tests were run in %.2lf seconds");
}
