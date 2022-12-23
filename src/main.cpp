#include "scanner.h"
#include <iostream>
#include <cstring>

Scanner scanner;

int main()
{
    std::ios::sync_with_stdio(false);
    while (true)
    {
        std::string line;
        if (std::cin.eof())
            line = "quit";
        else 
            getline(std::cin, line);
        scanner.parseline(line);
        scanner.execute();
    }
}
