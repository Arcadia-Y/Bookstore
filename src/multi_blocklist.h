#ifndef MULTI_BLOCKLIST_H
#define MULTI_BLOCKLIST_H

#include <fstream>
#include <cstring>
#include <vector>
#include <algorithm>

#define MAX_SIZE 800

template<typename key_type, typename value_type>
class Multi_Blocklist
{
    char index_name[64];

    struct element
    {
        key_type key;
        value_type value;
    };

    struct node_info
    {
        long next = 0;
        int size;
        element Min;
        element Max;
    };

    struct node
    {
        node_info info;
        element elements[MAX_SIZE];
    };

    // return the address of the corresponding node, return 0 if no node
    long seeknode(const key_type &key) 
    {
        std::fstream findex;
        findex.open(index_name);
        long end;
        findex.read(reinterpret_cast <char *> (&end), sizeof(long));
        if (end == sizeof(long)) return 0;
        node_info info, next_info;
        findex.read(reinterpret_cast <char *> (&info), sizeof(node_info));
        long now = sizeof(long);
        while (info.next)
        {
            if (key < info.Max.key || key == info.Max.key) break;
            findex.seekg(info.next);
            findex.read(reinterpret_cast <char *> (&next_info), sizeof(node_info));
            if (key < next_info.Min.key) break;
            now = info.next;
            info = next_info;
        }
        return now;
    }

    // seeknode with value
    long seeknode(const key_type &key, const value_type &value)
    {
        std::fstream findex;
        findex.open(index_name);
        long end;
        findex.read(reinterpret_cast <char *> (&end), sizeof(long));
        if (end == sizeof(long)) return 0;
        node_info info, next_info;
        findex.read(reinterpret_cast <char *> (&info), sizeof(node_info));
        long now = sizeof(long);
        while (info.next)
        {
            if (key < info.Max.key) break;
            findex.seekg(info.next);
            findex.read(reinterpret_cast <char *> (&next_info), sizeof(node_info));
            if (key < next_info.Min.key) break;
            if (key == next_info.Min.key && value < next_info.Min.value) break;
            now = info.next;
            info = next_info;
        }
        return now;
    }

public:
    // initialization
    Multi_Blocklist(const char *name)
    {
        strcpy(index_name, name);
        std::fstream findex;
        findex.open(index_name);
        if (findex.good())
        {
            findex.close();
            return;
        }
        findex.open(index_name, std::ios::out);
        long end = sizeof(long);
        findex.write(reinterpret_cast <char *> (&end), sizeof(long));
    }

