# ifndef MYSTRING_H
# define MYSTRING_H

#include <cstring>

template <int size>
struct mystring
{
    char string[size];
    mystring()
    { }
    mystring(const std::string &s)
    {
        strcpy(string, s.c_str());
    }
    mystring(char *s)
    {
        strcpy(string, s);
    }
    friend bool operator<(const mystring &a, const mystring &b)
    {
        return strcmp(a.string, b.string) < 0;
    }
    friend bool operator==(const mystring &a, const mystring &b)
    {
        return strcmp(a.string, b.string) == 0;
    }
};

# endif
