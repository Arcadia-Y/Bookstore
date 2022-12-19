# ifndef MYSTRING_H
# define MYSTRING_H

#include <cstring>

struct string21
{
    char string[21];
    string21()
    { }
    string21(const std::string &s)
    {
        strcpy(string, s.c_str());
    }
    string21(char *s)
    {
        strcpy(string, s);
    }
    friend bool operator<(const string21 &a, const string21 &b)
    {
        return strcmp(a.string, b.string) < 0;
    }
    friend bool operator==(const string21 &a, const string21 &b)
    {
        return strcmp(a.string, b.string) == 0;
    }
};

struct string31
{
    char string[31];
    string31()
    { }
    string31(const std::string &s)
    {
        strcpy(string, s.c_str());
    }
    string31(char *s)
    {
        strcpy(string, s);
    }
    friend bool operator<(const string31 &a, const string31 &b)
    {
        return strcmp(a.string, b.string) < 0;
    }
    friend bool operator==(const string31 &a, const string31 &b)
    {
        return strcmp(a.string, b.string) == 0;
    }
};

struct string61
{
    char string[61];
    string61()
    { }
    string61(const std::string &s)
    {
        strcpy(string, s.c_str());
    }
    string61(char *s)
    {
        strcpy(string, s);
    }
    friend bool operator<(const string61 &a, const string61 &b)
    {
        return strcmp(a.string, b.string) < 0;
    }
    friend bool operator==(const string61 &a, const string61 &b)
    {
        return strcmp(a.string, b.string) == 0;
    }
};

# endif
