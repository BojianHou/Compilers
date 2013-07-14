#include "mipscode.h"

struct OffsetNode* offset_head = NULL;
struct OffsetNode* offset_tail = NULL;
int arg_num = 0;
int param_num = 0;
int param_total = 0;
struct InterCodeNode* stop = NULL;

void outputMipsCode(FILE* stream,struct InterCodeNode* ptr){
    preMips(stream);
//    InitOffset(ptr);
    stop = ptr;
    struct InterCodeNode* temp = ptr;
    while(temp != NULL){
        fputsMips(stream,temp->code);
        temp = temp->next;
    }
}

void preMips(FILE* stream){
    fputs(".data\n",stream);
    fputs("_prompt: .asciiz \"Enter an integer:\"\n",stream);
    fputs("_ret: .asciiz \"\\n\"\n",stream);
    fputs(".globl main\n",stream);
    
    fputs("\n.text\n",stream);
    fputs("read:\n",stream);
    fputs("\tli $v0 , 4\n",stream);
    fputs("\tla $a0 , _prompt\n",stream);
    fputs("\tsyscall\n",stream);
    fputs("\tli $v0 , 5\n",stream);
    fputs("\tsyscall\n",stream);
    fputs("\tjr $ra\n",stream);
    
    fputs("\nwrite:\n",stream);
    fputs("\tli $v0 , 1\n",stream);
    fputs("\tsyscall\n",stream);
    fputs("\tli $v0 , 4\n",stream);
    fputs("\tla $a0 , _ret\n",stream);
    fputs("\tsyscall\n",stream);
    fputs("\tmove $v0, $0\n",stream);
    fputs("\tjr $ra\n",stream);
}

void InitOffset(struct InterCodeNode* ptr){
    struct InterCodeNode* temp = ptr;
    int flag = 0;
    arg_num = 0;
    param_num = 0;
    param_total = 0;
    while(temp != NULL){
        if (temp->code->kind == FUNCTION) {
            stop = temp;
            if (flag == 0) {
                flag = 1;
            }
            else{ 
                break;
            }
        }
        InitOffsetNode(temp->code);
        temp = temp->next;
    }
}

int isEqual(Operand op1,Operand op2){
    if(op1->kind == ADDRESS || op1->kind == TEMP){
        if(op2->kind == ADDRESS || op2->kind == TEMP){
            if(op1->u.temp_no == op2->u.temp_no)
                return 0;
            else
                return 1;
        }
        else
            return 1;
    }
    else if(op1->kind == REFERENCE || op1->kind == VARIABLE){
        if(op2->kind == REFERENCE || op2->kind == VARIABLE){
            if(op1->u.var_no == op2->u.var_no)
                return 0;
            else
                return 1;
        }
        else
            return 1;
    }
    else{
        return memcmp(op1,op2,sizeof(struct Operand_));
    }
}

void InitOffsetNode(struct InterCode* ptr){
    switch(ptr->kind){
        case ASSIGN:
            InsertOffset(ptr->u.assign.left,4); 
            InsertOffset(ptr->u.assign.right,4); 
            break;
        case ADD:case SUB:case MUL:case DIVISION:
            InsertOffset(ptr->u.binop.result,4); 
            InsertOffset(ptr->u.binop.op1,4); 
            InsertOffset(ptr->u.binop.op2,4); 
            break;
        case LABEL:        
            break;
        case REL:
            InsertOffset(ptr->u.rel.op1,4); 
            InsertOffset(ptr->u.rel.op2,4); 
            break;
        case GOTO:        
            break;
        case READ:
            InsertOffset(ptr->u.read.arg,4); 
            break;
        case CALL:
            InsertOffset(ptr->u.call.op1,4); 
            break;
        case WRITE:
            InsertOffset(ptr->u.write.arg,4); 
            break;
        case ARG:
            InsertOffset(ptr->u.arg.argop,4);
            Operand op = (Operand)malloc(sizeof(struct Operand_));
            create_operand(op,ARGOP,arg_num,NULL);
            InsertOffset(op,4);
            arg_num++;
            break;
        case RETURN_CODE:
            InsertOffset(ptr->u.ret.returnop,4); 
            break;
        case FUNCTION:
            break;
        case PARAM:
            InsertOffset(ptr->u.param.var,4); 
            param_total ++;
            break;
        case DEC:
            InsertOffset(ptr->u.dec.var,ptr->u.dec.size->u.value); 
            break;
        default:
            break;
    }
}

