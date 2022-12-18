# Documentaion for Bookstore
_By Arcadia-Y_

## 程序功能
本程序是一个基于命令行的图书管理系统。程序主体包括以下三个部分：
- 账户系统：管理帐户及权限数据，并提供相应功能。
- 图书系统：提供图书信息、库存信息和图书交易财务记录的相关服务及数据存储功能。
- 日志系统：记录书店管理系统运行过程中的各种数据，提供各类日志信息查询服务。
  
## 主体逻辑
将各子系统封装为类，由Scanner类读取并解析指令后再根据指令调用相应类的相关函数。

## 代码架构
下面将简介本程序主要的类及其对外接口。

```cpp
template<typename value_type>
class Datafile
{
    Datafile(const char *name);

    // write a new value into data file, return address
    long write(const value_type &value);

    // modify existd data
    void modify(long address, const value_type &value);

    // read data
    void read(long address, value_type &value)；
}
```
声明于datafile.h，可线性地读写信息至文件。用于存储账户信息、图书信息、日志信息等。

```cpp
template<typename key_type, typename value_type>
class Blocklist
{
    Blocklist(const char *name);

    // assign the value to the key
    void assign(const key_type &key, const value_type &value);

    // find the key and store the value in res, return false if key not found
    bool find(const key_type &key, value_type &res);

    // erase the key, return false if key not found
    bool erase(const key_type &key);

    // traverse the blocklist and execute func(key, value) 
    void traverse(void (*func)(const key_type key, const value_type value));
}
```
声明于blocklist.h，即块状链表类，类似"std::map"，将指定类型的键值对（键值不可重复）以块状链表的结构存储于硬盘中，支持插入、删除、查询、遍历等操作。用于存储账户信息、图书信息、日志信息的索引。

```cpp
template<typename key_type, typename value_type>
class Multi_Blocklist
{
    Multi_Blocklist(const char *name);

    // insert certain element, do nothing if element already exists
    void insert(const key_type &key, const value_type &value);

    // return all values with the key in ascending order
    std::vector<value_type> find(const key_type &key);

    // erase certain element, return false if element not found
    bool erase(const key_type &key, const value_type &value);
}
```
声明于multi_blocklist.h，类似"std::multimap"，与Blocklist不同之处在于键值可以重复。用于存储图书信息的索引。

```cpp
struct User
{
    char password[31];
    char username[31];
    char privilege;
}
```
声明于user_system.h，存储一个用户的用户名、密码、权限等账户信息。

```cpp
class User_System
{
    User_System();

    void login(const std::string &id);
    void login(const std::string &id, const std::string &passwd);

    void logout();

    // register
    void rgs(const std::string &id, const std::string &passwd, const std::string &username);

    void modify_password(const std::string &id, const std::string &newpasswd);
    void modify_password(const std::string &id, const std::string & curpass, const std::string &newpass);

    void adduser(const std::string &id, const std::string &passwd, const std::string priv, const std::string &username);

    void deleteuser(const std::string &id);

    // return priviledge level of current user
    char get_level(); 

    // select certain book fot current user
    void select(const std::string &ISBN);
}
```
声明于user_system.h，存储所有用户的账户信息，支持注册账户、创建账户、删除账户等功能；同时维护登录栈，支持登录账户、注销账户等功能。

```cpp
struct Book
{
    char bookname[61];
    char author[61];
    char keyword[61];
    double price;
    int remain;
}
```
声明于book_system.h，存储一本书籍的信息除ISBN外的信息。

```cpp
class Book_System
{ 
    Book_System();

    // type = 0 for all books, 1 for ISBN, 2 for name, 3 for author, 4 for keyword
    void show(int type, const std::string &info);

    void select(const std::string &ISBN);

    // type = 0 for price, 1 for ISBN, 2 for name, 3 for author, 4 for keyword
    void modify(int type, const std::string info);

    void buy(const std::string &ISBN, int quantity);

    void import_books(int quantity, double totalcost);
}
```
声明于book_system.h，存储所有图书信息，支持检索、购买、进货、选择图书、修改图书信息等操作。

```cpp
class Log_System
{
    Log_System();

    // show finance, 0 for all
    void show(int count);

    // record transaction
    void record(double amount);
}
```
声明于log_system.h，存储财务记录及用户操作记录，支持财务记录查询和生成日志等功能。

```cpp
class Scanner
{
    Scanner();

    // parse a line
    void parseline(const std::string &line);

    // execute the parsed line
    void execute();
}
```
声明于scanner.h，可解析输入的语句并据此调用对应的函数。