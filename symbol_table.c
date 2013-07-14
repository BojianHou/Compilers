#include "symbol_table.h"
#include "head.h"
extern bool isWrong;
extern int yylineno;

/*******************Function Definiton*************************/
unsigned int hash_pjw(char* name){
    unsigned int val = 0,i;
    for(; *name; ++name){
        val = (val << 2) + *name;
        if(i = val & ~0x3fff)
            val = (val ^ (i >> 12)) & 0x3fff;
    }
    return val;
}

void InitialVarTable(){
    int i;
    for(i = 0;i < TABLE_SIZE;i ++)
        VarTable[i] = NULL;
}

void CreateVarNode(VarNode* var,char* name,Type type){
    strcpy(var->name,name);
    var->type = type;
    var->var_no = 0;
    var->next = NULL;
}

void InsertVarNode(VarNode* var){
    unsigned int position = hash_pjw(var->name);
    VarNode* head = VarTable[position];
    var->next = VarTable[position];
    VarTable[position] = var; 
}

VarNode* SearchVar(char *name){
    unsigned int position = hash_pjw(name);
    VarNode* head = VarTable[position];
    while(head != NULL){
        if(strcmp(name,head->name) == 0){
            break;
        }
        head = head->next;
    }
    return head;
}

void InitialFuncTable(){
    int i;
    for(i = 0;i < TABLE_SIZE;i ++)
        FuncTable[i] = NULL;
    /************************* READ WRITE *******************************/
    FuncNode* write = (FuncNode *)malloc(sizeof(struct FuncNode));
    Type returnType1 = (Type)malloc(sizeof(struct Type_));
    returnType1->kind = basic;
    returnType1->type_info.basic_info = 0; 
    VarNode* para = (VarNode *)malloc(sizeof(struct VarNode));
    para->type = (Type)malloc(sizeof(struct Type_));
    para->type->kind = basic;
    para->type->type_info.basic_info = 0;
    CreateFuncNode(write,"write",returnType1,para);
    InsertFuncNode(write);
    
    FuncNode* read = (FuncNode *)malloc(sizeof(struct FuncNode));
    Type returnType2 = (Type)malloc(sizeof(struct Type_));
    returnType1->type_info.basic_info = 0; 
    CreateFuncNode(read,"read",returnType2,NULL); 
    InsertFuncNode(read);
}

void CreateFuncNode(FuncNode* func,char* name,Type returnType,VarNode* paralist){
    strcpy(func->name,name);
    func->returnType = returnType;
    func->paralist = paralist;
    func->next = NULL;
}

bool InsertFuncNode(FuncNode* func){
    unsigned int position = hash_pjw(func->name);
    FuncNode* head = FuncTable[position];
    while(head != NULL){
        if(strcmp(head->name,func->name) == 0)
            break;
        head = head->next;
    }
    if(head == NULL){
        func->next = FuncTable[position];
        FuncTable[position] = func;
        return true;
    }
    return false;
}

VarNode* InsertParaList(VarNode* paralist,VarNode* var,bool* flag){
    VarNode* head = paralist;
    VarNode* head_pre = paralist;
    if(paralist == NULL){
        paralist = var;
        paralist->next = NULL;
        *flag = true;
        return paralist;
    }
    while(head != NULL){
        if(strcmp(head->name,var->name) == 0)
            break;
        head_pre = head;
        head = head->next;
    }
    if(head == NULL){
        head_pre->next = var;
        var->next = NULL;
        *flag = true;
        return paralist;
    }
    *flag = false;
    return paralist;
}

FuncNode* SearchFunc(char* name){
    unsigned int position = hash_pjw(name);
    FuncNode* head = FuncTable[position];
    while(head != NULL){
        if(strcmp(name,head->name) == 0)
            break;
        head = head->next;
    }
    return head;
}

VarNode* SearchPar(char* name,char* func_name){
    FuncNode* ptr = SearchFunc(func_name);
    if(ptr == NULL)
        return NULL;
    VarNode* head = ptr->paralist;
    while(head != NULL){
        if(strcmp(name,head->name) == 0)
            break;
        head = head->next;
    }
    return head;
}

void InitialStructTable(){
    int i;
    for(i = 0;i < TABLE_SIZE;i ++)
        StructTable[i] = NULL;
}

