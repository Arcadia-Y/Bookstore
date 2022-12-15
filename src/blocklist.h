# ifndef BLOCKLIST_H
# define BLOCKLIST_H

#include <fstream>
#include <cstring>
#include <vector>
#include <algorithm>

#define MAX_SIZE 600

template<typename key_type, typename value_type>
class Blocklist
{
    char fname[64];
    char index_name[64];

    struct element
    {
        key_type key;
        long address;
    };

    struct node_info
    {
        long next = 0;
        int size;
        key_type Min;
        key_type Max;
    };

    struct node
    {
        node_info info;
        element elements[MAX_SIZE];
    };

    long seeknode(const key_type &key); // return the address of the corresponding node, return 0 if no node

public:
    Blocklist(const char *name); // create files and write some metadata
    void insert(const key_type &key, const value_type &value);
    std::vector<value_type> find(const key_type &key);
    bool erase(const key_type &key, const value_type &value);
    bool modify(const key_type &key, const value_type &value);
};

# endif