#include "intercode.h"
/***************Global Counter*******************/
int temp_count = 1;
int label_count = 1;
int var_count = 1;

Operand new_temp(){
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = TEMP;
    op->u.temp_no = temp_count;
    temp_count ++;
    return op;
}

Operand new_label() {
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = LABELOP;
    op->u.label_no = label_count;
    label_count++;
    return op;
}

void create_operand(Operand op,int kind, int i, char* c) {
    op->kind = kind;
    switch(kind) {
        case VARIABLE:
            op->u.var_no = i;
            break;
        case CONSTANT:
            op->u.value = i;
            break;
        case ADDRESS:
            op->u.temp_no = i;
            break;
        case TEMP:
            op->u.temp_no = i;
            break;
        case RELOP_OP:
            op->u.relop = c;
            break;
        case FUNC:
        	op->u.func_name = c;
        	break;
        case SIZE:
            op->u.value = i;
            break;
        case REFERENCE:
            op->u.var_no = i;
            break;
        case ARGOP:
            op->u.arg_no = i;
            break;
    }
    return;
}

struct OperandNode* create_operandnode(Operand op) {
	struct OperandNode* node = (struct OperandNode*)malloc(sizeof(struct OperandNode));
	node->op = op;
	node->next = NULL;
	return node;
}

struct OperandNode* insert_op(Operand op, struct OperandNode* arg_list) {
	struct OperandNode* node = create_operandnode(op);
	node->next = arg_list;
	arg_list = node;
	return arg_list;
}

struct InterCodeNode* create_node(struct InterCode* code) {
    struct InterCodeNode* node = (struct InterCodeNode*)malloc(sizeof(struct InterCodeNode));
    node->code = code;
    node->prev = NULL;
    node->next = NULL;
    return node;
}

int ComputeSize(VarNode* ptr,int count){
    while(ptr != NULL){
        if(ptr->type->kind == basic)
            count ++;
        else if(ptr->type->kind == array)
            count += ptr->type->type_info.array.size;
        else
            count += ComputeSize(ptr->type->type_info.structure,0);
        ptr = ptr->snext;
    }
    return count;
}