void CreateStructNode(StructNode* stru,char* name,Type type){
    strcpy(stru->name,name);
    stru->type = type;
    stru->next = NULL;
}

bool InsertStructNode(StructNode* stru){
    unsigned int position = hash_pjw(stru->name);
    StructNode* head1 = StructTable[position];
    VarNode* head2 = VarTable[position];
    FuncNode* head3 = FuncTable[position];
    while(head1 != NULL){
        if(strcmp(head1->name,stru->name) == 0)
            break;
        head1 = head1->next;
    }
    while(head2 != NULL){
        if(strcmp(head2->name,stru->name) == 0)
            break;
        head2 = head2->next;
    }
    while(head3 != NULL){
        if(strcmp(head3->name,stru->name) == 0)
            break;
        head3 = head3->next;
    }
    if(head1 == NULL && head2 == NULL && head3 == NULL){
        stru->next = StructTable[position];
        StructTable[position] = stru;
        return true;
    }
    return false; 
}

StructNode* SearchStruct(char *name){
    unsigned int position = hash_pjw(name);
    StructNode* head = StructTable[position];
    while(head != NULL){
        if(strcmp(name,head->name) == 0)
            break;
        head = head->next;
    }
    return head;
}

void InitialStack(){
    int i;
    for(i = 0;i < STACK_SIZE;i ++)
        stack[i] = NULL;
    top = -1;
}

bool InsertStack(VarNode* var){
    VarNode* temp = stack[top];
    while(temp != NULL){
        if(strcmp(temp->name,var->name) == 0){
            break;
        }
        temp = temp->snext;
    }
    if(temp == NULL){
        temp = stack[top];
        if(temp == NULL)
            stack[top] = var;
        else{
            while(temp->snext != NULL)
                temp = temp->snext;
            temp->snext = var;
            var->snext = NULL;
        }
        return true;
    }
    else{
        return false;
    }
}

void Delete_Stack_VarTable(){
    VarNode* ptr = stack[top];
    int position;
    while(ptr != NULL){
        VarNode* temp = ptr;
        ptr = ptr->snext;
        position = hash_pjw(temp->name);
        VarNode* temp2 = VarTable[position];
        VarTable[position] = VarTable[position]->next;
        free(temp);
    }
    stack[top] = NULL;
}

int get_dimension(Type type)
{
    int dim = 0;
    while (type->kind == array)
    {
        dim++;
        type = type->type_info.array.elem;
    }
    return dim;
}

bool is_equal_type(Type type1, Type type2) {
    int i;
    if (type1 == NULL || type2 == NULL)
        return false;

    if (type1->kind == basic && type2->kind == basic)
    {

        if (type1->type_info.basic_info == 0 && type2->type_info.basic_info == 0) 
            return true;
        else if (type1->type_info.basic_info == 1 && type2->type_info.basic_info == 1)
            return true;
    }
    else if(type1->kind == array && type2->kind == array)
    {

        Type s1 = type1;
        Type s2 = type2;
        while (s1->kind == array)
            s1 = s1->type_info.array.elem;
        while (s2->kind == array)
            s2 = s2->type_info.array.elem;
        if (get_dimension(type1) == get_dimension(type2) && is_equal_type(s1,s2) == true)
            return true;

        return false;
    }
    else if (type1->kind == structure && type2->kind == structure) {
        return (type1 == type2);
    }
    return false;
}

