# ifndef USER_SYSTEM_H
# define USER_SYSTEM_H

#include <cstring>
#include <vector>
#include "datafile.h"
#include "blocklist.h"
#include "mystring.h"

class User_System
{
    struct User
    {
        char password[31];
        char username[31];
        char privilege;
    };
    char level = '0'; // current priviledge level
    Datafile<User> fdata;
    Blocklist<string31, long> findex;
    std::vector<char[31]> id_stack;
    std::vector<char[21]> ISBN_stack;
    std::vector<char> level_stack;

public:
    User_System()
    {
        fdata("user_data");
        findex("user_data_index");
    }

    void login(char id[31])
    {
        long address;
        if (!findex.find(id, address)) throw 0;
        User user;
        fdata.read(address, user);
        if (user.privilege >= level) throw 0;
        level = user.privilege;
        id_stack.push_back(id);
        ISBN_stack.push_back("");
        level_stack.push_back(user.privilege);
    }

    void login(char id[31], char passwd[31])
    {
        long address;
        if (!findex.find(id, address)) throw 0;
        User user;
        fdata.read(address, user);
        if (strcmp(passwd, user.password)) throw 0;
        level = user.privilege;
        id_stack.push_back(id);
        ISBN_stack.push_back("");
        level_stack.push_back(user.privilege);
    }

    void logout()
    {
        if (id_stack.empty()) throw 0;
        id_stack.pop_back();
        ISBN_stack.pop_back();
        level_stack.pop_back();
        if (level_stack.empty())
            level = '0';
        else
            level = level_stack.back();
    }

    // register
    void rgs(char id[31], char passwd[31], char username[31])
    {
        long address;
        if (findex.find(id, address)) throw 0;
        User user;
        strcpy(user.password, passwd);
        strcpy(user.username, username);
        user.privilege = '1';
        address = fdata.write(user);
        findex.assign(id, address);
    }

    void modify_password(char id[31], char newpasswd[31])
    {
        if (level != '7') throw 0;
        long address;
        if (!findex.find(id, address)) throw 0;
        User user;
        fdata.read(address, user);
        strcpy(user.password, newpasswd);
        fdata.modify(address, user);
    }

    void modify_password(char id[31], char curpass[31], char newpass[31])
    {
        long address;
        if (!findex.find(id, address)) throw 0;
        User user;
        fdata.read(address, user);
        if (strcmp(curpass, user.password)) throw 0;
        strcpy(user.password, newpasswd);
        fdata.modify(address, user);
    }

    void adduser(char id[31], char passwd[31], char priv, char username[31])
    {
        if (priv >= level) throw 0;
        long address;
        if (findex.find(id, address)) throw 0;
        User user;
        strcpy(user.password, passwd);
        strcpy(user.username, username);
        user.privilege = priv;
        address = fdata.write(user);
        findex.assign(id, address);
    }

    void deleteuser(char id[31])
    {
        long address;
        if (!findex.find(id, address)) throw 0;
        for (auto it = id_stack.begin(); it != id_stack.end(); it++)
            if (!strcmp(id, *it))
                throw 0;
        findex.erase(id);
    }

    // return priviledge level of current user
    char get_level()
    {
        return level;
    }

    // select certain book fot current user
    void select(char ISBN[21])
    {
        strcpy(ISBN_stack.back(), ISBN);
    }

    char *get_selected()
    {
        return ISBN_stack.back();
    }
};

# endif