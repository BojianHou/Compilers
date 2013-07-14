#include <stdio.h>
#include "head.h"
#include "symbol_table.h"
#include "mipscode.h"
#define bool int
#define true 1
#define false 0
extern struct TreeNode* root;
extern struct InterCodeNode* head;
extern struct VarNode* VarTable[TABLE_SIZE];
extern struct FuncNode* FuncTable[TABLE_SIZE];
extern struct StructNode* StructTable[TABLE_SIZE];
extern struct VarNode* stack[STACK_SIZE];
extern struct InterCodeNode* generateIntercode(struct TreeNode* ptr);
extern int yylineno;
extern bool isWrong;
extern int myatoi(char* dest);
int main(int argc,char** argv){
    int i;
    if(argc <= 1){
        printf("Wrong Input!\n");
        printf("Usage:./parser + filename\n");
        return 1;
        
    }
    for(i = 1;i < argc; i++){
        FILE* f = fopen(argv[i],"r");
        if(!f){
            perror(argv[i]);
            return 1;
        }
        //FILE* out1 = fopen("code.ir","w");
        FILE* out2 = fopen("out2.s","w");
        InitialVarTable();
        InitialFuncTable();
        InitialStructTable();
        InitialStack();
        root = NULL;
        head = NULL;
        yylineno = 1;
        isWrong = false;
        yyrestart(f);
        yyparse();
        if(root != NULL && isWrong == false)
       //     displayTree(root,0);
        analyseTree(root);
        head = generateIntercode(root);
        //fputsNode(head,out1);
        outputMipsCode(out2,head);
        fclose(f);
    }
    return 0;
}

