#ifndef _INTERCODE_H_
#define _INTERCODE_H_

#include "symbol_table.h"
#include "stdio.h"

struct Array_info{
    int var_no;
    int size;
};
struct Struct_info{
    int var_no;
    int size;
    struct Struct_info* next;
};
typedef struct Operand_* Operand;
struct Operand_ {
    enum {VARIABLE, CONSTANT, ADDRESS, TEMP, LABELOP, RELOP_OP, FUNC,SIZE,REFERENCE,ARGOP} kind;
    union {
        int var_no;
        int value;
        int temp_no;
        int label_no;
        int arg_no;
        char* relop;//relop name
        char* func_name;
        
        /*
        struct Array_info* ainfo;  //数组信息
        struct Struct_info* sinfo; //结构体信息
        */
    }u;
};

struct OperandNode {
    Operand op;
    struct OperandNode* next;
};

struct InterCode {
    enum {ASSIGN, ADD, SUB, MUL, DIVISION, LABEL, REL, GOTO, READ, CALL, WRITE, ARG, RETURN_CODE, FUNCTION,PARAM,DEC,MEM_R,MEM_L} kind;
    union {
        struct {Operand left, right;}assign;//e.g. a = b
        struct {Operand result, op1, op2;}binop;//e.r. c = a op b
        struct {Operand labelop;}label;//e.g. LABEL label
        struct {Operand op1, relop, op2, labelop;}rel;//>= <= > < == !=, e.g. IF t1 != #0 GOTO label_true
        struct {Operand labelop;}go;//e.g. GOTO label_true
        struct {Operand arg;}read;//e.g. READ place
        struct {Operand op1, op2;}call;//e.g. place = CALL function.name
        struct {Operand arg;}write;//e.g. WRITE arg_list[i]
        struct {Operand argop;}arg;
        struct {Operand returnop;}ret;//e.g. RETURN t1
        struct {Operand func_name;}function;//e.g. Function main :
        struct {Operand var;}param; //e.g. PARAM v1
        struct {Operand var, size;}dec;
        struct {Operand op, reference;}mem_r;
        struct {Operand reference, op;}mem_l;
    }u;
};

struct InterCodeNode {
    struct InterCode* code;
    struct InterCodeNode* prev, *next;
};

extern Operand new_temp();
extern Operand new_label();
extern void create_operand(Operand op,int kind, int i, char* c);
extern struct OperandNode* create_operandnode(Operand op);
extern struct OperandNode* insert_op(Operand op, struct OperandNode* arg_list);
extern struct InterCodeNode* create_node(struct InterCode* code);
extern int ComputeSize(VarNode* ptr,int count);

extern struct InterCodeNode* join_node(struct InterCodeNode* node1, struct InterCodeNode* node2);
extern struct InterCodeNode* join_node3(struct InterCodeNode* node1, struct InterCodeNode* node2, struct InterCodeNode* node3);
extern struct InterCodeNode* join_node4(struct InterCodeNode* node1, struct InterCodeNode* node2, struct InterCodeNode* node3, struct InterCodeNode* node4);
extern struct InterCodeNode* join_node5(struct InterCodeNode* node1, struct InterCodeNode* node2, struct InterCodeNode* node3, struct InterCodeNode* node4, struct InterCodeNode* node5);
extern struct InterCodeNode* join_node6(struct InterCodeNode* node1, struct InterCodeNode* node2, struct InterCodeNode* node3, struct InterCodeNode* node4, struct InterCodeNode* node5, struct InterCodeNode* node6);

extern struct InterCodeNode* translate_Exp(struct TreeNode* exp, Operand place, char* func_name);
extern struct InterCodeNode* translate_Stmt(struct TreeNode* stmt, char* func_name);
extern struct InterCodeNode* translate_CompSt(struct TreeNode* compst, char* func_name);
extern struct InterCodeNode* translate_DefList(struct TreeNode* deflist, char* func_name);
extern struct InterCodeNode* translate_StmtList(struct TreeNode* stmtlist, char* func_name);
extern struct InterCodeNode* translate_Def(struct TreeNode* def, char* func_name);
extern struct InterCodeNode* translate_DecList(struct TreeNode* declist, char* func_name);
extern struct InterCodeNode* translate_Dec(struct TreeNode* dec, char* func_name);
extern struct InterCodeNode* translate_VarDec(struct TreeNode* vardec,Operand place);
extern struct InterCodeNode* translate_FunDec(struct TreeNode* fundec);
extern struct InterCodeNode* translate_Cond(struct TreeNode* exp, Operand label_true, Operand label_false, char* func_name);
extern struct InterCodeNode* translate_Args(struct TreeNode* args, struct OperandNode** arg_list_addr, char* func_name);
extern struct InterCodeNode* translate_ExtDefList(struct TreeNode* extdeflist);
extern struct InterCodeNode* translate_ExtDef(struct TreeNode* extdef);

//Output Function
extern void fputsNode(struct InterCodeNode* head,FILE* stream);
extern void fputsCode(struct InterCode* ptr,FILE* stream);
extern void fputsOperand(Operand ptr,FILE* stream);

#endif