void InsertOffset(Operand op,int size){
    struct OffsetNode* node = (struct OffsetNode*)malloc(sizeof(struct OffsetNode));
    node->op = op;
    node->next = NULL;

    struct OffsetNode* temp = offset_head;
    while(temp != NULL){
       if(isEqual(temp->op,op) == 0)
            break;
        temp = temp->next;
    }
    if(temp == NULL){
        if(offset_tail == NULL){
            node->offset = size;
            offset_head = offset_tail = node;
        }
        else{
            node->offset = size + offset_tail->offset;
            offset_tail->next = node;
            offset_tail = offset_tail->next;
        }
    }
}

int getOffset(Operand op){
    struct OffsetNode* temp = offset_head;
    while(temp != NULL){
        if(isEqual(temp->op,op) == 0)
            break;
        temp = temp->next;
    }
    if(temp == NULL){
        printf("not Found\n");
        exit(0);
    }
    else{
        return temp->offset;
    }
}

void fputsMips(FILE* stream,struct InterCode* ptr){
    switch(ptr->kind){
        case ASSIGN:
            load(stream,ptr->u.assign.right,2);
            printf("\tmove $t1, $t2\n");
            fprintf(stream,"\tmove $t1, $t2\n");
            store(stream,ptr->u.assign.left,1);
            break;
        case ADD:
            load(stream,ptr->u.binop.op1,2);
            load(stream,ptr->u.binop.op2,3);
            printf("\tadd $t1, $t2, $t3\n");
            fprintf(stream,"\tadd $t1, $t2, $t3\n");
            store(stream,ptr->u.binop.result,1);
            break;
        case SUB:
            load(stream,ptr->u.binop.op1,2);
            load(stream,ptr->u.binop.op2,3);
            printf("\tsub $t1, $t2, $t3\n");
            fprintf(stream,"\tsub $t1, $t2, $t3\n");
            store(stream,ptr->u.binop.result,1);
            break;
        case MUL:
            load(stream,ptr->u.binop.op1,2);
            load(stream,ptr->u.binop.op2,3);
            printf("\tmul $t1, $t2, $t3\n");
            fprintf(stream,"\tmul $t1, $t2, $t3\n");
            store(stream,ptr->u.binop.result,1);
            break;
        case DIVISION:
            load(stream,ptr->u.binop.op1,2);
            load(stream,ptr->u.binop.op2,3);
            printf("\tdiv $t2, $t3\n");
            fprintf(stream,"\tdiv $t2, $t3\n");
            printf("\tmflo $t1\n");
            fprintf(stream,"\tmflo $t1\n");
            store(stream,ptr->u.binop.result,1);
            break;
        case LABEL:
            fputsOperand(ptr->u.label.labelop,stream);
            printf(":\n");
            fprintf(stream,":\n");
            break;
        case REL:
            load(stream,ptr->u.rel.op1,1);
            load(stream,ptr->u.rel.op2,3);
            char* relop;
            if(strcmp(ptr->u.rel.relop->u.relop,"==") == 0)
                relop = "beq";
            else if(strcmp(ptr->u.rel.relop->u.relop,"!=") == 0)
                relop = "bne";
            else if(strcmp(ptr->u.rel.relop->u.relop,">") == 0)
                relop = "bgt";
            else if(strcmp(ptr->u.rel.relop->u.relop,"<") == 0)
                relop = "blt";
            else if(strcmp(ptr->u.rel.relop->u.relop,">=") == 0)
                relop = "bge";
            else if(strcmp(ptr->u.rel.relop->u.relop,"<=") == 0)
                relop = "ble";
            printf("\t%s $t1, $t3, ",relop);        
            fprintf(stream,"\t%s $t1, $t3, ",relop);        
            fputsOperand(ptr->u.rel.labelop,stream);
            printf("\n");
            fprintf(stream,"\n");
            break;
        case GOTO:        
            printf("\tj ");
            fprintf(stream,"\tj ");
            fputsOperand(ptr->u.go.labelop,stream);
            printf("\n");
            fprintf(stream,"\n");
            break;
        case READ:
            printf("\taddi $sp, $sp, -4\n");
            fprintf(stream,"\taddi $sp, $sp, -4\n");
            printf("\tsw $ra, 0($sp)\n");
            fprintf(stream,"\tsw $ra, 0($sp)\n");
            printf("\tjal read\n");
            fprintf(stream,"\tjal read\n");
            printf("\tlw $ra, 0($sp)\n");
            fprintf(stream,"\tlw $ra, 0($sp)\n");
            printf("\taddi $sp, $sp, 4\n");
            fprintf(stream,"\taddi $sp, $sp, 4\n");
            printf("\tmove $t1, $v0\n");
            fprintf(stream,"\tmove $t1, $v0\n");
            store(stream,ptr->u.read.arg,1);
            break;
        case CALL:
            printf("\taddi $sp, $sp, -4\n");
            fprintf(stream,"\taddi $sp, $sp, -4\n");
            printf("\tsw $ra, 0($sp)\n");
            fprintf(stream,"\tsw $ra, 0($sp)\n");
            printf("\tjal %s\n",ptr->u.call.op2->u.func_name);
            fprintf(stream,"\tjal %s\n",ptr->u.call.op2->u.func_name);
            printf("\tlw $ra, 0($sp)\n");
            fprintf(stream,"\tlw $ra, 0($sp)\n");
            printf("\taddi $sp, $sp, 4\n");
            fprintf(stream,"\taddi $sp, $sp, 4\n");
            printf("\tmove $t1, $v0\n");
            fprintf(stream,"\tmove $t1, $v0\n");
            store(stream,ptr->u.read.arg,1);
            break;
        case WRITE:
            load(stream,ptr->u.write.arg,1);
            printf("\tmove $a0, $t1\n");
            fprintf(stream,"\tmove $a0, $t1\n");
            printf("\taddi $sp, $sp, -4\n");
            fprintf(stream,"\taddi $sp, $sp, -4\n");
            printf("\tsw $ra, 0($sp)\n");
            fprintf(stream,"\tsw $ra, 0($sp)\n");
            printf("\tjal write\n");
            fprintf(stream,"\tjal write\n");
            printf("\tlw $ra, 0($sp)\n");
            fprintf(stream,"\tlw $ra, 0($sp)\n");
            printf("\taddi $sp, $sp, 4\n");
            fprintf(stream,"\taddi $sp, $sp, 4\n");
            printf("\tmove $v0, $0\n");
            fprintf(stream,"\tmove $v1, $0\n");
            break;
        case ARG:
            if (arg_num > 4) {
                load(stream, ptr->u.arg.argop, 1);
                Operand op = (Operand)malloc(sizeof(struct Operand_));
                arg_num --;
                create_operand(op, ARGOP, arg_num, NULL);
                store(stream, op, 1);
            }
            else if (arg_num > 0) {
                load(stream, ptr->u.arg.argop, 1);
                arg_num --;
                printf("\tmove $a%d, $t1\n", arg_num);
                fprintf(stream, "\tmove $a%d, $t1\n", arg_num);
            }
            break;
        case PARAM:
            if (param_num >= 4) {
                printf("\tlw $t1, %d($sp)\n", offset_tail->offset + (param_total - param_num - 4) * 4 + 28);
                fprintf(stream, "\tlw $t1, %d($sp)\n", offset_tail->offset + (param_total - param_num - 4) * 4 + 28);
                param_num ++;
                store(stream, ptr->u.param.var, 1);
            }
            else if (param_num >= 0) {
                printf("\tmove $t1, $a%d\n", param_num);              
                fprintf(stream,"\tmove $t1, $a%d\n", param_num);              
                param_num ++;
                store(stream, ptr->u.param.var, 1);
            }
            break;
        case DEC:
            break;
        case FUNCTION:
            offset_head = NULL;
            offset_tail = NULL;
            InitOffset(stop);
            printf("\n%s:\n",ptr->u.function.func_name->u.func_name);
            fprintf(stream,"\n%s:\n",ptr->u.function.func_name->u.func_name);
            printf("\taddi $sp, $sp, %d\n", -offset_tail->offset);
            fprintf(stream, "\taddi $sp, $sp, %d\n", -offset_tail->offset);
            break;
        case RETURN_CODE:
            load(stream,ptr->u.ret.returnop,1);
            printf("\tmove $v0, $t1\n");
            fprintf(stream,"\tmove $v0, $t1\n");
            printf("\taddi $sp, $sp, %d\n",offset_tail->offset);
            fprintf(stream,"\taddi $sp, $sp, %d\n",offset_tail->offset);
            printf("\tjr $ra\n");
            fprintf(stream,"\tjr $ra\n");
            break;
    }
}

