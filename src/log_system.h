# ifndef LOG_SYSTEM_H
# define LOG_SYSTEM_H

#include <iostream>
#include <iomanip>
#include "datafile.h"

class Log_System
{
    struct transaction
    {
        double income;
        double outlay;
    };
    long end_address;
    Datafile<transaction> fdata;

public:
    Log_System(): fdata("finance_data")
    { 
        end_address = fdata.end();
        if (end_address == sizeof(long))
        {
            transaction tmp;
            tmp.income = tmp.outlay = 0;
            fdata.write(tmp);
            end_address += sizeof(transaction);
        }
    }

    // show finance, 0 for all
    void show(int count)
    {
        if (count > (end_address - sizeof(long)) / sizeof(transaction) - 1)
            throw 0;
        std::cout << std::fixed << std::setprecision(2);
        transaction tsc1, tsc2;
        fdata.read(end_address - sizeof(transaction), tsc1);
        if (!count)
        {
            std::cout << "+ " << tsc1.income << " - " << tsc1.outlay << '\n';
            return;
        }
        fdata.read(end_address - sizeof(transaction) * (1 + count), tsc2);
        std::cout << "+ " << tsc1.income - tsc2.income << " - " << tsc1.outlay - tsc2.outlay << '\n';
    }

    // record transaction
    void record_transaction(double amount)
    {
        transaction tmp;
        fdata.read(end_address - sizeof(transaction), tmp);
        if (amount > 0)
            tmp.income += amount;
        else
            tmp.outlay -= amount;
        fdata.write(tmp);
        end_address += sizeof(transaction);
    }

    // record log information
    void record_log(const std::string &info)
    {

    }

    // todo
    void showlog()
    {

    }
};

# endif