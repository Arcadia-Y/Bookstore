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
            return 0;
        getline(std::cin, line);
        if (line == "quit" || line == "exit")
            return 0;
        scanner.parseline(line);
        scanner.execute();
    }
}