Type Specifier_analyse(struct TreeNode *specifier,char* func_name) {
    int i;
    Type type = (Type)malloc(sizeof(struct Type_));
    if (strcmp(specifier->childlist[0]->type, "TYPE") == 0) {
        type->kind = basic;
        if (strcmp(specifier->childlist[0]->text, "int") == 0) {
            type->type_info.basic_info = 0;
        }
        else if (strcmp(specifier->childlist[0]->text, "float") == 0) {
            type->type_info.basic_info = 1;
        }
        return type;
    }
    else if (strcmp(specifier->childlist[0]->type, "StructSpecifier") == 0) {
        char *structname;
        StructNode* stru = (StructNode *)malloc(sizeof(StructNode));
        struct TreeNode *struct_specifier = specifier->childlist[0];
        type->kind = structure;
        if (strcmp(struct_specifier->childlist[1]->type, "OptTag") == 0) {
            if (struct_specifier->childlist[1] == NULL) {
                structname = NULL;
            }
            else {
                structname = struct_specifier->childlist[1]->childlist[0]->text;
            }
            for (i = 0; i < struct_specifier->childnum; i++) {
                if (struct_specifier->childlist[i] == NULL);
                else if (strcmp(struct_specifier->childlist[i]->type, "LC") == 0) {
                    top++;
                }
                else if (strcmp(struct_specifier->childlist[i]->type, "DefList") == 0) {
                    DefList_analyse(struct_specifier->childlist[i],func_name,INSTRUCT);
                    type->type_info.structure = stack[top];
                    stack[top] = NULL;
                    CreateStructNode(stru,structname,type);
                    if(InsertStructNode(stru) == false){
                        printf("Error type 16 at line %d: Duplicate name \"%s\"\n", struct_specifier->line, stru->name);
                        return NULL;
                    }
                }
                else if (strcmp(struct_specifier->childlist[i]->type, "RC") == 0) {
                    top--;
                }
            }
        }
        else if (strcmp(struct_specifier->childlist[1]->type, "Tag") == 0) {
            structname = struct_specifier->childlist[1]->childlist[0]->text;
            StructNode* sNode = SearchStruct(structname);
            if (sNode == NULL) {
                printf("Error type 17 at line %d: Undefined struct \"%s\"\n", struct_specifier->line, structname);
                return NULL;
            }
            else{
                type->type_info.structure = sNode->type->type_info.structure;
            }
        }
        return type;
    }
    return NULL;
}

void ExtDecList_analyse(struct TreeNode *ext_dec_list,Type type,char* func_name){
    int i;
    for(i = 0;i < ext_dec_list->childnum;i ++){
        if(strcmp(ext_dec_list->childlist[i]->type,"VarDec") == 0){
            VarNode *var = (VarNode *)malloc(sizeof(VarNode));
            VarDec_analyse(var,ext_dec_list->childlist[i],type);
            if(InsertStack(var) == false){
                printf("Error type 3 at line %d: Redefinition of variable \"%s\"\n", ext_dec_list->childlist[0]->line, var->name);
            }
            else{
                InsertVarNode(var);
            }
        }
        else if(strcmp(ext_dec_list->childlist[i]->type,"ExtDecList") == 0)
            ExtDef_analyse(ext_dec_list->childlist[i],func_name);
    } 
}

void VarDec_analyse(VarNode* var,struct TreeNode *var_dec,Type type){
    if(strcmp(var_dec->childlist[0]->type,"ID") == 0){
        CreateVarNode(var,var_dec->childlist[0]->text,type);
        return;
    }
    else{
        Type arrayType = (Type)malloc(sizeof(struct Type_));
        arrayType->kind = array;
        arrayType->type_info.array.size = myatoi(var_dec->childlist[2]->text);
        arrayType->type_info.array.elem = type;
        VarDec_analyse(var,var_dec->childlist[0],arrayType);
        return;
    }
}

void FunDec_analyse(struct TreeNode* fun_dec,Type returnType,char* func_name){
    int i;
    FuncNode* func = (FuncNode *)malloc(sizeof(FuncNode));
    char *name = fun_dec->childlist[0]->text;
    strcpy(func_name,name);
    if(strcmp(fun_dec->childlist[2]->type,"VarList") == 0){
        func->paralist = VarList_analyse(fun_dec->childlist[2],func->paralist,func_name);
    }
    else
        func->paralist = NULL;
    CreateFuncNode(func,name,returnType,func->paralist);
    if(InsertFuncNode(func) == false){
        printf("Error type 4 at line %d: Redefinition of function \"%s\"\n", fun_dec->line, func->name);
    }
}

VarNode* VarList_analyse(struct TreeNode* varlist,VarNode* paralist,char* func_name){
    int i;
    bool flag = true;
    for(i = 0;i < varlist->childnum;i ++){
        if(strcmp(varlist->childlist[i]->type,"ParamDec") == 0){
            VarNode *var = (VarNode *)malloc(sizeof(VarNode));
            ParamDec_analyse(var,varlist->childlist[i],func_name);
            paralist = InsertParaList(paralist,var,&flag);
            if(flag == false){
                printf("Undefined type at line %d: redefinition of Paralist \"%s\"\n",varlist->childlist[i]->line,var->name);
            }

        }
        else if(strcmp(varlist->childlist[i]->type,"VarList") == 0){
            paralist = VarList_analyse(varlist->childlist[i],paralist,func_name);
        }    
    } 
    return paralist;
}