void load(FILE* stream,Operand op,int num){
    int offset = 0;
    switch(op->kind){
        case VARIABLE:case TEMP:
            offset = offset_tail->offset - getOffset(op);
            printf("\tlw $t%d, %d($sp)\n",num,offset);
            fprintf(stream,"\tlw $t%d, %d($sp)\n",num,offset);
            break;
        case CONSTANT:
            printf("\tli $t%d, %d\n",num,op->u.value);
            fprintf(stream,"\tli $t%d, %d\n",num,op->u.value);
            break;
        case ADDRESS:
            offset = offset_tail->offset - getOffset(op);
            printf("\tlw $t5, %d($sp)\n",offset);
            fprintf(stream,"\tlw $t5, %d($sp)\n",offset);
            printf("\tlw $t%d, 0($t5)\n",num);
            fprintf(stream,"\tlw $t%d, 0($t5)\n",num);
            break;
        case LABELOP:
            break;
        case RELOP_OP:
            break;
        case FUNC:
            break;
        case SIZE:
            break;
        case REFERENCE:
            offset = offset_tail->offset - getOffset(op);
            printf("\taddi $t%d, $sp, %d\n",num,offset);
            fprintf(stream,"\taddi $t%d, $sp, %d\n",num,offset);
            break;
        default:
            printf("Wrong Operand\n");
            fprintf(stream,"Wrong Operand\n");
            break;
    }
}

