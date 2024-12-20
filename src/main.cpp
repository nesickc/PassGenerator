#include "PassGenerator.hpp"
#include <iostream>

int main()
{
    PassGenerator passGen(
        "random-ass-seed-phrase"
    );

    auto generatedPassword = passGen.GeneratePassword( "Facebook" );

    std::cout << "Generated password is:" << generatedPassword << std::endl;


    return 0;
}