void ParamDec_analyse(VarNode* var,struct TreeNode *param_dec,char* func_name){
    Type type = Specifier_analyse(param_dec->childlist[0],func_name);
    VarDec_analyse(var,param_dec->childlist[1],type);
}

void ExtDef_analyse(struct TreeNode *ext_def,char* func_name){
    int i;
    struct TreeNode* child0 = ext_def->childlist[0];
    if(strcmp(child0->type,"Specifier") == 0){
        /**************analyse first child*******************/
        Type type = Specifier_analyse(child0,func_name);
        if (type == NULL)
            return;
        struct TreeNode* child1 = ext_def->childlist[1];
        /**************analyse second child*******************/
        if(strcmp(child1->type, "ExtDecList") == 0) {
            ExtDecList_analyse(child1,type,func_name);
            return;
        }
        else if(strcmp(child1->type,"SEMI") == 0){
            return;    
        }
        else if(strcmp(child1->type,"FunDec") == 0){
            FunDec_analyse(child1,type,func_name);
            /**************analyse third child*******************/
            struct TreeNode* child2 = ext_def->childlist[2];
            if(strcmp(child2->type,"CompSt") == 0){
                CompSt_analyse(child2, child1->childlist[0]->text);
            }
        }
    }
}

void CompSt_analyse(struct TreeNode *comp_st, char* func_name){
    int i;
    for(i = 0;i < comp_st->childnum;i ++){
        if(comp_st->childlist[i] == NULL);     //Empty Sentence
        else if(strcmp(comp_st->childlist[i]->type,"LC") == 0){
            top ++;
        }
        else if(strcmp(comp_st->childlist[i]->type,"DefList") == 0){
            DefList_analyse(comp_st->childlist[i],func_name,NOTINSTRUCT);
        }
        else if(strcmp(comp_st->childlist[i]->type,"StmtList") == 0){
            StmtList_analyse(comp_st->childlist[i], func_name);
        }
        else if(strcmp(comp_st->childlist[i]->type,"RC") == 0){
            //Delete_Stack_VarTable();
            top --;
        }
    }

}

void DefList_analyse(struct TreeNode *def_list,char* func_name,int error_type){
    int i;
    for(i = 0;i < def_list->childnum;i ++){
        if(def_list->childlist[i] == NULL);   //Empty Sentence
        else if(strcmp(def_list->childlist[i]->type,"Def") == 0){
            Def_analyse(def_list->childlist[i],func_name,error_type);
        }
        else if(strcmp(def_list->childlist[i]->type,"DefList") == 0){
            DefList_analyse(def_list->childlist[i],func_name,error_type);
        }
    }
}

void StmtList_analyse(struct TreeNode *stmt_list, char* func_name){
    int i;
    for(i = 0;i < stmt_list->childnum;i ++){
        if(stmt_list->childlist[i] == NULL);    //Empty Sentence
        else if(strcmp(stmt_list->childlist[i]->type,"Stmt") == 0){
            Stmt_analyse(stmt_list->childlist[i], func_name);
        }
        else if(strcmp(stmt_list->childlist[i]->type,"StmtList") == 0){
            StmtList_analyse(stmt_list->childlist[i], func_name);
        }
    }
}

void Stmt_analyse(struct TreeNode *stmt, char* func_name){
    int i;
    for(i = 0;i < stmt->childnum;i ++){
        if(strcmp(stmt->childlist[i]->type,"Exp") == 0){
            Exp_analyse(stmt->childlist[i],func_name);
        }
        else if(strcmp(stmt->childlist[i]->type,"CompSt") == 0){
            CompSt_analyse(stmt->childlist[i], func_name);
        }
        else if(strcmp(stmt->childlist[i]->type,"Stmt") == 0){
            Stmt_analyse(stmt->childlist[i], func_name);
        }
        else if(strcmp(stmt->childlist[i]->type,"RETURN") == 0) {
            Type type = Exp_analyse(stmt->childlist[1],func_name);
            Type return_type = SearchFunc(func_name)->returnType;
            if (is_equal_type(type, return_type) == false) {
                printf("Error type 8 at line %d: The return type mismatched\n", stmt->line);
                return;
            }
            return;
        }
        else if(strcmp(stmt->childlist[i]->type,"IF") == 0) {
            if(stmt->childnum == 5) {
                Exp_analyse(stmt->childlist[2],func_name);
                Stmt_analyse(stmt->childlist[4], func_name);
                return;
            }
            else {
                Exp_analyse(stmt->childlist[2],func_name);
                Stmt_analyse(stmt->childlist[4], func_name);
                Stmt_analyse(stmt->childlist[6], func_name);
                return;
            }
        }
        else if(strcmp(stmt->childlist[i]->type,"WHILE") == 0) {
            Exp_analyse(stmt->childlist[2],func_name);
            Stmt_analyse(stmt->childlist[4], func_name);
            return;
        }
    }
}

