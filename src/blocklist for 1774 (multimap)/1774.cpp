#include "blocklist_multimap.h"
#include <iostream>

struct mystring
{
    char string[65];
    friend bool operator<(const mystring &a, const mystring &b)
    {
        return strcmp(a.string, b.string) < 0;
    }
    friend bool operator==(const mystring &a, const mystring &b)
    {
        return strcmp(a.string, b.string) == 0;
    }
};

int main()
{
    freopen("input.dat", "r", stdin);
    freopen("ans.txt", "w", stdout);
    std::ios::sync_with_stdio(0);
    int n;
    std::cin >> n;
    Blocklist<mystring, int> list("1774data");
    for (int i = 0; i < n; ++i)
    {
        char buff[65];
        std::cin >> buff;
        if (buff[0] == 'i')
        {
            int value;
            std::cin >> buff >> value;
            mystring key;
            strcpy(key.string, buff);
            list.insert(key, value);
        }
        else if (buff[0] == 'd')
        {
            int value;
            std::cin >> buff >> value;
            mystring key;
            strcpy(key.string, buff);
            list.erase(key, value);
        }
        else
        {
            std::cin >> buff;
            mystring key;
            strcpy(key.string, buff);
            std::vector<int> res = list.find(key);
            if (res.empty())
                std::cout << "null\n";
            else
            {
                for (int i = 0; i < res.size(); ++i)
                    std::cout << res[i] << ' ';
                std::cout << '\n';
            }
        }
    }
}
