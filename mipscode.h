#ifndef _TARGET_CODE_H
#define _TARGET_CODE_H

#include "intercode.h"

struct OffsetNode{
    Operand op;
    int offset;
    struct OffsetNode* next;
};

extern struct OffsetNode* offset_head; 
extern struct OffsetNode* offset_tail; 

extern void outputMipsCode(FILE* stream,struct InterCodeNode* ptr);
extern void preMips(FILE* stream);
extern void fputsMips(FILE* stream,struct InterCode* ptr);

extern void load(FILE* stream,Operand op,int num);
extern void store(FILE* stream,Operand op,int num);

extern void InitOffset(struct InterCodeNode* ptr);
extern void InitOffsetNode(struct InterCode* ptr);
extern int getOffset(Operand op);
extern void InsertOffset(Operand op,int size);

extern int isEqual(Operand op1,Operand op2);
#endif
