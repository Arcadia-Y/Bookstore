# ifndef BOOK_SYSTEM_H
# define BOOK_SYSTEM_H

#include <iostream>
#include <iomanip>
#include <cstring>
#include "datafile.h"
#include "blocklist.h"
#include "multi_blocklist.h"
#include "mystring.h"

class Book_System
{
    struct Book
    {
        char bookname[61];
        char author[61];
        char keyword[61];
        double price = 0;
        int remain = 0;
    };
    Datafile<Book> fdata;
    Blocklist<string21, long> findex;
    Multi_Blocklist<string61, string21> *ISBNindex; // 0 for bookname, 1 for author, 2 for keyword
    std::string curISBN;

    // show information of a book
    void showbook(const std::string &ISBN, long address)
    {
        Book book;
        fdata.read(address, book);
        std::cout << std::fixed << std::setprecision(2);
        std::cout << ISBN << '\t' << book.bookname << '\t' << book.author << '\t' << book.keyword
        << '\t' << book.price << '\t' << book.remain << '\n';
    }

    void parse_keywords(const std::string &keywords, std::vector<std::string> &res)
    {
        res.clear();
        std::string buff;
        for (int i = 0; i < keywords.size(); i++)
        {
            if (keywords[i] == '|')
            {
                if (buff.empty()) throw 0;
                res.push_back(buff);
                buff.clear();
            }
            else
                buff.append(keywords[i]);
        }
        if (buff.empty()) throw 0;
        res.push_back(buff);
    }

public:
    Book_System():
    fdata("book_data"), findex("book_index")
    {
        ISBNindex = new  Multi_Blocklist<string61, string21>[3] 
        {{"bookname_index"}, {"author_index"}, {"keyword_index"}};
    }

    ~Book_System()
    {
        delete []ISBNindex;
    }

    // type = 0 for all books, 1 for ISBN, 2 for name, 3 for author, 4 for keyword
    void show(int type, const std::string &info)
    {
        long address;
        std::vector<string21> str_vec;
        std::string ISBN;
        switch (type)
        {
            case 0:
                findex.traverse(showbook);
                break;
            case 1:
                if (!findex.find(info, address))
                {
                    std::cout << '\n';
                    break;
                }
                showbook(info, address);
                break;
            case 2:
            case 3:
            case 4:
                ISBNindex[type-2].find(info, str_vec);
                if (str_vec.empty())
                {
                    std::cout << '\n';
                    break;
                }
                for (auto it = str_vec.begin(); it != str_vec.end(); it++)
                {
                    ISBN = it->string;
                    findex.find(it->string, address);
                    showbook(ISBN, address);
                }
        }
    }

    void select(const std::string &ISBN)
    {
        curISBN = ISBN;
        long address;
        if (!findex.find(ISBN, address))
        {
            Book book;
            book.author[0] = '\0';
            book.bookname[0] = '\0';
            book.keyword[0] = '\0';
            address = fdata.write(book);
            findex.assign(curISBN, address);
        }
    }

    void get_ISBN(std::string &ISBN)
    {
        ISBN = curISBN;
    }

    // type = 0 for price, 1 for ISBN, 2 for name, 3 for author, 4 for keyword
    void modify(const std::vector<int> &types, const std::vector<std::string> &info_vec)
    {
        long address;
        Book book;
        std::vector<std::string> keywords;
        // check ISBN and keywords
        for (int i = 0; i < types.size(); i++)
        {
            if (types[i] == 1)
                if (findex.find(info_vec[i], address))
                    throw 0;
            else if (types[i] == 4)
            {
                parse_keywords(info_vec[i], keywords);
                for (int i = 0; i < keywords.size(); i++)
                    for (int j = i+1; j < keywords.size(); j++)
                        if (keywords[i] == keywords[j])
                            throw 0; 
            }
        }
        findex.find(curISBN, address);
        fdata.read(address, book);
        for (int i = 0; i < types.size(); i++)
        {
            switch (types[i])
            {
                case 0:
                    book.price = std::stod(info_vec[i]);
                    break;
                case 1:
                    findex.erase(curISBN);
                    curISBN = info_vec[i];
                    findex.assign(curISBN, address);
                    break;
                case 2:
                    if (book.bookname[0] != '\0')
                        ISBNindex[0].erase(book.bookname, address);
                    strcpy(book.bookname, info_vec[i].c_str());
                    ISBNindex[0].insert(book.bookname, address);
                    break;
                case 3:
                    if (book.author[0] != '\0')
                        ISBNindex[1].erase(book.author, address);
                    strcpy(book.author, info_vec[i].c_str());
                    ISBNindex[1].insert(book.author, address);
                    break;
                case 4:
                    if (book.keyword[0] != '\0')
                    {
                        std::vector<std::string> curkey;
                        parse_keywords(book.keyword, curkey);
                        for (auto it = curkey.begin(); it != curkey.end(); it++)
                            ISBNindex[2].erase(*it, address);
                    }
                    strcpy(book.keyword, info_vec[i].c_str());
                    for (auto it = keywords.begin(); it != keywords.end(); it++)
                        ISBNindex[2].insert(*it, address);
            }
        }
        fdata.modify(address, book);
    }

    void buy(const std::string &ISBN, int quantity)
    {
        long address;
        if (!findex.find(ISBN, address)) throw 0;
        Book book;
        fdata.read(address, book);
        if (book.remain < quantity) throw 0;
        book.remain -= quantity;
        std::cout << std::fixed << std::setprecision(2);
        std::cout << book.price * quantity << '\n';
        fdata.modify(address. book);
    }

    void import_books(int quantity)
    {
        if (curISBN.empty()) throw 0;
        long address;
        Book book;
        findex.find(curISBN, address);
        fdata.read(address, book);
        book.remain += quantity;
        fdata.modify(address, book);
    }
};

# endif