#include <iostream>
#include <exception>
#include "../include/cli.h"

int main()
{
    try
    {
        return runCli(std::cin, std::cout);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown error occurred." << std::endl;
        return 1;
    }
}