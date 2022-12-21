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
    Blocklist<mystring<31>, long> findex;
    std::vector<std::string> id_stack;
    std::vector<std::string> ISBN_stack;
    std::vector<char> level_stack;

public:
    User_System():
    fdata("user_data"), findex("user_data_index")
    { 
        if (fdata.end() == sizeof(long))
        {
            User user;
            strcpy(user.password, "sjtu");
            strcpy(user.username, "Administrator");
            user.privilege = '7';
            long address = fdata.write(user);
            std::string userid = "root";
            findex.assign(userid, address);
        }
    }

    void login(const std::string &id)
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

    void login(const std::string &id, const std::string &passwd)
    {
        long address;
        if (!findex.find(id, address)) throw 0;
        User user;
        fdata.read(address, user);
        if (strcmp(passwd.c_str(), user.password)) throw 0;
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
    void rgs(const std::string &id, const std::string &passwd, const std::string &username)
    {
        long address;
        if (findex.find(id, address)) throw 0;
        User user;
        strcpy(user.password, passwd.c_str());
        strcpy(user.username, username.c_str());
        user.privilege = '1';
        address = fdata.write(user);
        findex.assign(id, address);
    }

    void modify_password(const std::string &id, const std::string &newpasswd)
    {
        long address;
        if (!findex.find(id, address)) throw 0;
        User user;
        fdata.read(address, user);
        strcpy(user.password, newpasswd.c_str());
        fdata.modify(address, user);
    }

    void modify_password(const std::string &id, const std::string &curpass, const std::string &newpass)
    {
        long address;
        if (!findex.find(id, address)) throw 0;
        User user;
        fdata.read(address, user);
        if (strcmp(curpass.c_str(), user.password)) throw 0;
        strcpy(user.password, newpass.c_str());
        fdata.modify(address, user);
    }

    void adduser(const std::string &id, const std::string &passwd, char priv, const std::string &username)
    {
        if (priv >= level) throw 0;
        long address;
        if (findex.find(id, address)) throw 0;
        User user;
        strcpy(user.password, passwd.c_str());
        strcpy(user.username, username.c_str());
        user.privilege = priv;
        address = fdata.write(user);
        findex.assign(id, address);
    }

    void deleteuser(const std::string &id)
    {
        long address;
        if (!findex.find(id, address)) throw 0;
        for (auto it = id_stack.begin(); it != id_stack.end(); it++)
            if (id == *it)
                throw 0;
        findex.erase(id);
    }

    // return priviledge level of current user
    char get_level()
    {
        return level;
    }

    // select certain book fot current user
    void select(const std::string &ISBN)
    {
        if (ISBN_stack.empty()) return;
        ISBN_stack.back() = ISBN;
    }

    void get_ISBN(std::string &ISBN)
    {
        if (ISBN_stack.empty()) ISBN.clear();
        else ISBN = ISBN_stack.back();
    }

    void update_ISBN(const std::string &newISBN)
    {
        std::string curISBN = ISBN_stack.back();
        for (int i = 0; i < ISBN_stack.size(); i++)
            if (ISBN_stack[i] == curISBN)
                ISBN_stack[i] = newISBN;
    }
};

# endif