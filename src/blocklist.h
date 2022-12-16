# ifndef BLOCKLIST_H
# define BLOCKLIST_H

#include <fstream>
#include <cstring>
#include <algorithm>

#define MAX_SIZE 600

template<typename key_type, typename value_type>
class Blocklist
{
    char index_name[64];

    struct element
    {
        key_type key;
        value_type value;
        friend bool operator<(const element &a, const element &b)
        {
            return a.key < b.key;
        }
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

    // return the address of the corresponding node
    long seeknode(const key_type &key)
    {
        std::fstream findex;
        findex.open(index_name);
        long end;
        findex.read(reinterpret_cast <char *> (&end), sizeof(long));
        node_info info, next_info;
        findex.read(reinterpret_cast <char *> (&info), sizeof(node_info));
        long now = sizeof(long);
        while (info.next)
        {
            if (key < info.Max || key == info.Max) break;
            findex.seekg(info.next);
            findex.read(reinterpret_cast <char *> (&next_info), sizeof(node_info));
            if (key < next_info.Min) break;
            now = info.next;
            info = next_info;
        }
        return now;
    }

public:
    // create files and write some metadata
    Blocklist(const char *name)
    {
        strcpy(index_name, name);
        std::fstream findex;
        findex.open(index_name);
        if (findex.good()) return;
        findex.open(index_name, std::ios::out);
        long end = sizeof(long);
        findex.write(reinterpret_cast <char *> (&end), sizeof(long));
    }

    // assign the value to the key
    void assign(const key_type &key, const value_type &value)
    {
        std::fstream findex;
        findex.open(index_name);
        findex.seekg(0);
        long end;
        findex.read(reinterpret_cast <char *> (&end), sizeof(long));

        // initialize
        if (end == sizeof(long))
        {
            end += sizeof(node);
            findex.seekp(0);
            findex.write(reinterpret_cast <char *> (&end), sizeof(long));
            node tmp;
            tmp.info.size = 1;
            tmp.elements[0].key = tmp.info.Min = tmp.info.Max = key;
            tmp.elements[0].value = value;
            findex.write(reinterpret_cast <char *> (&tmp), sizeof(node));
            return;
        }

        // find the key
        long now = seeknode(key);
        findex.seekg(now);
        node tmp_node;
        findex.read(reinterpret_cast <char *> (&tmp_node), sizeof(node));
        element toinsert;
        toinsert.key = key;
        element *locat = std::lower_bound(tmp_node.elements, tmp_node.elements+tmp_node.info.size, toinsert);

        // modify data if exist
        if (locat != tmp_node.elements+tmp_node.info.size && locat->key == key)
        {
            locat->value = value;
            return;
        }

        // insert data if not exist
        for (int i = tmp_node.info.size; tmp_node.elements + i != locat; --i)
            tmp_node.elements[i] = tmp_node.elements[i-1];
        locat->key = key;
        locat->value = value;
        tmp_node.info.Min = tmp_node.elements[0].key;
        tmp_node.info.Max = tmp_node.elements[tmp_node.info.size].key;
        tmp_node.info.size++;
        if (tmp_node.info.size < MAX_SIZE)
        {
            findex.seekp(now);
            findex.write(reinterpret_cast <char *> (&tmp_node), sizeof(node));
            return;
        }

        // split if node is full
        node new_node;
        new_node.info.next = tmp_node.info.next;
        tmp_node.info.next = end;
        new_node.info.size = tmp_node.info.size = MAX_SIZE / 2;
        new_node.info.Max = tmp_node.info.Max;
        new_node.info.Min = tmp_node.elements[MAX_SIZE/2].key;
        tmp_node.info.Max = tmp_node.elements[MAX_SIZE/2-1].key;
        for (int i = 0; i < MAX_SIZE/2; ++i)
            new_node.elements[i] = tmp_node.elements[i+MAX_SIZE/2];
        findex.seekp(now);
        findex.write(reinterpret_cast <char *> (&tmp_node), sizeof(node));
        findex.seekp(end);
        findex.write(reinterpret_cast <char *> (&new_node), sizeof(node));
        end += sizeof(node);
        findex.seekp(0);
        findex.write(reinterpret_cast <char *> (&end), sizeof(long));
    }

    // find the key and store the value in res, return false if key not found
    bool find(const key_type &key, value_type &res)
    {
        std::fstream findex;
        findex.open(index_name);
        findex.seekg(0);
        long end;
        findex.read(reinterpret_cast <char *> (&end), sizeof(long));
        if (end == sizeof(long)) return false;
        findex.seekg(seeknode(key));
        node tmp_node;
        findex.read(reinterpret_cast <char *> (&tmp_node), sizeof(node));
        element tofind;
        tofind.key = key;
        element *locat = std::lower_bound(tmp_node.elements, tmp_node.elements+tmp_node.info.size, tofind);
        if (locat == tmp_node.elements+tmp_node.info.size || !(locat->key == key))
            return false;
        res = locat->value;
        return true;
    }

    // erase the key, return false if key not found
    bool erase(const key_type &key)
    {
        std::fstream findex;
        findex.open(index_name);
        findex.seekg(0);
        long end;
        findex.read(reinterpret_cast <char *> (&end), sizeof(long));
        if (end == sizeof(long)) return false;
        long now = seeknode(key);
        findex.seekg(now);
        node tmp_node;
        findex.read(reinterpret_cast <char *> (&tmp_node), sizeof(node));
        element tofind;
        tofind.key = key;
        element *locat = std::lower_bound(tmp_node.elements, tmp_node.elements+tmp_node.info.size, tofind);

        if (locat == tmp_node.elements+tmp_node.info.size || !(locat->key == key))
            return false;
        for ( ; locat != tmp_node.elements + tmp_node.info.size - 1; ++locat)
            *locat = *(locat+1);
        tmp_node.info.size--;
        if (tmp_node.info.size)
        {
            tmp_node.info.Min = tmp_node.elements[0].key;
            tmp_node.info.Max = tmp_node.elements[tmp_node.info.size-1].key;
        }
        findex.seekp(now);
        findex.write(reinterpret_cast <char *> (&tmp_node), sizeof(node));
        return true;
    }
};

# endif