void DecList_analyse(struct TreeNode *dec_list,Type type,char* func_name,int error_type){
    int i;
    for(i = 0;i < dec_list->childnum;i ++){
        if(strcmp(dec_list->childlist[i]->type,"Dec") == 0){
            Dec_analyse(dec_list->childlist[i],type,func_name,error_type);
        }
        else if(strcmp(dec_list->childlist[i]->type,"DecList") == 0){
            DecList_analyse(dec_list->childlist[i],type,func_name,error_type);
        }    
    } 
}

void Dec_analyse(struct TreeNode *dec,Type type,char* func_name,int error_type){
    int i;
    for(i = 0;i < dec->childnum;i ++){
        if(strcmp(dec->childlist[i]->type,"VarDec") == 0){
            VarNode* var = (VarNode*)malloc(sizeof(VarNode));
            VarDec_analyse(var,dec->childlist[i],type);
            if(InsertStack(var) == false){
                if(error_type == INSTRUCT)
                    printf("Error type 15 at line %d: Redefined field \"%s\"\n",dec->line,var->name);
                else
                    printf("Error type 3 at line %d: Redefined variable \"%s\"\n",dec->line,var->name);
            }
            else{
                InsertVarNode(var);
            }
        }
        else if(strcmp(dec->childlist[i]->type,"Exp") == 0){
            if(error_type == INSTRUCT){
                printf("Error type 15 at line %d: Assignment in STRUCT\n",dec->line);
                return;
            }
            Type type_exp = Exp_analyse(dec->childlist[i],func_name);
            if(is_equal_type(type_exp, type) == false)
                printf("Error type 5 at line %d: Type mismatched\n",dec->line);
        }
    }
}

void Def_analyse(struct TreeNode *def,char* func_name,int error_type) {
    Type type = Specifier_analyse(def->childlist[0],func_name);
    if(type == NULL)
        return;
    else{
        DecList_analyse(def->childlist[1],type,func_name,error_type);
        return;
    }
}

