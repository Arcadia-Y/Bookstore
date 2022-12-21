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
        scanner.parseline(line);
        if (scanner.exit())
            return 0;
        scanner.execute();
    }
}
