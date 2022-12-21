# ifndef SCANNER_H
# define SCANNER_H

#include <iostream>
#include <cstring>
#include <vector>
#include "book_system.h"
#include "user_system.h"
#include "log_system.h"

class Scanner
{
    User_System user_system;
    Book_System book_system;
    Log_System log_system;
    std::vector<std::string> tokens;
    char level = '0';

    void checkid(const std::string &id)
    {
        if (id.size() > 30) throw 0;
        for (int i = 0; i < id.size(); i++)
            if ((id[i] < '0' || id [i] > '9') && (id[i] < 'A' || id [i] > 'Z') && 
                (id[i] < 'a' || id [i] > 'z') && id[i] != '_')
                throw 0;
    }

    void checkint(const std::string &input)
    {
        if (input.size() > 10) throw 0;
        for (int i = 0; i < input.size(); i++)
            if (input[i] < '0' || input[i] > '9')
                throw 0;
    }

    void checkfloat(const std::string &input)
    {
        if (input.size() > 13) throw 0;
        bool point = false;
        for (int i = 0; i < input.size(); i++)
        {
            if (input[i] == '.' && !point && i != 0 && i != input.size()-1)
                point = true;
            else if (input[i] < '0' || input[i] > '9')
                throw 0;
        }
    }

    void parse_exp(const std::string &exp, int &type, std::string &info)
    {
        if (exp[0] != '-') throw 0;
        std::string buff;
        int i = 1;
        for (; i < exp.size(); i++)
        {
            if (exp[i] == '=') break;
            buff += exp[i];
        }
        if (i >= exp.size() - 1) throw 0;
        info.clear();
        for (i += 1; i < exp.size(); i++)
            info += exp[i];
        if (info.empty()) throw 0;
        if (buff == "ISBN")
        {
            if (info.size() > 20) throw 0;
            type = 1;
        }
        else if (buff == "price")
        {
            checkfloat(info);
            type = 5;
        }
        else
        {
            if (buff == "name") type = 2;
            else if (buff == "author") type = 3;
            else if (buff == "keyword") type = 4;
            else throw 0;
            if (info.size() > 60 || info.size() < 3) throw 0;
            if (info[0] != '\"' || info.back() != '\"') throw 0;
            for (int j = 1; j < info.size()-1; j++)
            {
                if (info[j] == '\"') throw 0;
                info[j-1] = info[j];
            }
            info.pop_back();
            info.pop_back();
        }
    }

public:
    // parse a line
    void parseline(const std::string &line)
    {
        tokens.clear();
        std::string buff;
        int i = 0;
        for (int i = 0; i < line.size(); i++)
        {
            if (line[i] == ' ')
            {
                if (!buff.empty())
                {
                    tokens.push_back(buff);
                    buff.clear();
                }
            }
            else
                buff += line[i];  
        }
        if (!buff.empty()) tokens.push_back(buff);
    }

