#include <iostream>
#include <time.h>
#include "NotationParserTests.h"
#include "PositionTests.h"
#include "ZobristTests.h"
#include "MoveSearcherTests.h"
#include "TacticsTests.h"

int main()
{
    std::cout << "Tests are running..." << std::endl;

    time_t start;
    time_t end;
    
    time(&start);
    NotationParserTests::Run();
    PositionTests::Run();
    //ZobristTests::Run();
    MoveSearcherTests::Run();
    //TacticsTests::Run();
    time(&end);

    const double duration = difftime(end, start);
    std::string message;
    message.resize(50);
    sprintf_s(message.data(), 50, "Tests were run in %.2lf seconds.", duration);
    std::cout << message << std::endl;
}