    // insert certain element, do nothing if element already exists
    void insert(const key_type &key, const value_type &value)
    {
        std::fstream findex;
        findex.open(index_name);

        // insert key
        findex.seekg(0);
        long end;
        findex.read(reinterpret_cast <char *> (&end), sizeof(long));
        if (end == sizeof(long))
        {
            end += sizeof(node);
            findex.seekp(0);
            findex.write(reinterpret_cast <char *> (&end), sizeof(long));
            node tmp;
            tmp.info.size = 1;
            tmp.info.next = 0;
            tmp.elements[0].key = tmp.info.Min.key = tmp.info.Max.key = key;
            tmp.elements[0].value = tmp.info.Min.value = tmp.info.Max.value = value;
            findex.write(reinterpret_cast <char *> (&tmp), sizeof(node));
            return;
        }
        long now = seeknode(key, value);
        findex.seekg(now);
        node tmp_node;
        findex.read(reinterpret_cast <char *> (&tmp_node), sizeof(node));
        element toinsert;
        toinsert.key = key;
        toinsert.value = value;
        element *locat = std::lower_bound(tmp_node.elements, tmp_node.elements+tmp_node.info.size, toinsert,
                [](const element &a, const element &b)
                {
                    if (!(a.key == b.key)) return a.key < b.key;
                    return a.value < b.value;
                });
        if (locat != tmp_node.elements+tmp_node.info.size && locat->key == key && locat->value == value) return;
        for (int i = tmp_node.info.size; tmp_node.elements + i != locat; --i)
            tmp_node.elements[i] = tmp_node.elements[i-1];
        *locat = toinsert;
        tmp_node.info.Min = tmp_node.elements[0];
        tmp_node.info.Max = tmp_node.elements[tmp_node.info.size];
        tmp_node.info.size++;
        if (tmp_node.info.size < MAX_SIZE) 
        {
            findex.seekp(now);
            findex.write(reinterpret_cast <char *> (&tmp_node), sizeof(node));
            return;
        }

        // seperate
        node new_node;
        new_node.info.next = tmp_node.info.next;
        tmp_node.info.next = end;
        new_node.info.size = tmp_node.info.size = MAX_SIZE / 2;
        new_node.info.Max = tmp_node.info.Max;
        new_node.info.Min = tmp_node.elements[MAX_SIZE/2];
        tmp_node.info.Max = tmp_node.elements[MAX_SIZE/2-1];
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

    // return all values with the key in ascending order
    std::vector<value_type> find(const key_type &key)
    {
        std::vector<value_type> res;
        std::fstream findex;
        findex.open(index_name);
        findex.seekg(0);
        long end;
        findex.read(reinterpret_cast <char *> (&end), sizeof(long));
        if (end == sizeof(long)) return res;
        long now = seeknode(key);
        findex.seekg(now);
        node tmp_node;
        findex.read(reinterpret_cast <char *> (&tmp_node), sizeof(node));

        element tofind;
        tofind.key = key;
        element *locat = std::lower_bound(tmp_node.elements, tmp_node.elements+tmp_node.info.size, tofind, 
                [](const element &a, const element &b)
                {
                    return a.key < b.key;
                });
        for (; locat != tmp_node.elements + tmp_node.info.size; ++locat)
        {
            if (key < locat->key) break;
            res.push_back(locat->value); 
        }

        if (!(key == tmp_node.info.Max.key)) return res;
        do 
        {
            if (tmp_node.info.next == 0) break;
            now = tmp_node.info.next;
            findex.seekg(now);
            findex.read(reinterpret_cast <char *> (&tmp_node), sizeof(node));
            locat = tmp_node.elements;
            for (; locat != tmp_node.elements + tmp_node.info.size; ++locat)
            {
                if (key < locat->key) break;
                res.push_back(locat->value); 
            }
        }
        while (key == tmp_node.info.Max.key);
        return res;
    }

    // erase certain element, return false if element not found
    bool erase(const key_type &key, const value_type &value)
    {
        std::fstream findex;
        findex.open(index_name);
        findex.seekg(0);
        long end;
        findex.read(reinterpret_cast <char *> (&end), sizeof(long));
        if (end == sizeof(long)) return false;
        long now = seeknode(key, value);
        findex.seekg(now);
        node tmp_node;
        findex.read(reinterpret_cast <char *> (&tmp_node), sizeof(node));

        element toerase;
        toerase.key = key;
        toerase.value = value;
        element *locat = std::lower_bound(tmp_node.elements, tmp_node.elements+tmp_node.info.size, toerase,
                [](const element &a, const element &b)
                {
                    if (!(a.key == b.key)) return a.key < b.key;
                    return a.value < b.value;
                });
        if (locat != tmp_node.elements + tmp_node.info.size)
        {
            if (!(locat->value == value && locat->key == key)) return false;
            goto erasing;
        }

        do 
        {
            if (tmp_node.info.next == 0) break;
            now = tmp_node.info.next;
            findex.seekg(now);
            findex.read(reinterpret_cast <char *> (&tmp_node), sizeof(node));
            locat = std::lower_bound(tmp_node.elements, tmp_node.elements+tmp_node.info.size, toerase,
                                    [](const element &a, const element &b)
                                    {
                                        if (!(a.key == b.key)) return a.key < b.key;
                                        return a.value < b.value;
                                    });
            if (locat != tmp_node.elements+tmp_node.info.size)
            {
                if (!(locat->key == key && locat->value == value)) return false;
                break;
            }
        }
        while (key == tmp_node.info.Max.key);
        if (locat == tmp_node.elements + tmp_node.info.size) return false;

        erasing:
        for ( ; locat != tmp_node.elements + tmp_node.info.size - 1; ++locat)
                *locat = *(locat+1);
        tmp_node.info.size--;
        if (tmp_node.info.size)
        {
            tmp_node.info.Min = tmp_node.elements[0];
            tmp_node.info.Max = tmp_node.elements[tmp_node.info.size-1];
        }
        findex.seekp(now);
        findex.write(reinterpret_cast <char *> (&tmp_node), sizeof(node));
        return true;
    }
};

#endif
