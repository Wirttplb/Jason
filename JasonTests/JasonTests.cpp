#include <iostream>
#include "NotationParserTests.h"
#include "ZobristTests.h"
#include "MoveSearcherTests.h"
#include "TacticsTests.h"

int main()
{
    //NotationParserTests::Run();
    //ZobristTests::Run();
    MoveSearcherTests::Run();
    //TacticsTests::Run();

    std::cout << "Tests have been run.\n";
}
