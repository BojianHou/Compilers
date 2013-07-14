#ifndef _SYMBOL_H
#define _SYMBOL_H
#include<stdlib.h>
#include<stdio.h>
#include"head.h"
#define NAME_SIZE 20
#define TABLE_SIZE 16384
#define STACK_SIZE 10
#define bool int 
#define true 1
#define false 0
#define INSTRUCT 1
#define NOTINSTRUCT 2

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;

typedef struct VarNode{
    char name[NAME_SIZE];
    Type type;
    int var_no;
    struct VarNode* next;
    struct VarNode* snext;
}VarNode;

struct Type_{
    enum {basic,array,structure} kind;  //数据类型
    union{
        int basic_info;   //基本类型：**int/float**int is 0**float is 1
        struct {Type elem;int size;} array;  //数组类型
        VarNode* structure;   //结构类型
    }type_info;
};

/*struct FieldList_{
    char* name;
    Type type;
    FieldList next;
};*/



typedef struct FuncNode{
    char name[NAME_SIZE];
    Type returnType;
    VarNode* paralist;
    struct FuncNode* next;
}FuncNode;

typedef struct StructNode{
    char name[NAME_SIZE];
    Type type;
    struct StructNode* next;
}StructNode;

struct TreeNode;

struct VarNode* VarTable[TABLE_SIZE];
struct FuncNode* FuncTable[TABLE_SIZE];
struct StructNode* StructTable[TABLE_SIZE];
struct VarNode* stack[STACK_SIZE];
int top;

extern unsigned int hash_pjw(char* name);
extern int myatoi(char* dest);
extern int JudgeOperand(char* type);

extern void InitialVarTable();
extern void CreateVarNode(VarNode* var,char *name,Type type);
extern void InsertVarNode(VarNode* var);
extern VarNode* SearchVar(char *name);

extern void InitialFuncTable();
extern void CreateFuncNode(FuncNode* func,char *name,Type returnType,VarNode* paralist);
extern bool InsertFuncNode(FuncNode* func);
extern VarNode* InsertParaList(VarNode* paralist,VarNode* var,bool* flag);
extern FuncNode* SearchFunc(char *name);
extern VarNode* SearchPar(char* name,char* func_name);

extern void InitialStructTable();
extern void CreateStructNode(StructNode* stru,char *name,Type type);
extern bool InsertStructNode(StructNode* stru);
extern StructNode* SearchStruct(char *name);

extern void InitialStack();
extern bool InsertStack(VarNode* var);
extern void Delete_Stack_VarTable();

extern int get_dimension(Type type);
extern bool is_equal_type(Type type1, Type type2);

extern Type Specifier_analyse(struct TreeNode *specifier,char* func_name);
extern void ExtDecList_analyse(struct TreeNode *ext_dec_list,Type type,char* func_name);
extern void VarDec_analyse(VarNode* var,struct TreeNode *var_dec,Type type);
extern void FunDec_analyse(struct TreeNode *fun_dec,Type type,char* func_name);
extern VarNode* VarList_analyse(struct TreeNode *varlist,VarNode* paralist,char* func_name);
extern void ParamDec_analyse(VarNode* var,struct TreeNode *param_dec,char* func_name);
extern void CompSt_analyse(struct TreeNode *comp_st, char* func_name);
extern void DefList_analyse(struct TreeNode *def_list,char* func_namei,int error_type);
extern void StmtList_analyse(struct TreeNode *stmt_list, char* func_name);
extern void Stmt_analyse(struct TreeNode *stmt_list, char* func_name);
extern void DecList_analyse(struct TreeNode *dec_list,Type type,char* func_name,int error_type);
extern void Dec_analyse(struct TreeNode *dec,Type type,char* func_name,int error_type);
extern void ExtDef_analyse(struct TreeNode *ext_def,char* func_name);
extern void Def_analyse(struct TreeNode *def,char* func_name,int error_type);
extern Type Exp_analyse(struct TreeNode *exp,char* func_name);
extern bool Args_analyse(struct TreeNode *args,VarNode* var,char* func_name);
#endif
