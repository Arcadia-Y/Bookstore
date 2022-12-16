# Documentaion for Bookstore
_By Arcadia-Y_

## 程序功能
本程序是一个基于命令行的图书管理系统。程序主体包括以下三个部分：
- 账户系统：管理帐户及权限数据，并提供相应功能。
- 图书系统：提供图书信息、库存信息和图书交易财务记录的相关服务及数据存储功能。
- 日志系统：记录书店管理系统运行过程中的各种数据，提供各类日志信息查询服务。
  
## 主体逻辑
将各子系统封装为类，main函数读取指令后再根据指令调用相应类的相关函数。

## 代码架构
程序代码主要有以下几个类：


```cpp
template<typename value_type>
class Datafile
```
声明于datafile.h，可线性地读写信息至文件。用于存储账户信息、图书信息、日志信息等。

```cpp
template<typename key_type, typename value_type>
class Blocklist
```
声明于blocklist.h，即块状链表类，类似"std::map"，将指定类型的键值对（键值不可重复）以块状链表的结构存储于硬盘中，支持插入、删除、查询等操作。用于存储账户信息、图书信息、日志信息的索引。

```cpp
template<typename key_type, typename value_type>
class Multi_Blocklist
```
声明于multi_blocklist.h，类似"std::multimap"，与Blocklist不同之处在于键值可以重复。用于存储图书信息的索引。

```cpp
class user
```
声明于user.h，存储一个用户的用户名、密码、权限等账户信息，支持检查密码、修改密码等功能。

```cpp
class user_system
```
声明于user.h，以块状链表存储所有用户的账户信息，支持注册账户、创建账户、删除账户等功能。

```cpp
class login_stack
```
声明于user.h，即登录栈，记录账户登录状态，支持登录账户、注销账户等功能。

```cpp
class book
```
声明于book.h，存储一本书籍的信息，支持修改图书信息的功能。

```cpp
class book_system
```
声明于book.h，以块状链表存储所有图书的信息，支持检索、购买、进货等操作。

```cpp
struct transaction
```
声明于log.h，记录单笔交易的相关信息。

```cpp
class log_system
```
声明于log.h，存储财务记录及用户操作记录，支持财务记录查询和生成日志等功能。