void store(FILE* stream,Operand op,int num){
    int offset = 0;
    switch(op->kind){
        case VARIABLE:case TEMP:
            offset = offset_tail->offset - getOffset(op);
            printf("\tsw $t%d, %d($sp)\n",num,offset);
            fprintf(stream,"\tsw $t%d, %d($sp)\n",num,offset);
            break;
        case CONSTANT:
            break;
        case ADDRESS:
            offset = offset_tail->offset - getOffset(op);
            printf("\tlw $t5, %d($sp)\n",offset);
            fprintf(stream,"\tlw $t5, %d($sp)\n",offset);
            printf("\tsw $t%d, 0($t5)\n",num);
            fprintf(stream,"\tsw $t%d, 0($t5)\n",num);
            break;
        case LABELOP:
            break;
        case RELOP_OP:
            break;
        case FUNC:
            break;
        case SIZE:
            break;
        case REFERENCE:
            break;
        case ARGOP:////////////////////pay attention to the offset
            offset = offset_tail->offset - getOffset(op);
            printf("\tsw $t%d, %d($sp)\n", num, offset);
            fprintf(stream, "\tsw $t%d, %d($sp)\n", num, offset);
            break;
        default:
            printf("Wrong Operand\n");
            fprintf(stream,"Wrong Operand\n");
            break;
    }
}