//join two nodes to one, attention that a node may be composed by several nodes
struct InterCodeNode* join_node(struct InterCodeNode* node1, struct InterCodeNode* node2) {
    if (node1 == NULL)
        return node2;
    else if (node2 == NULL)
        return node1;
    else {
        struct InterCodeNode* temp = node1;
        while(temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = node2;
        node2->prev = temp;
        return node1;
    }
}

//join three nodes to one
struct InterCodeNode* join_node3(struct InterCodeNode* node1, struct InterCodeNode* node2, struct InterCodeNode* node3) {
    struct InterCodeNode* temp1 = join_node(node1, node2);
    struct InterCodeNode* temp2 = join_node(temp1, node3);
    return temp2;

}

//join four nodes to one
struct InterCodeNode* join_node4(struct InterCodeNode* node1, struct InterCodeNode* node2, struct InterCodeNode* node3, struct InterCodeNode* node4) { 
    struct InterCodeNode* temp1 = join_node(node1, node2);
    struct InterCodeNode* temp2 = join_node(node3, node4);
    struct InterCodeNode* temp3 = join_node(temp1, temp2);
    return temp3;
}

//join five nodes to one
struct InterCodeNode* join_node5(struct InterCodeNode* node1, struct InterCodeNode* node2, struct InterCodeNode* node3, struct InterCodeNode* node4, struct InterCodeNode* node5) {
    struct InterCodeNode* temp1 = join_node(node1, node2);
    struct InterCodeNode* temp2 = join_node(node3, node4);
    struct InterCodeNode* temp3 = join_node(temp1, temp2);
    struct InterCodeNode* temp4 = join_node(temp3, node5);
    return temp4;
}

//join six nodes to one
struct InterCodeNode* join_node6(struct InterCodeNode* node1, struct InterCodeNode* node2, struct InterCodeNode* node3, struct InterCodeNode* node4, struct InterCodeNode* node5, struct InterCodeNode* node6) {
	struct InterCodeNode* temp1 = join_node5(node1, node2, node3, node4, node5);
	struct InterCodeNode* temp2 = join_node(temp1, node6);
	return temp2;
}

struct InterCodeNode* translate_Exp(struct TreeNode* exp, Operand place, char* func_name) {
    if(exp->childnum == 1){
        if(strcmp(exp->childlist[0]->type,"INT") == 0){//INT
            int value = myatoi(exp->childlist[0]->text);
            create_operand(place,CONSTANT,value,NULL);
            return NULL;
        }
        else if(strcmp(exp->childlist[0]->type,"ID") == 0){//ID
            VarNode* var = SearchPar(exp->childlist[0]->text,func_name);
            if(var == NULL)
                var = SearchVar(exp->childlist[0]->text);
            //Licious 
            if(var != NULL){
                create_operand(place,VARIABLE,var->var_no,NULL);
                if(var->var_no == 0){
                    // New Variable
                    var->var_no = var_count;
                    var_count ++;
                }
                return NULL;
            }
        }
        else{
            //FAULT
            return NULL;
        }
    }
    else if(exp->childnum == 2){
        if(strcmp(exp->childlist[0]->type,"MINUS") == 0){//MINUS Exp
            Operand t1 = new_temp();
            struct InterCodeNode* node1 = translate_Exp(exp->childlist[1],t1, func_name);
            if(t1->kind == CONSTANT){
                place->kind = CONSTANT;
                place->u.value = -(t1->u.value);
                return NULL;
            }
            else{
                Operand zero = (Operand)malloc(sizeof(struct Operand_));
                create_operand(zero,CONSTANT,0,NULL);
                struct InterCode* code2 = (struct InterCode*)malloc(sizeof(struct InterCode));
                code2->kind = SUB;
                code2->u.binop.result = place;
                code2->u.binop.op1 = zero;
                code2->u.binop.op2 = t1;
                struct InterCodeNode* node2 = create_node(code2);
                struct InterCodeNode* node = join_node(node1, node2);
                return node;
            }
        }
        else if(strcmp(exp->childlist[0]->type,"NOT") == 0){//NOT Exp
            /**********************Untested************************/
            Operand label1 = new_label();
            Operand label2 = new_label();

            Operand zero = (Operand)malloc(sizeof(struct Operand_));
            Operand one = (Operand)malloc(sizeof(struct Operand_));
            create_operand(zero,CONSTANT,0,NULL);
            create_operand(one,CONSTANT,1,NULL);

            struct InterCode* code0 = (struct InterCode*)malloc(sizeof(struct InterCode));
            code0->kind = ASSIGN;
            code0->u.assign.right = zero;
            code0->u.assign.left = place;
            struct InterCodeNode* node1 = translate_Cond(exp,label1,label2, func_name);

            struct InterCode* code2 = (struct InterCode*)malloc(sizeof(struct InterCode));
            code2->kind = LABEL;
            code2->u.label.labelop = label1;

            struct InterCode* code3 = (struct InterCode*)malloc(sizeof(struct InterCode));
            code3->kind = ASSIGN;
            code3->u.assign.right = one;
            code3->u.assign.left = place;

            struct InterCode* code4 = (struct InterCode*)malloc(sizeof(struct InterCode));
            code4->kind = LABEL;
            code4->u.label.labelop = label2;

            struct InterCodeNode* node0 = create_node(code0);
            struct InterCodeNode* node2 = create_node(code2);
            struct InterCodeNode* node3 = create_node(code3);
            struct InterCodeNode* node4 = create_node(code4);
            struct InterCodeNode* node = join_node5(node0, node1, node2, node3, node4);
            return node;
            /**********************Untested************************/
        }
        else{
            return NULL;
        }
    }
    else if(exp->childnum == 3){
        if (strcmp(exp->childlist[1]->type, "ASSIGNOP") == 0) {//Exp ASSIGNOP Exp
            Operand left = new_temp();
            Operand right = new_temp();
            struct InterCodeNode* node0 = translate_Exp(exp->childlist[0], left, func_name);
            struct InterCodeNode* node1 = translate_Exp(exp->childlist[2], right, func_name);
            struct InterCode* code2 = (struct InterCode*)malloc(sizeof(struct InterCode));
            code2->kind = ASSIGN;
            code2->u.assign.left = left;
            code2->u.assign.right = right;
            place = left;
            struct InterCodeNode* node2 = create_node(code2);
            struct InterCodeNode* node = join_node3(node0, node1, node2);
            return node;
        }
        else if (strcmp(exp->childlist[1]->type, "PLUS") == 0) {//Exp PLUS Exp
            Operand t1 = new_temp();
            Operand t2 = new_temp();
            struct InterCodeNode* node0 = translate_Exp(exp->childlist[0], t1, func_name);
            struct InterCodeNode* node1 = translate_Exp(exp->childlist[2], t2, func_name);
            struct InterCode* code2 = (struct InterCode*)malloc(sizeof(struct InterCode));
            code2->kind = ADD;
            code2->u.binop.op1 = t1;
            code2->u.binop.op2 = t2;
            code2->u.binop.result = place;
            struct InterCodeNode* node2 = create_node(code2);
            struct InterCodeNode* node = join_node3(node0, node1, node2);

            return node;
        }
        else if (strcmp(exp->childlist[1]->type, "MINUS") == 0) {//Exp MINUS Exp
            Operand t1 = new_temp();
            Operand t2 = new_temp();
            struct InterCodeNode* node0 = translate_Exp(exp->childlist[0], t1, func_name);
            struct InterCodeNode* node1 = translate_Exp(exp->childlist[2], t2, func_name);
            struct InterCode* code2 = (struct InterCode*)malloc(sizeof(struct InterCode));
            code2->kind = SUB;
            code2->u.binop.op1 = t1;
            code2->u.binop.op2 = t2;
            code2->u.binop.result = place;
            struct InterCodeNode* node2 = create_node(code2);
            struct InterCodeNode* node = join_node3(node0, node1, node2);
            return node;
        }
        else if (strcmp(exp->childlist[1]->type, "STAR") == 0) {//Exp STAR Exp
            Operand t1 = new_temp();
            Operand t2 = new_temp();
            struct InterCodeNode* node0 = translate_Exp(exp->childlist[0], t1, func_name);
            struct InterCodeNode* node1 = translate_Exp(exp->childlist[2], t2, func_name);
            struct InterCode* code2 = (struct InterCode*)malloc(sizeof(struct InterCode));
            code2->kind = MUL;
            code2->u.binop.op1 = t1;
            code2->u.binop.op2 = t2;
            code2->u.binop.result = place;
            struct InterCodeNode* node2 = create_node(code2);
            struct InterCodeNode* node = join_node3(node0, node1, node2);
            return node;
        }
        else if (strcmp(exp->childlist[1]->type, "DIV") == 0) {//Exp DIV Exp
            Operand t1 = new_temp();
            Operand t2 = new_temp();
            struct InterCodeNode* node0 = translate_Exp(exp->childlist[0], t1, func_name);
            struct InterCodeNode* node1 = translate_Exp(exp->childlist[2], t2, func_name);
            struct InterCode* code2 = (struct InterCode*)malloc(sizeof(struct InterCode));
            code2->kind = DIVISION;
            code2->u.binop.op1 = t1;
            code2->u.binop.op2 = t2;
            code2->u.binop.result = place;
            struct InterCodeNode* node2 = create_node(code2);
            struct InterCodeNode* node = join_node3(node0, node1, node2);
            return node;
        }
        else if (strcmp(exp->childlist[0]->type, "LP") == 0) {//LP Exp RP
            return translate_Exp(exp->childlist[1], place, func_name);
        }
        else if (strcmp(exp->childlist[1]->type, "RELOP") == 0 || strcmp(exp->childlist[1]->type, "AND") == 0 || strcmp(exp->childlist[1]->type, "OR") == 0) {//Exp RELOP Exp /Exp AND Exp /Exp OR Exp 

            Operand label1 = new_label();
            Operand label2 = new_label();

            Operand zero = (Operand)malloc(sizeof(struct Operand_)); 
            create_operand(zero,CONSTANT,0,NULL);
            Operand one = (Operand)malloc(sizeof(struct Operand_));
            create_operand(one,CONSTANT,1,NULL);

            struct InterCode* code0 = (struct InterCode*)malloc(sizeof(struct InterCode));
            code0->kind = ASSIGN;
            code0->u.assign.right = zero;
            code0->u.assign.left = place;
            struct InterCodeNode* node1 = translate_Cond(exp,label1,label2, func_name);

            struct InterCode* code2 = (struct InterCode*)malloc(sizeof(struct InterCode));
            code2->kind = LABEL;
            code2->u.label.labelop = label1;

            struct InterCode* code3 = (struct InterCode*)malloc(sizeof(struct InterCode));
            code3->kind = ASSIGN;
            code3->u.assign.right = one;
            code3->u.assign.left = place;

            struct InterCode* code4 = (struct InterCode*)malloc(sizeof(struct InterCode));
            code4->kind = LABEL;
            code4->u.label.labelop = label2;

            struct InterCodeNode* node0 = create_node(code0);
            struct InterCodeNode* node2 = create_node(code2);
            struct InterCodeNode* node3 = create_node(code3);
            struct InterCodeNode* node4 = create_node(code4);
            struct InterCodeNode* node = join_node5(node0, node1, node2, node3, node4);
            return node;
        }
        else if (strcmp(exp->childlist[0]->type, "ID") == 0 && strcmp(exp->childlist[2]->type, "RP") == 0) {//ID LP RP
        	FuncNode* func = SearchFunc(exp->childlist[0]->text);
        	if (func == NULL) {
        		return NULL;
        	}
        	else {
        		if (strcmp(func->name, "read") == 0) {
        			struct InterCode* code = (struct InterCode*)malloc(sizeof(struct InterCode));
        			code->kind = READ;
        			code->u.read.arg = place;
        			struct InterCodeNode* node = create_node(code);
        			return node;
        		}
        		else {
        			Operand funop = (Operand)malloc(sizeof(struct Operand_)); 
                    create_operand(funop,FUNC, 0, func->name);
        			struct InterCode* code = (struct InterCode*)malloc(sizeof(struct InterCode));
        			code->kind = CALL;
        			code->u.call.op1 = place;
        			code->u.call.op2 = funop;
        			struct InterCodeNode* node = create_node(code);
        			return node;
        		}
        	}
        }
        else if (strcmp(exp->childlist[1]->type, "DOT") == 0) {//Exp DOT ID
            VarNode* var = SearchPar(exp->childlist[0]->childlist[0]->text,func_name);
            Operand op = new_temp();
            if(var == NULL){
                //NOT A PARAM
                var = SearchVar(exp->childlist[0]->childlist[0]->text);
                create_operand(op,REFERENCE,var->var_no,NULL);
                VarNode* svar = var->type->type_info.structure;
                if(strcmp(svar->name,exp->childlist[2]->text) == 0){
                    //First Member
                    Operand t1 = new_temp();
                    struct InterCode* code = (struct InterCode*)malloc(sizeof(struct InterCode));
                    code->kind = ASSIGN;
                    code->u.assign.left = t1;
                    code->u.assign.right = op;
                    struct InterCodeNode* node = create_node(code);
                    create_operand(place,ADDRESS,t1->u.temp_no,NULL);
                    return node;
                }
                else{
                    //Not First Member
                    int count = 0;
                    while(svar != NULL){
                        if(strcmp(svar->name,exp->childlist[2]->text) == 0)
                            break;
                        count ++;
                        svar = svar->snext;
                    }
                    Operand result = new_temp();
                    Operand op2 = (Operand)malloc(sizeof(struct Operand_));
                    create_operand(op2,CONSTANT,count*4,NULL);
                    struct InterCode* code1 = (struct InterCode*)malloc(sizeof(struct InterCode));
                    code1->kind = ADD;
                    code1->u.binop.result = result;
                    code1->u.binop.op1 = op;
                    code1->u.binop.op2 = op2;
                    struct InterCodeNode* node1 = create_node(code1);
                    create_operand(place,ADDRESS,result->u.temp_no,NULL);
                    return node1;
                }
            }
            else{
                //CONDITION: PARAM
                create_operand(op,ADDRESS,var->var_no,NULL);
                VarNode* svar = var->type->type_info.structure;
                if(strcmp(svar->name,exp->childlist[2]->text) == 0){
                    //First Member
                    Operand t1 = new_temp();
                    struct InterCode* code = (struct InterCode*)malloc(sizeof(struct InterCode));
                    code->kind = ASSIGN;
                    code->u.assign.left = t1;
                    code->u.assign.right = op;
                    struct InterCodeNode* node = create_node(code);
                    create_operand(place,TEMP,t1->u.temp_no,NULL);
                    return node;
                }
                else{
                    //Not First Member
                    int count = 0;
                    while(svar != NULL){
                        if(strcmp(svar->name,exp->childlist[2]->text) == 0)
                            break;
                        count ++;
                        svar = svar->snext;
                    }
                    Operand result = new_temp();
                    create_operand(op,VARIABLE,op->u.var_no,NULL);
                    Operand op2 = (Operand)malloc(sizeof(struct Operand_));
                    create_operand(op2,CONSTANT,count*4,NULL);
                    struct InterCode* code1 = (struct InterCode*)malloc(sizeof(struct InterCode));
                    code1->kind = ADD;
                    code1->u.binop.result = result;
                    code1->u.binop.op1 = op;
                    code1->u.binop.op2 = op2;
                    struct InterCodeNode* node1 = create_node(code1);
                    Operand new_result = (Operand)malloc(sizeof(struct Operand_));
                    Operand new_left = new_temp();
                    create_operand(new_result,ADDRESS,result->u.temp_no,NULL);
                    struct InterCode* code2 = (struct InterCode*)malloc(sizeof(struct InterCode));
                    code2->kind = ASSIGN;
                    code2->u.assign.left = new_left;
                    code2->u.assign.right = new_result;
                    struct InterCodeNode* node2 = create_node(code2);
                    create_operand(place,TEMP,new_left->u.temp_no,NULL);
                    return join_node(node1,node2);
                }
            }
        }
    }
    else if(exp->childnum == 4) {
    	if (strcmp(exp->childlist[2]->type, "Args") == 0) {//ID LP Args RP
        	FuncNode* func = SearchFunc(exp->childlist[0]->text);
        	if (func == NULL) {
        		return NULL;
        	}
        	else {
        		struct OperandNode* arg_list = NULL;
				struct InterCodeNode* node1 = translate_Args(exp->childlist[2], &arg_list, func_name);
				if (strcmp(func->name, "write") == 0) {
					struct InterCode* code2 = (struct InterCode*)malloc(sizeof(struct InterCode));
					code2->kind = WRITE;
					code2->u.write.arg = arg_list->op;
					struct InterCodeNode* node2 = create_node(code2); 
					struct InterCodeNode* node = join_node(node1, node2);
					return node;
				}
				else {
					struct InterCodeNode* node2 = NULL;
					while (arg_list != NULL)
					{
						struct InterCode* code3 = (struct InterCode*)malloc(sizeof(struct InterCode));
						code3->kind = ARG;
						code3->u.arg.argop = arg_list->op;
						struct InterCodeNode* node3 = create_node(code3);
						node2 = join_node(node2, node3);
						arg_list = arg_list->next;
					}
        			Operand funop = (Operand)malloc(sizeof(struct Operand_)); 
					create_operand(funop,FUNC, 0, func->name);
					struct InterCode* code3 = (struct InterCode*)malloc(sizeof(struct InterCode));
        			code3->kind = CALL;
                    if(place == NULL){
                        Operand ptr = new_temp();
                        code3->u.call.op1 = ptr;
                    }
                    else
        			    code3->u.call.op1 = place;
        			code3->u.call.op2 = funop;
        			struct InterCodeNode* node3 = create_node(code3);
					struct InterCodeNode* node = join_node3(node1, node2, node3);
					return node;
				}
        	}
        }
        else if (strcmp(exp->childlist[1]->type, "LB") == 0) {//Exp LB Exp RB
            VarNode* var = SearchPar(exp->childlist[0]->childlist[0]->text,func_name);
            Operand op = new_temp();
            if(var == NULL){
                //NOT A PARAM
                var = SearchVar(exp->childlist[0]->childlist[0]->text);
                create_operand(op,REFERENCE,var->var_no,NULL);
                if(strcmp(exp->childlist[2]->childlist[0]->type,"INT") == 0){
                    int size = myatoi(exp->childlist[2]->childlist[0]->text);
                    if(size == 0){
                        Operand t1 = new_temp();
                        struct InterCode* code = (struct InterCode*)malloc(sizeof(struct InterCode));
                        code->kind = ASSIGN;
                        code->u.assign.left = t1;
                        code->u.assign.right = op;
                        struct InterCodeNode* node = create_node(code);
                        create_operand(place,ADDRESS,t1->u.temp_no,NULL);
                        return node;
                    }
                    else{
                        Operand result = new_temp();
                        Operand op2 = (Operand)malloc(sizeof(struct Operand_));
                        create_operand(op2,CONSTANT,size*4,NULL);
                        struct InterCode* code1 = (struct InterCode*)malloc(sizeof(struct InterCode));
                        code1->kind = ADD;
                        code1->u.binop.result = result;
                        code1->u.binop.op1 = op;
                        code1->u.binop.op2 = op2;
                        struct InterCodeNode* node1 = create_node(code1);
                        create_operand(place,ADDRESS,result->u.temp_no,NULL);
                        return node1;
                    }
                }
                else if(strcmp(exp->childlist[2]->childlist[0]->type,"ID") == 0){
                    // ID
                    VarNode* varID = SearchVar(exp->childlist[2]->childlist[0]->text);
                    Operand result1 = new_temp();
                    Operand op1 = new_temp();
                    create_operand(op1,VARIABLE,varID->var_no,NULL);
                    Operand op2 = new_temp();
                    create_operand(op2,CONSTANT,4,NULL);
                    struct InterCode* code1 = (struct InterCode*)malloc(sizeof(struct InterCode));
                    code1->kind = MUL;
                    code1->u.binop.result = result1;
                    code1->u.binop.op1 = op1;
                    code1->u.binop.op2 = op2;
                    struct InterCodeNode* node1 = create_node(code1);
                    
                    Operand result2 = new_temp();
                    Operand op3 = new_temp();
                    create_operand(op3,REFERENCE,var->var_no,NULL);
                    struct InterCode* code2 = (struct InterCode*)malloc(sizeof(struct InterCode));
                    code2->kind = ADD;
                    code2->u.binop.result = result2;
                    code2->u.binop.op1 = op3;
                    code2->u.binop.op2 = result1;
                    struct InterCodeNode* node2 = create_node(code2);
                    create_operand(place,ADDRESS,result2->u.temp_no,NULL);
                    return join_node(node1,node2);
                }
                else{
                    //Exp
                    Operand op1 = new_temp();
                    Operand op2 = new_temp();
                    create_operand(op2,CONSTANT,4,NULL);
                    Operand result1 = new_temp(); 
                    struct InterCodeNode* node0 = translate_Exp(exp->childlist[2], op1, func_name);
                    struct InterCode* code1 = (struct InterCode*)malloc(sizeof(struct InterCode));
                    code1->kind = MUL;
                    code1->u.binop.result = result1;
                    code1->u.binop.op1 = op1;
                    code1->u.binop.op2 = op2;
                    struct InterCodeNode* node1 = create_node(code1);

                    Operand result2 = new_temp();
                    Operand op3 = new_temp();
                    create_operand(op3,REFERENCE,var->var_no,NULL);
                    struct InterCode* code2 = (struct InterCode*)malloc(sizeof(struct InterCode));
                    code2->kind = ADD;
                    code2->u.binop.result = result2;
                    code2->u.binop.op1 = op3;
                    code2->u.binop.op2 = result1;
                    struct InterCodeNode* node2 = create_node(code2);
                    create_operand(place,ADDRESS,result2->u.temp_no,NULL);
                    return join_node3(node0,node1,node2);
                }
            }
            else{
                //IN THE PARALIST
                create_operand(op,ADDRESS,var->var_no,NULL);
                if(strcmp(exp->childlist[2]->type,"INT") == 0){
                    int size = myatoi(exp->childlist[2]->childlist[0]->text);
                    if(size == 0){
                        Operand t1 = new_temp();
                        struct InterCode* code = (struct InterCode*)malloc(sizeof(struct InterCode));
                        code->kind = ASSIGN;
                        code->u.assign.left = t1;
                        code->u.assign.right = op;
                        struct InterCodeNode* node = create_node(code);
                        create_operand(place,TEMP,t1->u.temp_no,NULL);
                        return node;
                    }
                    else{
                        Operand result = new_temp();
                        Operand op2 = (Operand)malloc(sizeof(struct Operand_));
                        Operand op3 = (Operand)malloc(sizeof(struct Operand_));
                        create_operand(op3,VARIABLE,op->u.var_no,NULL);
                        create_operand(op2,CONSTANT,size*4,NULL);
                        struct InterCode* code1 = (struct InterCode*)malloc(sizeof(struct InterCode));
                        code1->kind = ADD;
                        code1->u.binop.result = result;
                        code1->u.binop.op1 = op3;
                        code1->u.binop.op2 = op2;
                        struct InterCodeNode* node1 = create_node(code1);
                        create_operand(place,ADDRESS,result->u.temp_no,NULL);
                        return node1;
                    }
                }
                else if(strcmp(exp->childlist[2]->type,"ID") == 0){
                    // ID
                    VarNode* varID = SearchVar(exp->childlist[2]->childlist[0]->text);
                    Operand result1 = new_temp();
                    Operand op1 = new_temp();
                    create_operand(op1,VARIABLE,varID->var_no,NULL);
                    Operand op2 = new_temp();
                    create_operand(op2,CONSTANT,4,NULL);
                    struct InterCode* code1 = (struct InterCode*)malloc(sizeof(struct InterCode));
                    code1->kind = MUL;
                    code1->u.binop.result = result1;
                    code1->u.binop.op1 = op1;
                    code1->u.binop.op2 = op2;
                    struct InterCodeNode* node1 = create_node(code1);
                    
                    Operand result2 = new_temp();
                    Operand op3 = new_temp();
                    create_operand(op3,VARIABLE,var->var_no,NULL);
                    struct InterCode* code2 = (struct InterCode*)malloc(sizeof(struct InterCode));
                    code2->kind = ADD;
                    code2->u.binop.result = result2;
                    code2->u.binop.op1 = op3;
                    code2->u.binop.op2 = result1;
                    struct InterCodeNode* node2 = create_node(code2);
                    create_operand(place,ADDRESS,result2->u.temp_no,NULL);
                    return join_node(node1,node2);
                }
                else{
                }
            }
        }
    }
    else{
        return NULL;
    }
}

struct InterCodeNode* translate_Stmt(struct TreeNode* stmt, char* func_name) {
	if (stmt->childnum == 1) {//CompSt
		return translate_CompSt(stmt->childlist[0], func_name);
	}
	else if (stmt->childnum == 2) {//Exp SEMI
		return translate_Exp(stmt->childlist[0], NULL, func_name);
	}
	else if (stmt->childnum == 3) {//RETURN Exp SEMI
		Operand t1 = new_temp();
		struct InterCodeNode* node1 = translate_Exp(stmt->childlist[1], t1, func_name);
		struct InterCode* code2 = (struct InterCode*)malloc(sizeof(struct InterCode));
		code2->kind = RETURN_CODE;
		code2->u.ret.returnop = t1;
		struct InterCodeNode* node2 = create_node(code2);
		struct InterCodeNode* node = join_node(node1, node2);
		return node;
	}
	else if (stmt->childnum == 5) {
		if (strcmp(stmt->childlist[0]->type, "IF") == 0) {//IF LP Exp RP Stmt
			Operand label1 = new_label();
			Operand label2 = new_label();
			struct InterCodeNode* node1 = translate_Cond(stmt->childlist[2], label1, label2, func_name);
			struct InterCodeNode* node3 = translate_Stmt(stmt->childlist[4], func_name);
			
			struct InterCode* code2 = (struct InterCode*)malloc(sizeof(struct InterCode));
			code2->kind = LABEL;
			code2->u.label.labelop = label1;
			struct InterCode* code4 = (struct InterCode*)malloc(sizeof(struct InterCode));
			code4->kind = LABEL;
			code4->u.label.labelop = label2;
			
			struct InterCodeNode* node2 = create_node(code2);
			struct InterCodeNode* node4 = create_node(code4);
			
			struct InterCodeNode* node = join_node4(node1, node2, node3, node4);
			return node;
		}
		else if (strcmp(stmt->childlist[0]->type, "WHILE") == 0) {//WHILE LP Exp RP Stmt
			Operand label1 = new_label();
			Operand label2 = new_label();
			Operand label3 = new_label();
			struct InterCodeNode* node2 = translate_Cond(stmt->childlist[2], label2, label3, func_name);//pay attention to the index of the label
			struct InterCodeNode* node4 = translate_Stmt(stmt->childlist[4], func_name);
			
			struct InterCode* code1 = (struct InterCode*)malloc(sizeof(struct InterCode));
			code1->kind = LABEL;
			code1->u.label.labelop = label1;
			struct InterCode* code3 = (struct InterCode*)malloc(sizeof(struct InterCode));
			code3->kind = LABEL;
			code3->u.label.labelop = label2;
			struct InterCode* code5 = (struct InterCode*)malloc(sizeof(struct InterCode));
			code5->kind = GOTO;
			code5->u.go.labelop = label1;
			struct InterCode* code6 = (struct InterCode*)malloc(sizeof(struct InterCode));
			code6->kind = LABEL;
			code6->u.label.labelop = label3;
			
			struct InterCodeNode* node1 = create_node(code1);
			struct InterCodeNode* node3 = create_node(code3);
			struct InterCodeNode* node5 = create_node(code5);
			struct InterCodeNode* node6 = create_node(code6);
			
			struct InterCodeNode* node = join_node6(node1, node2, node3, node4, node5, node6);
			return node;
		}
	}
	else if (stmt->childnum == 7) {//IF LPExp RP Stmt ELSE Stmt
		Operand label1 = new_label();
		Operand label2 = new_label();
		Operand label3 = new_label();
		struct InterCodeNode* node1 = translate_Cond(stmt->childlist[2], label1, label2, func_name);
		struct InterCodeNode* node3 = translate_Stmt(stmt->childlist[4], func_name);
		struct InterCodeNode* node6 = translate_Stmt(stmt->childlist[6], func_name);
		
		struct InterCode* code2 = (struct InterCode*)malloc(sizeof(struct InterCode));
		code2->kind = LABEL;
		code2->u.label.labelop = label1;
		struct InterCode* code4 = (struct InterCode*)malloc(sizeof(struct InterCode));
		code4->kind = GOTO;
		code4->u.go.labelop = label3;
		struct InterCode* code5 = (struct InterCode*)malloc(sizeof(struct InterCode));
		code5->kind = LABEL;
		code5->u.label.labelop = label2;
		struct InterCode* code7 = (struct InterCode*)malloc(sizeof(struct InterCode));
		code7->kind = LABEL;
		code7->u.label.labelop = label3;
		
		struct InterCodeNode* node2 = create_node(code2);
		struct InterCodeNode* node4 = create_node(code4);
		struct InterCodeNode* node5 = create_node(code5);
		struct InterCodeNode* node7 = create_node(code7);
		
		struct InterCodeNode* nodetemp = join_node6(node1, node2, node3, node4, node5, node6);
		struct InterCodeNode* node = join_node(nodetemp, node7);
		return node;
	}
	else {
		return NULL;
	}
}

struct InterCodeNode* translate_CompSt(struct TreeNode* compst, char* func_name) {
    struct InterCodeNode* node1 = translate_DefList(compst->childlist[1], func_name);
    struct InterCodeNode* node2 = translate_StmtList(compst->childlist[2], func_name);