Type Exp_analyse(struct TreeNode *exp,char* func_name) {
    if(exp->childnum == 1){
        if(strcmp(exp->childlist[0]->type,"ID") == 0){
            //先在Parlist进行查找，然后去VarTable进行查找
            VarNode* var = SearchPar(exp->childlist[0]->text,func_name);
            if(var == NULL){
                var = SearchVar(exp->childlist[0]->text);
                if(var == NULL){
                    printf("Error type 1 at line %d: Undefined variable \"%s\"\n",exp->line,exp->childlist[0]->text);
                    return NULL;
                }
            }
            return var->type;
        }
        else if(strcmp(exp->childlist[0]->type,"INT") == 0){
            Type type = (Type)malloc(sizeof(struct Type_));
            type->kind = basic;
            type->type_info.basic_info = 0;
            return type;
        }
        else if(strcmp(exp->childlist[0]->type,"FLOAT") == 0){
            Type type = (Type)malloc(sizeof(struct Type_));
            type->kind = basic;
            type->type_info.basic_info = 1;
            return type;
        }
    }
    else if(exp->childnum == 2){
        Type type = Exp_analyse(exp->childlist[1],func_name);
        if(type == NULL)
            return NULL;
        if(strcmp(exp->childlist[0]->type,"MINUS") == 0){
            if(type->kind != basic)
                printf("Error type 7 at line %d: Operands type mismatched\n",exp->childlist[1]->line);
        }
        else{
            if(type->kind != basic || type->type_info.basic_info != 0)
                printf("Error type 7 at line %d: Operands type mismatched\n",exp->childlist[1]->line);
        }
        return type;
    }
    else if(exp->childnum == 3){
        Type type1 = NULL,type2 = NULL;
        if(strcmp(exp->childlist[1]->type,"LP") != 0){
            if(strcmp(exp->childlist[0]->type,"LP") == 0) //Exp ---> (Exp)
                return Exp_analyse(exp->childlist[1],func_name);
            //Exp ---> ID () 之外的情况
            type1 = Exp_analyse(exp->childlist[0],func_name);
            type2 = NULL;
            if (strcmp(exp->childlist[2]->type, "Exp") == 0) {
                type2 = Exp_analyse(exp->childlist[2],func_name);
            }
            if(type1 == NULL || type2 == NULL && strcmp(exp->childlist[1]->type,"DOT") != 0) {
                return NULL;
            }
        }
        if(JudgeOperand(exp->childlist[1]->type) == 0){
            bool isRight = false; 
            //ID
            if(exp->childlist[0]->childnum == 1 && strcmp(exp->childlist[0]->childlist[0]->type,"ID") == 0){
                isRight = true;
            }
            else if(exp->childlist[0]->childnum == 3){
                //(Exp)
                if(strcmp(exp->childlist[0]->childlist[1]->type,"Exp") == 0)
                    isRight = true;
                //Exp.ID
                else if(strcmp(exp->childlist[0]->childlist[1]->type,"DOT") == 0){
                    isRight = true;
                }
            }
            //Exp[Exp]
            else if(exp->childlist[0]->childnum == 4 && strcmp(exp->childlist[0]->childlist[1]->type,"LB") == 0){
                isRight = true;
            }
            if(!isRight){
                printf("Error type 6 at line %d: The left-hand side of an assignment must be a variable\n",exp->line);
                return NULL;
            }
            if(is_equal_type(type1,type2) == false) {
                printf("Error type 5 at line %d: Type mismatched\n",exp->line);
                return NULL;
            }
            return type1;
        }
        else if(JudgeOperand(exp->childlist[1]->type) == 1){
            if(type1->kind != basic || type2->kind != basic) {
                printf("Error type 7 at line %d: Operands type mismatched\n",exp->line);
                return NULL;
            }
            else if(type1->type_info.basic_info != 0 || type2->type_info.basic_info != 0){
                printf("Error type 7 at line %d: Operands type mismatched\n",exp->line);
                return NULL;
            }
            else
                return type1;
        }
        else if(JudgeOperand(exp->childlist[1]->type) == 2){
            if (type1->kind != basic || type2->kind != basic) {
                printf("Error type 7 at line %d: Operands type mismatched\n",exp->line);
                return NULL;
            }
            else if (type1->type_info.basic_info != type2->type_info.basic_info) {
                printf("Error type 7 at line %d: Operands type mismatched\n",exp->line);
                return NULL;
            }
            else{
                return type1;
            }
        }
        else if(JudgeOperand(exp->childlist[1]->type) == 4){
            if (type1->kind != structure) {
                printf("Error type 13 at line %d: Illegal use of \".\"\n", exp->line);
                //    return NULL;
            }
            else {
                VarNode* ptr;
                char* name;
                if(exp->childlist[0]->childnum == 1){
                    name = exp->childlist[0]->childlist[0]->text;
                    ptr = SearchPar(name,func_name);
                    if(ptr == NULL){
                        ptr = stack[top];
                        while(ptr != NULL) {
                            if (strcmp(ptr->name, name) == 0)
                                break;
                            ptr = ptr->snext;
                        }
                    }
                }
                else{
                    ptr = Exp_analyse(exp->childlist[0],func_name)->type_info.structure;
                }
                if (ptr == NULL) {
                    printf("Error type 1 at line %d: Undefined variable \"%s\"\n", exp->line, name);
                    return NULL;
                }
                else {
                    VarNode* stru_head = ptr->type->type_info.structure;
                    while (stru_head != NULL) {
                        if (strcmp(stru_head->name, exp->childlist[2]->text) == 0) {
                            break;	
                        }
                        stru_head = stru_head->snext;
                    }
                    if (stru_head == NULL) {
                        printf("Error type 14 at line %d: Un-existed field \"%s\"\n", exp->line, exp->childlist[2]->text);
                        return NULL;
                    }
                    else
                        return stru_head->type;
                }
            }
            //注意struct的递归调用
            return Exp_analyse(exp->childlist[0],func_name);
        }
        else if(JudgeOperand(exp->childlist[1]->type) == 5){
            FuncNode* func = SearchFunc(exp->childlist[0]->text);
            VarNode* var = SearchVar(exp->childlist[0]->text);
            if(func == NULL && var == NULL) {
                printf("Error type 2 at line %d: Undefined function \"%s\"\n",exp->line,exp->childlist[0]->text);
                return NULL;
            }
            else if(func == NULL && var != NULL) {
                printf("Error type 11 at line %d: \"%s\" must be a function\n",exp->line,exp->childlist[0]->text);  
                return NULL;          	
            }
        }
    }
    else{
        if(strcmp(exp->childlist[1]->type,"LP") == 0){
            FuncNode* func = SearchFunc(exp->childlist[0]->text);
            VarNode* var = SearchVar(exp->childlist[0]->text);
            if(var == NULL)
                var = SearchPar(exp->childlist[0]->text,func_name);
            if(func == NULL && var != NULL) {
                printf("Error type 11 at line %d: \"%s\" must be a function\n",exp->line,exp->childlist[0]->text);  
                return NULL;
            }
            else if(func == NULL && var == NULL) {
                printf("Error type 2 at line %d: Undefined function \"%s\"\n",exp->line,exp->childlist[0]->text);
                return NULL;
            }
            if(!Args_analyse(exp->childlist[2],func->paralist,func_name)){
                printf("Error type 9 at line %d: The method \"%s\" is not applicable for the arguments \n",exp->line,func->name);
                return func->returnType;
            }
            return func->returnType;
        }
        else{
            //注意多维数组
            TreeNode* ptr = exp->childlist[0]->childlist[0];
            int dimension = 1;
            while(strcmp(ptr->text,"") == 0){
                ptr = ptr->childlist[0];
                dimension ++;
            }
            VarNode* var = SearchVar(ptr->text);
            if(var == NULL){
                printf("Can not translate the code: Contain multidimensional aray and function parameters of array type!\n");
                exit(-1);
            }
            if(var->type->kind != array){
                printf("Error type 10 at line %d: \"%s\" must be an array\n",exp->line,var->name);
                return NULL;
            }
            Type type = Exp_analyse(exp->childlist[2],func_name);
            if(type->kind != basic || type->type_info.basic_info != 0){
                printf("Error type 12 at line %d: Operands type mistaken\n",exp->line);
                // return NULL;
            }
            Type rtype = var->type;
            while(dimension > 0 && rtype != NULL){
                rtype = rtype->type_info.array.elem;
                dimension --;
            }
            if(rtype == NULL){
                printf("Error type Extra2 at line%d: Too many '[]'\n",exp->line);
            }
            return rtype; 
        }
    }
    return NULL;
}
bool Args_analyse(struct TreeNode *args,VarNode* var,char* func_name){
    Type type1 = Exp_analyse(args->childlist[0],func_name);
    Type type2 = var->type;
    bool flag;
    if(type1->kind == type2->kind){
        if(type1->kind == basic)
            flag = (type1->type_info.basic_info == type2->type_info.basic_info);
        else if(type1->kind == array){
            flag = is_equal_type(type1, type2);
        }
        else{
            flag = (strcmp(type1->type_info.structure->name,type1->type_info.structure->name) == 0);
        }
    }
    if(flag == false)
        return false;
    else if(args->childnum == 1){
        return flag;
    }
    else{
        if(var->next == NULL)
            return false;
        flag = Args_analyse(args->childlist[2],var->next,func_name);
        return flag;
    }
}
int JudgeOperand(char* type){
    if(strcmp(type,"ASSIGNOP") == 0)
        return 0;
    else if(strcmp(type,"AND") == 0 || strcmp(type,"OR") == 0)
        return 1;
    else if(strcmp(type,"RELOP") == 0 || strcmp(type,"PLUS") == 0 || strcmp(type,"MINUS") == 0 || strcmp(type,"STAR") == 0 || strcmp(type,"DIV") == 0)
        return 2;
    else if(strcmp(type,"LB") == 0)
        return 3;
    else if(strcmp(type,"DOT") == 0)
        return 4;
    else if(strcmp(type,"LP") == 0)
        return 5;
}
