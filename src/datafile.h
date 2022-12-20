# ifndef DATAFILE_H
# define DATAFILE_H

#include <fstream>
#include <cstring>

template<typename value_type>
class Datafile
{
    char fname[64];

public:
    // initialization
    Datafile(const char *name)
    {
        strcpy(fname, name);
        std::fstream fdata;
        fdata.open(fname);
        if (fdata.good()) return;
        fdata.open(fname, std::ios::out);
        long cursor = sizeof(long);
        fdata.write(reinterpret_cast<char*> (&cursor), sizeof(long));
    }

    // write a new value into data file, return address
    long write(const value_type &value) 
    {
        std::fstream fdata;
        fdata.open(fname);
        fdata.seekg(0);
        long address;
        fdata.read(reinterpret_cast <char *> (&address), sizeof(long));
        fdata.seekp(address);
        fdata.write(reinterpret_cast <const char *> (&value), sizeof(value_type));
        address += sizeof(value_type);
        fdata.seekp(0);
        fdata.write(reinterpret_cast <char *> (&address), sizeof(long));
        return address - sizeof(value_type);
    }

    // modify existd data
    void modify(long address, const value_type &value) 
    {
        std::fstream fdata;
        fdata.open(fname);
        fdata.seekp(address);
        fdata.write(reinterpret_cast <const char *> (&value), sizeof(value_type));
    }

    // read data
    void read(long address, value_type &value)
    {
        std::fstream fdata;
        fdata.open(fname);
        fdata.seekg(address);
        fdata.read(reinterpret_cast <char *> (&value), sizeof(value_type));
    }

    // return end_address of the data
    long end()
    {
        std::fstream fdata;
        fdata.open(fname);
        long address;
        fdata.read(reinterpret_cast <char *> (&address), sizeof(long));
        return address;
    }
};

# endif