    // execute the parsed line
    void execute()
    {
        try
        {
            if (tokens.empty()) return;
            std::string buff = tokens[0];
            if (buff == "su")
            {
                if (tokens.size() == 2)
                {
                    user_system.login(tokens[1]);
                    level = user_system.get_level();
                }
                else if (tokens.size() == 3)
                {
                    user_system.login(tokens[1], tokens[2]);
                    level = user_system.get_level();
                }
                else throw 0;
            }
            else if (buff == "logout")
            {
                if (level == '0' || tokens.size() != 1) throw 0;
                user_system.logout();
                std::string ISBN;
                level = user_system.get_level();
                user_system.get_ISBN(ISBN);
                book_system.select(ISBN);
            }
            else if (buff == "register")
            {
                if (tokens.size() != 4) throw 0;
                checkid(tokens[1]);
                checkid(tokens[2]);
                if (tokens[3].size() > 30) throw 0;
                user_system.rgs(tokens[1], tokens[2], tokens[3]);
            }
            else if (buff == "passwd")
            {
                if (level == '0') throw 0;
                if (tokens.size() == 3)
                {
                    if (level != '7') throw 0;
                    checkid(tokens[2]);
                    user_system.modify_password(tokens[1], tokens[2]);
                }
                else if (tokens.size() == 4)
                {
                    checkid(tokens[3]);
                    user_system.modify_password(tokens[1], tokens[2], tokens[3]);
                }
                else throw 0;
            }
            else if (buff == "useradd")
            {
                if (level < '3' || tokens.size() != 5) throw 0;
                checkid(tokens[1]);
                checkid(tokens[2]);
                if (tokens[3] != "1" && tokens[3] != "3" && tokens[3] != "7")
                    throw 0;
                if (tokens[4].size() > 30) throw 0;
                user_system.adduser(tokens[1], tokens[2], tokens[3][0], tokens[4]);
            }
            else if (buff == "delete")
            {
                if (level != '7' || tokens.size() != 2) throw 0;
                user_system.deleteuser(tokens[1]);
            }
            else if (buff == "show")
            {
                if (level == '0') throw 0;
                if (tokens.size() == 1)
                {
                    book_system.show(0, buff);
                }
                else if (tokens[1] == "finance")
                {
                    if (level != '7') throw 0;
                    if (tokens.size() == 2)
                    {
                        log_system.show(0);
                    }
                    else if (tokens.size() == 3)
                    {
                        checkint(tokens[2]);
                        int number = std::stoi(tokens[2]);
                        if (!number)
                        {
                            std::cout << '\n';
                        }
                        else
                        {
                            log_system.show(number);
                        }
                    }
                    else throw 0;
                }
                else if (tokens.size() == 2)
                {
                    int type;
                    parse_exp(tokens[1], type, buff);
                    if (type == 5) throw 0;
                    if (type == 4 && buff.find('|') != std::string::npos) throw 0;
                    book_system.show(type, buff);
                }
                else throw 0;  
            }
            else if (buff == "buy")
            {
                if (level == '0' || tokens.size() != 3) throw 0;
                checkint(tokens[2]);
                int quantity = std::stoi(tokens[2]);
                if (!quantity) throw 0;
                double cost = book_system.buy(tokens[1], quantity);
                log_system.record_transaction(cost);
            }
            else if (buff == "select")
            {
                if (level < '3' || tokens.size() != 2) throw 0;
                if (tokens[1].size() > 20) throw 0;
                book_system.select(tokens[1]);
                user_system.select(tokens[1]);
            }
            else if (buff == "modify")
            {
                if (level < '3' || tokens.size() < 2 || tokens.size() > 6) throw 0;
                std::vector<int> types;
                std::vector<std::string> info_vec;
                std::string newISBN;
                for (int i = 1; i < tokens.size(); i++)
                {
                    int type;
                    parse_exp(tokens[i], type, buff);
                    for (int j = 0; j < types.size(); j++)
                        if (types[j] == type)
                            throw 0;
                    types.push_back(type);
                    info_vec.push_back(buff);
                    if (type == 1) newISBN = buff;
                }
                book_system.modify(types, info_vec);
                if (!newISBN.empty())
                    user_system.update_ISBN(newISBN);
            }
            else if (buff == "import")
            {
                if (level < '3' || tokens.size() != 3) throw 0;
                checkint(tokens[1]);
                checkfloat(tokens[2]);
                int quantity = std::stoi(tokens[1]);
                double cost = std::stod(tokens[2]);
                if (quantity <= 0 || cost <= 0) throw 0;
                book_system.import_books(quantity);
                log_system.record_transaction(-cost);
            }
            else if (buff == "log")
            {
                if (level != '7' || tokens.size() != 1) throw 0;
                log_system.showlog();
            }
            else throw 0;
        }
        catch(int)
        {
            std::cout << "Invalid\n";
        }
    }

    // return true if program should exit
    bool exit()
    {
        if (tokens.size() != 1) return false;
        return tokens[0] == "quit" || tokens[0] == "exit";
    }
};

# endif