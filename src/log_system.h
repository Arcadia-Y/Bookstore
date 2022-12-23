# ifndef LOG_SYSTEM_H
# define LOG_SYSTEM_H

#include <iostream>
#include <iomanip>
#include "datafile.h"

#define LOG_SIZE 50

class Log_System
{
    struct transaction
    {
        double income;
        double outlay;
    };
    struct log
    {
        char content[LOG_SIZE + 1];
        bool end = false;
    };
    long end_address;
    Datafile<transaction> fdata;
    Datafile<log> logdata;

public:
    Log_System(): fdata("finance_data"), logdata("log_data")
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
    void record_log(std::string &info)
    {
        log tmp;
        while (true)
        {
            strncpy(tmp.content, info.c_str(), LOG_SIZE + 1);
            if (info.size() <= LOG_SIZE)
            {
                tmp.end = true;
                logdata.write(tmp);
                break;
            }
            info = info.substr(LOG_SIZE + 1);
            logdata.write(tmp);
        }
    }

    // show log information
    void showlog()
    {
        long address = sizeof(long);
        long end = logdata.end();
        log tmp;
        std::string info;
        std::cout << "----------------------------\n";
        std::cout << "Log for Bookstore\n";
        std::cout << "----------------------------\n";
        while (address < end)
        {
            logdata.read(address, tmp);
            if (tmp.end)
            {
                 info.append(tmp.content);
                 std::cout << info << '\n';
                 info.clear();
            }
            else
                info.append(tmp.content, LOG_SIZE + 1);
            address += sizeof(log);
        }
        std::cout << "----------------------------\n";
    }
};

# endif