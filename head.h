#ifndef _LAB02_HEAD_H
#define _LAB02_HEAD_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "math.h"
#include "symbol_table.h"
#define TABLE_SIZE 16384
#define NAME_SIZE 20
#define bool int
#define true 1
#define false 0

#define TYPE_LEN 16
#define TEXT_LEN 32
    
typedef struct TreeNode {
    	int line;               //-1表示为终结符号
    	char type[TYPE_LEN];    //属性值类型
    	char text[TEXT_LEN];    //词素
        int childnum;
        struct Type_* type_sem;
    	struct TreeNode **childlist;
}TreeNode;

extern int myatoi(char* dest);
#endif
