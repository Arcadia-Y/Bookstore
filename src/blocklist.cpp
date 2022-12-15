# include "blocklist.h"

template<typename key_type, typename value_type>
Blocklist<key_type, value_type>::Blocklist(const char *name)
{
    strcpy(fname, name);
    strcpy(index_name, fname);
    strcat(index_name, "_index");
    std::fstream fdata, findex;
    fdata.open(fname);
    if (fdata.good())
    {
        fdata.close();
        return;
    }
    fdata.open(fname, std::ios::out);
    findex.open(index_name, std::ios::out);
    long cursor = sizeof(long);
    fdata.write(reinterpret_cast<char*> (&cursor), sizeof(long));
    long end = sizeof(long);
    findex.write(reinterpret_cast <char *> (&end), sizeof(long));
    fdata.close();
    findex.close();
}

template<typename key_type, typename value_type>
long Blocklist<key_type, value_type>::seeknode(const key_type &key)
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
        if (key < info.Max || key == info.Max) break;
        findex.seekg(info.next);
        findex.read(reinterpret_cast <char *> (&next_info), sizeof(node_info));
        if (key < next_info.Min) break;
        now = info.next;
        info = next_info;
    }
    return now;
}

template<typename key_type, typename value_type>
void Blocklist<key_type, value_type>::insert(const key_type &key, const value_type &value)
{
    std::fstream fdata, findex;
    fdata.open(fname);
    findex.open(index_name);

    // write value into file
    fdata.seekg(0);
    long address;
    fdata.read(reinterpret_cast <char *> (&address), sizeof(long));
    fdata.seekg(address);
    fdata.write(reinterpret_cast <char *> (&value), sizeof(value_type));
    address += sizeof(value_type);
    fdata.seekg(0);
    fdata.write(reinterpret_cast <char *> (&address), sizeof(long));

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
        tmp.elements[0] = tmp.info.Min = tmp.info.Max = value;
        findex.write(reinterpret_cast <char *> (&tmp), sizeof(node));
        return;
    }
    long now = seeknode(key);
    findex.seekg(now);
    node tmp_node = findex.read(reinterpret_cast <char *> (&tmp_node), sizeof(node));
    int locat = 0;
    for (; locat < tmp_node.info.size; ++locat)
    {
        if (key < tmp_node.elements[locat].key) 
            break;
        if (key == tmp_node.elements[locat].key)
        {
            fdata.seekg(tmp_node.elements[locat].address);
            value_type tmp_value;
            fdata.read(reinterpret_cast <char *> (&tmp_value), sizeof(value_type));
            if (value < tmp_value) break;
        }
    }
    for (int i = tmp_node.info.size; i > locat; --i)
        tmp_node.elements[i] = tmp_node.elements[i-1];
    tmp_node.elements[locat].key = key;
    tmp_node.elements[locat].address = address;
    if (key < tmp_node.info.Min) tmp_node.info.Min = key;
    if (tmp_node.info.Max < key) tmp_node.info.Max = key;
    tmp_node.info.size++;
    if (tmp_node.info.size < MAX_SIZE) return;

    // seperate
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

template<typename key_type, typename value_type>
std::vector<value_type> Blocklist<key_type, value_type>::find(const key_type &key)
{
    std::vector<value_type> res;
    std::fstream fdata, findex;
    fdata.open(fname);
    findex.open(index_name);
    findex.seekg(0);
    long end;
    findex.read(reinterpret_cast <char *> (&end), sizeof(long));
    if (end == sizeof(long)) return res;
    findex.seekg(seeknode(key));
    node tmp_node;
    findex.read(reinterpret_cast <char *> (&tmp_node), sizeof(node));

    for (int i = 0; i < tmp_node.info.size; ++i)
    {
        if (key < tmp_node.elements[i].key) break;
        if (key == tmp_node.elements[i].key)
        {
            fdata.seekg(tmp_node.elements[locat].address);
            value_type tmp_value;
            fdata.read(reinterpret_cast <char *> (&tmp_value), sizeof(value_type));
            res.push_back(tmp_value); 
        }
    }
    return res;
}

template<typename key_type, typename value_type>
bool Blocklist<key_type, value_type>::erase(const key_type &key, const value_type &value)
{
    std::fstream fdata, findex;
    fdata.open(fname);
    findex.open(index_name);
    findex.seekg(0);
    long end;
    findex.read(reinterpret_cast <char *> (&end), sizeof(long));
    if (end == sizeof(long)) return false;
    long now = seeknode(key);
    findex.seekg(now);
    node tmp_node;
    findex.read(reinterpret_cast <char *> (&tmp_node), sizeof(node));

    int locat = -1;
    for (int i = 0; i < tmp_node.info.size; ++i)
    {
        if (key < tmp_node.elements[i].key) break;
        if (key == tmp_node.elements[i].key)
        {
            fdata.seekg(tmp_node.elements[i].address);
            value_type tmp_value;
            fdata.read(reinterpret_cast <char *> (&tmp_value), sizeof(value_type));
            if (value == tmp_value)
            {
                locat = i;
                break;
            }
        }
    }
    if (locat == -1) return false;
    for (int i = locat; i < tmp_node.info.size - 1; ++i)
        tmp_node.elements[i] = tmp_node.elements[i+1];
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
