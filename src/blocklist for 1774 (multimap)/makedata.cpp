#include <iostream>

using namespace std;

int main()
{
    ios::sync_with_stdio(0);
    freopen("input.dat", "w", stdout);
    cout << "number\n";
    for (int i = 0; i < 2000; ++i)
        cout << "delete " << "Kiss" << i/7 << ' ' << i << '\n';
    for (int i = 0; i < 2000; i++)
        cout << "insert " << "Kiss" << i/7 << ' ' << i << '\n';
    for (int i = 0; i < 2000; ++i)
        cout << "insert " << "Kiss" << i/7 << ' ' << i << '\n';
    for (int i = 202; i < 1104; i += 6)
        cout << "delete " << "Kiss" << i/7 << ' ' << i << '\n';
    for (int i = 0; i < 2000; i++)
        cout << "delete " << "Kiss " << i << '\n';
    for (int i = 0; i < 2000; i++)
        cout << "insert " << "Kiss " << i << '\n';
    for (int i = 0; i < 2000; i++)
        cout << "delete " << "Kiss " << i << '\n';
    for (int i = 0; i < 2000; i++)
        cout << "insert " << "Kiss " << i << '\n';
    for (int i = 1000; i < 1500; i+=4)
        cout << "delete " << "Kiss " << i << '\n';
    for (int i = 0; i < 2000; i += 7)
        cout << "find " << "Kiss" << i/7 << '\n';
    cout << "find Kiss\n";
}
