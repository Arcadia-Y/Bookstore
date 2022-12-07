# ifndef BLOCKLIST_H
# define BLOCKLIST_H

#define BLOCK_SIZE 300

template<typename Element>
class blocklist
{
    struct blocknode
    {
        blocknode* pre;
        blocknode* next;

        Element values[BLOCK_SIZE];

        int size;
        Element Min;
        Element Max;
    };
    
};
# endif