    struct InterCodeNode* node = join_node(node1,node2);
    return node;
}

struct InterCodeNode* translate_DefList(struct TreeNode* deflist, char* func_name){
    // Empty Sentence
    if(deflist == NULL){
        return NULL;
    }
    
    struct InterCodeNode* node1 = translate_Def(deflist->childlist[0], func_name);
    struct InterCodeNode* node2 = translate_DefList(deflist->childlist[1], func_name);
    
    struct InterCodeNode* node = join_node(node1,node2);
    return node;
}

struct InterCodeNode* translate_StmtList(struct TreeNode* stmtlist, char* func_name){
    if(stmtlist == NULL)
        return NULL;
    else{
        struct InterCodeNode* node1 = translate_Stmt(stmtlist->childlist[0], func_name);
        struct InterCodeNode* node2 = translate_StmtList(stmtlist->childlist[1], func_name);
    
        struct InterCodeNode* node = join_node(node1,node2);
        return node;
    }
}

struct InterCodeNode* translate_Def(struct TreeNode* def, char* func_name){
    struct InterCodeNode* node = translate_DecList(def->childlist[1], func_name);
    return node;
}

struct InterCodeNode* translate_DecList(struct TreeNode* declist, char* func_name){
    if(declist->childnum == 1){
        struct InterCodeNode* node = translate_Dec(declist->childlist[0], func_name);
        return node;
    }
    else if(declist->childnum == 3){
        struct InterCodeNode* node1 = translate_Dec(declist->childlist[0], func_name);
        struct InterCodeNode* node2 = translate_DecList(declist->childlist[2], func_name);
        
        struct InterCodeNode* node = join_node(node1,node2);
        return node;
    }
}

struct InterCodeNode* translate_Dec(struct TreeNode* dec, char* func_name){
    if(dec->childnum == 1){
        Operand temp = new_temp();
        struct InterCodeNode* node = translate_VarDec(dec->childlist[0],temp);
        return node;
    }
    else if(dec->childnum == 3){
        Operand left = new_temp();
        Operand right = new_temp();
        struct InterCodeNode* node0 = translate_VarDec(dec->childlist[0],left);
        struct InterCodeNode* node1 = translate_Exp(dec->childlist[2], right, func_name);
        struct InterCode* code2 = (struct InterCode*)malloc(sizeof(struct InterCode));
        code2->kind = ASSIGN;
        code2->u.assign.left = left;
        code2->u.assign.right = right;
        struct InterCodeNode* node2 = create_node(code2);
        struct InterCodeNode* node = join_node3(node0, node1, node2);
        return node;
    }
}

struct InterCodeNode* translate_VarDec(struct TreeNode* vardec,Operand place){
    if(vardec->childnum == 1){
        //VarDec ---> ID
        VarNode* var = SearchVar(vardec->childlist[0]->text);
        if(var != NULL){
            if(var->type->kind == basic){
                create_operand(place,VARIABLE,var_count,NULL);
                var->var_no = var_count;
                var_count ++;
                return NULL;
            }
            else{
                int size = ComputeSize(var->type->type_info.structure,0);
                Operand var_op = new_temp();
                Operand size_op = new_temp();
                create_operand(size_op,SIZE,size * 4,NULL);
                create_operand(var_op,VARIABLE,var_count,NULL);
                var->var_no = var_count;
                var_count ++;
                struct InterCode* code = (struct InterCode*)malloc(sizeof(struct InterCode));
                code->kind = DEC;
                code->u.dec.var = var_op;
                code->u.dec.size = size_op;
                struct InterCodeNode* node = create_node(code);
                return node;
            }
        }
    }
    else if(vardec->childnum == 4){
        //VarDec ---> ID [INT]
        VarNode* var = SearchVar(vardec->childlist[0]->childlist[0]->text);
        if(var != NULL){
            int size = myatoi(vardec->childlist[2]->text);
            Operand var_op = new_temp();
            Operand size_op = new_temp();
            create_operand(size_op,SIZE,size * 4,NULL);
            create_operand(var_op,VARIABLE,var_count,NULL);
            var->var_no = var_count;
            var_count ++;
            struct InterCode* code = (struct InterCode*)malloc(sizeof(struct InterCode));
            code->kind = DEC;
            code->u.dec.var = var_op;
            code->u.dec.size = size_op;
            struct InterCodeNode* node = create_node(code);
            return node;
        }
        return NULL;
    }
    else{
        return NULL;
    }
}

struct InterCodeNode* translate_FunDec(struct TreeNode* fundec){
    if(fundec->childnum == 3){
        //FunDec ---> ID()
        struct InterCode* code = (struct InterCode *)malloc(sizeof(struct InterCode));
        Operand fun = (Operand)malloc(sizeof(struct Operand_));
        create_operand(fun,FUNC,0,fundec->childlist[0]->text); 
        code->kind = FUNCTION;
        code->u.function.func_name = fun;
        struct InterCodeNode* node = create_node(code);
        return node;
    }
    else{
        //FunDec ---> ID (Para)
        struct InterCode* code1 = (struct InterCode *)malloc(sizeof(struct InterCode));
        Operand fun = (Operand)malloc(sizeof(struct Operand_));
        create_operand(fun,FUNC,0,fundec->childlist[0]->text); 
        code1->kind = FUNCTION;
        code1->u.function.func_name = fun;
        struct InterCodeNode* node1 = create_node(code1); 
        struct InterCodeNode* node = node1;
        VarNode* temp = SearchFunc(fundec->childlist[0]->text)->paralist;
        while(temp != NULL){
            struct InterCode* code2 = (struct InterCode *)malloc(sizeof(struct InterCode));
            Operand param = (Operand)malloc(sizeof(struct Operand_));
            create_operand(param,VARIABLE,var_count,NULL);
            temp->var_no = var_count;
            var_count ++;
            code2->kind = PARAM;
            code2->u.param.var = param;
            struct InterCodeNode* node2 = create_node(code2);
            node = join_node(node,node2);
            temp = temp->next;
        }
        return node;
    }
}

struct InterCodeNode* translate_Cond(struct TreeNode* exp, Operand label_true, Operand label_false, char* func_name) {
    if (exp->childnum == 2) {
        if (strcmp(exp->childlist[0]->type, "NOT") == 0) {
            return translate_Cond(exp->childlist[1], label_false, label_true, func_name);
        }
        else {
            Operand t1 = new_temp();
            struct InterCodeNode* node1= translate_Exp(exp, t1, func_name);
            Operand not_equal = (Operand)malloc(sizeof(struct Operand_)); 
            create_operand(not_equal,RELOP_OP, 0, "!=");
        	Operand zero = (Operand)malloc(sizeof(struct Operand_)); 
            create_operand(zero,CONSTANT, 0, NULL);

            struct InterCode* code2 = (struct InterCode*)malloc(sizeof(struct InterCode));
            code2->kind = REL;
            code2->u.rel.op1 = t1;
            code2->u.rel.relop = not_equal;
            code2->u.rel.op2 = zero;
            code2->u.rel.labelop = label_true;

            struct InterCode* code3 = (struct InterCode*)malloc(sizeof(struct InterCode));
            code3->kind = GOTO;
            code3->u.go.labelop = label_false;

            struct InterCodeNode* node2 = create_node(code2);
            struct InterCodeNode* node3 = create_node(code3);
            struct InterCodeNode* node = join_node3(node1, node2, node3);
            return node;
        }
    }
    else if (exp->childnum == 3) {
        if (strcmp(exp->childlist[1]->type, "RELOP") == 0) {
            Operand t1 = new_temp();
            Operand t2 = new_temp();

            struct InterCodeNode* node1= translate_Exp(exp->childlist[0], t1, func_name);
            struct InterCodeNode* node2= translate_Exp(exp->childlist[2], t2, func_name);

        	Operand relop = (Operand)malloc(sizeof(struct Operand_)); 
            create_operand(relop,RELOP_OP, 0, exp->childlist[1]->text);

            struct InterCode* code3 = (struct InterCode*)malloc(sizeof(struct InterCode));
            code3->kind = REL;
            code3->u.rel.op1 = t1;
            code3->u.rel.relop = relop;
            code3->u.rel.op2 = t2;
            code3->u.rel.labelop = label_true;

            struct InterCode* code4 = (struct InterCode*)malloc(sizeof(struct InterCode));
            code4->kind = GOTO;
            code4->u.go.labelop = label_false;
            
            struct InterCodeNode* node3 = create_node(code3);
            struct InterCodeNode* node4 = create_node(code4);
            struct InterCodeNode* node = join_node4(node1, node2, node3, node4);
            return node;   
        }
        else if (strcmp(exp->childlist[1]->type, "AND") == 0) {
            Operand label1 = new_label();
            struct InterCodeNode* node1= translate_Cond(exp->childlist[0], label1, label_false, func_name);
            struct InterCodeNode* node2= translate_Cond(exp->childlist[2], label_true, label_false, func_name);
            struct InterCode* code_mid = (struct InterCode*)malloc(sizeof(struct InterCode));
            code_mid->kind = LABEL;
            code_mid->u.label.labelop = label1;

            struct InterCodeNode* node_mid = create_node(code_mid);
            struct InterCodeNode* node = join_node3(node1, node_mid, node2);
            return node;
        }
        else if (strcmp(exp->childlist[1]->type, "OR") == 0) {
            Operand label1 = new_label();
            struct InterCodeNode* node1= translate_Cond(exp->childlist[0], label_true, label1, func_name);
            struct InterCodeNode* node2= translate_Cond(exp->childlist[2], label_true, label_false, func_name);
            struct InterCode* code_mid = (struct InterCode*)malloc(sizeof(struct InterCode));
            code_mid->kind = LABEL;
            code_mid->u.label.labelop = label1;

            struct InterCodeNode* node_mid = create_node(code_mid);
            struct InterCodeNode* node = join_node3(node1, node_mid, node2);
            return node;
        }
        else {
            Operand t1 = new_temp();
            struct InterCodeNode* node1= translate_Exp(exp, t1, func_name);
        	Operand not_equal = (Operand)malloc(sizeof(struct Operand_)); 
    		Operand zero = (Operand)malloc(sizeof(struct Operand_)); 
            create_operand(not_equal,RELOP_OP, 0, "!=");
            create_operand(zero,CONSTANT, 0, NULL);

            struct InterCode* code2 = (struct InterCode*)malloc(sizeof(struct InterCode));
            code2->kind = REL;
            code2->u.rel.op1 = t1;
            code2->u.rel.relop = not_equal;
            code2->u.rel.op2 = zero;
            code2->u.rel.labelop = label_true;

            struct InterCode* code3 = (struct InterCode*)malloc(sizeof(struct InterCode));
            code3->kind = GOTO;
            code3->u.go.labelop = label_false;

            struct InterCodeNode* node2 = create_node(code2);
            struct InterCodeNode* node3 = create_node(code3);
            struct InterCodeNode* node = join_node3(node1, node2, node3);
            return node;
        }

    }
    else {
        Operand t1 = new_temp();
        struct InterCodeNode* node1= translate_Exp(exp, t1, func_name);
        Operand not_equal = (Operand)malloc(sizeof(struct Operand_)); 
        Operand zero = (Operand)malloc(sizeof(struct Operand_)); 
        create_operand(not_equal,RELOP_OP, 0, "!=");
        create_operand(zero,CONSTANT, 0, NULL);

        struct InterCode* code2 = (struct InterCode*)malloc(sizeof(struct InterCode));
        code2->kind = REL;
        code2->u.rel.op1 = t1;
        code2->u.rel.relop = not_equal;
        code2->u.rel.op2 = zero;
        code2->u.rel.labelop = label_true;

        struct InterCode* code3 = (struct InterCode*)malloc(sizeof(struct InterCode));
        code3->kind = GOTO;
        code3->u.go.labelop = label_false;

        struct InterCodeNode* node2 = create_node(code2);
        struct InterCodeNode* node3 = create_node(code3);
        struct InterCodeNode* node = join_node3(node1, node2, node3);
        return node;
    }
}

struct InterCodeNode* translate_Args(struct TreeNode* args, struct OperandNode** arg_list_addr, char* func_name) {
	if (args->childnum == 1) {//Exp
		Operand t1 = new_temp();
		struct InterCodeNode* node1 = translate_Exp(args->childlist[0], t1, func_name);
		//if Exp is an ID?////////////////////////////////////////////////
		*arg_list_addr = insert_op(t1, *arg_list_addr);
		return node1;
	}
	else {//Exp COMMA Args
		Operand t1 = new_temp();
		struct InterCodeNode* node1 = translate_Exp(args->childlist[0], t1, func_name);
		//if Exp is an ID?////////////////////////////////////////////////
		*arg_list_addr = insert_op(t1, *arg_list_addr);
		struct InterCodeNode* node2 = translate_Args(args->childlist[2], arg_list_addr, func_name);
		struct InterCodeNode* node = join_node(node1, node2);
		return node;
	}
}

struct InterCodeNode* translate_ExtDefList(struct TreeNode* extdeflist){
    if(extdeflist == NULL)
        return NULL;
    struct InterCodeNode* node1 = translate_ExtDef(extdeflist->childlist[0]);
    struct InterCodeNode* node2 = translate_ExtDefList(extdeflist->childlist[1]);
    return join_node(node1,node2);
}

struct InterCodeNode* translate_ExtDef(struct TreeNode* extdef){
    if(extdef->childnum == 3){
        char* func_name = extdef->childlist[1]->childlist[0]->text;
        struct InterCodeNode* node1 = translate_FunDec(extdef->childlist[1]);
        struct InterCodeNode* node2 = translate_CompSt(extdef->childlist[2], func_name);

        return join_node(node1,node2);
    }
    else 
        return NULL;

}

void fputsNode(struct InterCodeNode* head,FILE* stream){
    struct InterCodeNode* ptr = head;
    int i = 1;
    while(ptr != NULL){
        fputsCode(ptr->code,stream);
        ptr = ptr->next;
    }
    return;
}
void fputsCode(struct InterCode* ptr,FILE* stream){
    switch(ptr->kind){
        case ASSIGN:
            fputsOperand(ptr->u.assign.left,stream);
            printf(" := ");
            fprintf(stream," := ");
            fputsOperand(ptr->u.assign.right,stream);
            break;
        case ADD:
            fputsOperand(ptr->u.binop.result,stream);
            printf(" := ");
            fprintf(stream," := ");
            fputsOperand(ptr->u.binop.op1,stream);
            printf(" + ");
            fprintf(stream," + ");
            fputsOperand(ptr->u.binop.op2,stream);
            break;
        case SUB:        
            fputsOperand(ptr->u.binop.result,stream);
            printf(" := ");
            fprintf(stream," := ");
            fputsOperand(ptr->u.binop.op1,stream);
            printf(" - ");
            fprintf(stream," - ");
            fputsOperand(ptr->u.binop.op2,stream);
            break;
        case MUL:        
            fputsOperand(ptr->u.binop.result,stream);
            printf(" := ");
            fprintf(stream," := ");
            fputsOperand(ptr->u.binop.op1,stream);
            printf(" * ");
            fprintf(stream," * ");
            fputsOperand(ptr->u.binop.op2,stream);
            break;
        case DIVISION:        
            fputsOperand(ptr->u.binop.result,stream);
            printf(" := ");
            fprintf(stream," := ");
            fputsOperand(ptr->u.binop.op1,stream);
            printf(" / ");
            fprintf(stream," / ");
            fputsOperand(ptr->u.binop.op2,stream);
            break;
        case LABEL:        
            printf("LABEL ");
            fprintf(stream,"LABEL ");
            fputsOperand(ptr->u.label.labelop,stream);
            printf(" :");
            fprintf(stream," :");
            break;
        case REL:
            printf("IF ");
            fprintf(stream,"IF ");
            fputsOperand(ptr->u.rel.op1,stream);
            printf(" ");
            fprintf(stream," ");
            fputsOperand(ptr->u.rel.relop,stream);
            printf(" ");
            fprintf(stream," ");
            fputsOperand(ptr->u.rel.op2,stream);
            printf(" GOTO ");
            fprintf(stream," GOTO ");
            fputsOperand(ptr->u.rel.labelop,stream);
            break;
        case GOTO:        
            printf("GOTO ");
            fprintf(stream,"GOTO ");
            fputsOperand(ptr->u.go.labelop,stream);
            break;
        case READ:
            printf("READ ");
            fprintf(stream,"READ ");
            fputsOperand(ptr->u.read.arg,stream);
            break;
        case CALL:
            fputsOperand(ptr->u.call.op1,stream);
            printf(" := CALL ");
            fprintf(stream," := CALL ");
            fputsOperand(ptr->u.call.op2,stream);
            break;
        case WRITE:
            printf("WRITE ");
            fprintf(stream,"WRITE ");
            fputsOperand(ptr->u.write.arg,stream);
            break;
        case ARG:
            printf("ARG ");
            fprintf(stream,"ARG ");
            fputsOperand(ptr->u.arg.argop,stream);
            break;
        case RETURN_CODE:
            printf("RETURN ");
            fprintf(stream,"RETURN ");
            fputsOperand(ptr->u.ret.returnop,stream);
            break;
        case FUNCTION:
            printf("FUNCTION ");
            fprintf(stream,"FUNCTION ");
            fputsOperand(ptr->u.function.func_name,stream);
            printf(" :");
            fprintf(stream," :");
            break;
        case PARAM:
            printf("PARAM ");
            fprintf(stream,"PARAM ");
            fputsOperand(ptr->u.param.var,stream);
            break;
        case DEC:
            printf("DEC ");
            fprintf(stream,"DEC ");
            fputsOperand(ptr->u.dec.var,stream);
            printf(" ");
            fprintf(stream," ");
            fputsOperand(ptr->u.dec.size,stream);
            break;
    }
    printf("\n");
    fprintf(stream,"\n");
}
void fputsOperand(Operand ptr,FILE* stream){
    switch(ptr->kind){
        case VARIABLE:
            printf("v%d",ptr->u.var_no);
            fprintf(stream,"v%d",ptr->u.var_no);
            break;
        case CONSTANT:
            printf("#%d",ptr->u.value);
            fprintf(stream,"#%d",ptr->u.value);
            break;
        case ADDRESS:
            printf("*t%d",ptr->u.var_no);
            fprintf(stream,"*t%d",ptr->u.var_no);
            break;
        case TEMP:
            printf("t%d",ptr->u.temp_no);
            fprintf(stream,"t%d",ptr->u.temp_no);
            break;
        case LABELOP:
            printf("label%d",ptr->u.label_no);
            fprintf(stream,"label%d",ptr->u.label_no);
            break;
        case RELOP_OP:
            printf("%s",ptr->u.relop);
            fprintf(stream,"%s",ptr->u.relop);
            break;
        case FUNC:
            printf("%s",ptr->u.func_name);
            fprintf(stream,"%s",ptr->u.func_name);
            break;
        case SIZE:
            printf("%d",ptr->u.value);
            fprintf(stream,"%d",ptr->u.value);
            break;
        case REFERENCE:
            printf("&v%d",ptr->u.var_no);
            fprintf(stream,"&v%d",ptr->u.var_no);
            break;
        default:
            printf("Wrong Operand\n");
            fprintf(stream,"Wrong Operand\n");
            break;
    }
    return;
}
