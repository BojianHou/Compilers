%{
    #include<stdio.h>
    #include<stdarg.h>
    #include"head.h"
    #include"lex.yy.c"
    #include "symbol_table.h"
    #include "intercode.h"
    
    struct TreeNode *root;
    struct TreeNode *InsertToTree(char *type, int line,int n, ...);

    struct InterCodeNode* head;
    void displayTree(struct TreeNode *p, int depth);
    extern bool isWrong;
    
%}

/*Declared types*/
%union {struct TreeNode *type_node;}

/*Declared tokens*/
%token <type_node>INT FLOAT TYPE ID SEMI COMMA
%token <type_node>ASSIGNOP
%token <type_node>AND OR NOT
%token <type_node>RELOP
%token <type_node>PLUS MINUS STAR DIV
%token <type_node>DOT LP RP LB RB LC RC
%token <type_node>STRUCT RETURN IF ELSE WHILE

/*Declared non-terminals*/
%type <type_node>Program ExtDefList ExtDef ExtDecList
%type <type_node>Specifier StructSpecifier OptTag Tag
%type <type_node>VarDec FunDec VarList ParamDec
%type <type_node>CompSt StmtList Stmt
%type <type_node>DefList Def DecList Dec
%type <type_node>Exp Args

/*Avoid the conflict*/
%right ASSIGNOP
%left AND OR
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left DOT LP RP LB RB LC RC
%nonassoc STRUCT RETURN IF ELSE WHILE

%%
Program     :   ExtDefList		            {$$ = InsertToTree("Program",@1.first_line, 1, $1); root = $$;}
            ;
ExtDefList  :   ExtDef ExtDefList		    {$$ = InsertToTree("ExtDefList",@1.first_line, 2, $1, $2);}
            |   /*Empty*/		            {$$ = NULL;}
            ;
ExtDef      :   Specifier ExtDecList SEMI   {$$ = InsertToTree("ExtDef", @1.first_line, 3, $1, $2, $3);}
            |   Specifier SEMI		        {$$ = InsertToTree("ExtDef", @1.first_line, 2, $1, $2);}
            |   Specifier FunDec CompSt		{$$ = InsertToTree("ExtDef", @1.first_line, 3, $1, $2, $3);}
            |   error SEMI                  {}
            |   Specifier FunDec error SEMI {}
            ; ExtDecList  :   VarDec		                {$$ = InsertToTree("ExtDecList",@1.first_line, 1, $1);}
            |   VarDec COMMA ExtDecList		{$$ = InsertToTree("ExtDecList",@1.first_line, 3, $1, $2, $3);}
            ;
Specifier   :   TYPE		                {$$ = InsertToTree("Specifier", @1.first_line, 1, $1);}
            |   StructSpecifier		        {$$ = InsertToTree("Specifier", @1.first_line, 1, $1);}
            ;
StructSpecifier :   STRUCT OptTag LC DefList RC		{$$ = InsertToTree("StructSpecifier", @1.first_line, 5, $1, $2, $3, $4, $5);}
                |   STRUCT Tag		        {$$ = InsertToTree("StructSpecifier", @1.first_line, 2, $1, $2);}
                ;
OptTag      :   ID		                    {$$ = InsertToTree("OptTag", @1.first_line, 1, $1);}
 			;
Tag         :   ID		                    {$$ = InsertToTree("Tag", @1.first_line, 1, $1);}
            ;
VarDec      :   ID		                    {$$ = InsertToTree("VarDec", @1.first_line,1, $1);}
            |   VarDec LB INT RB		    {$$ = InsertToTree("VarDec", @1.first_line,4, $1, $2, $3, $4);}
            ;
FunDec      :   ID LP VarList RP		    {$$ = InsertToTree("FunDec", @1.first_line,4, $1, $2, $3, $4);}
            |   ID LP RP		            {$$ = InsertToTree("FunDec", @1.first_line,3, $1, $2, $3);}
            |   error RP                    {}
            ;
VarList     :   ParamDec COMMA VarList		{$$ = InsertToTree("VarList", @1.first_line,3, $1, $2, $3);}
            |   ParamDec		            {$$ = InsertToTree("VarList", @1.first_line,1, $1);}
            ;
ParamDec    :   Specifier VarDec		    {$$ = InsertToTree("ParamDec",@1.first_line, 2, $1, $2);}
            |   error COMMA                 {}
            |   error RB                    {}
            ;
CompSt      :   LC DefList StmtList RC		{$$ = InsertToTree("CompSt",@1.first_line, 4, $1, $2, $3, $4);}
            ;
StmtList    :   Stmt StmtList		        {$$ = InsertToTree("StmtList",@1.first_line, 2, $1, $2);}
            |   /*Empty*/		            {$$ = NULL;}
            ;
Stmt        :   Exp SEMI		            {$$ = InsertToTree("Stmt", @1.first_line, 2, $1, $2);}
            |   CompSt		                {$$ = InsertToTree("Stmt", @1.first_line, 1, $1);}
            |   RETURN Exp SEMI		        {$$ = InsertToTree("Stmt", @1.first_line, 3, $1, $2, $3);}
            |   IF LP Exp RP Stmt		    {$$ = InsertToTree("Stmt", @1.first_line, 5, $1, $2, $3, $4, $5);}
            |   IF LP Exp RP Stmt ELSE Stmt	{$$ = InsertToTree("Stmt", @1.first_line, 7, $1, $2, $3, $4, $5, $6, $7);}
            |   WHILE LP Exp RP Stmt		{$$ = InsertToTree("Stmt", @1.first_line, 5, $1, $2, $3, $4, $5);}
            ;
DefList     :   Def DefList		            {$$ = InsertToTree("DefList", @1.first_line,2, $1, $2);}
            |   /*Empty*/		            {$$ = NULL;}
            ;
Def         :   Specifier DecList SEMI		{$$ = InsertToTree("Def", @1.first_line,3, $1, $2, $3);}
            |   error SEMI                  {}
            ;
DecList     :   Dec		                    {$$ = InsertToTree("DecList", @1.first_line,1, $1);}
            |   Dec COMMA DecList		    {$$ = InsertToTree("DecList", @1.first_line,3, $1, $2, $3);}
            ;
Dec         :   VarDec		                {$$ = InsertToTree("Dec", @1.first_line,1, $1);}
            |   VarDec ASSIGNOP Exp		    {$$ = InsertToTree("Dec", @1.first_line,3, $1, $2, $3);}
            ;
Exp         :   Exp ASSIGNOP Exp		    {$$ = InsertToTree("Exp", @1.first_line,3, $1, $2, $3);}
            |   Exp AND Exp		            {$$ = InsertToTree("Exp", @1.first_line,3, $1, $2, $3);}
            |   Exp OR Exp		            {$$ = InsertToTree("Exp", @1.first_line,3, $1, $2, $3);}
            |   Exp RELOP Exp		        {$$ = InsertToTree("Exp", @1.first_line,3, $1, $2, $3);}
            |   Exp PLUS Exp		        {$$ = InsertToTree("Exp", @1.first_line,3, $1, $2, $3);}
            |   Exp MINUS Exp		        {$$ = InsertToTree("Exp", @1.first_line,3, $1, $2, $3);}
            |   Exp STAR Exp		        {$$ = InsertToTree("Exp", @1.first_line,3, $1, $2, $3);}
            |   Exp DIV Exp		            {$$ = InsertToTree("Exp", @1.first_line,3, $1, $2, $3);}
            |   LP Exp RP		            {$$ = InsertToTree("Exp", @1.first_line,3, $1, $2, $3);}
            |   MINUS Exp		            {$$ = InsertToTree("Exp", @1.first_line,2, $1, $2);}
            |   NOT Exp		                {$$ = InsertToTree("Exp", @1.first_line,2, $1, $2);}
            |   ID LP Args RP		        {$$ = InsertToTree("Exp", @1.first_line,4, $1, $2, $3, $4);}
            |   ID LP RP		            {$$ = InsertToTree("Exp", @1.first_line,3, $1, $2, $3);}
            |   Exp LB Exp RB		        {$$ = InsertToTree("Exp", @1.first_line,4, $1, $2, $3, $4);}
            |   Exp DOT ID		            {$$ = InsertToTree("Exp", @1.first_line,3, $1, $2, $3);}
            |   ID		                    {$$ = InsertToTree("Exp", @1.first_line,1, $1);}
            |   INT		                    {$$ = InsertToTree("Exp", @1.first_line,1, $1);}
            |   FLOAT		                {$$ = InsertToTree("Exp", @1.first_line,1, $1);}
            ;
Args        :   Exp COMMA Args		        {$$ = InsertToTree("Args",@1.first_line, 3, $1, $2, $3);}
            |   Exp		                    {$$ = InsertToTree("Args",@1.first_line, 1, $1);}
            ;
%%

struct TreeNode *InsertToTree(char *type, int line,int n, ...) {
	struct TreeNode *parent = (struct TreeNode *)malloc(sizeof(struct TreeNode));
	int i;
	parent->line = line;
	strcpy(parent->type, type);
    parent->childnum = n;
	parent->childlist = (struct TreeNode **)malloc(sizeof(struct TreeNode*) * parent->childnum);
	va_list VarList;
    va_start(VarList, n);
    for(i = 0;i < parent->childnum;i ++)
        parent->childlist[i] = va_arg(VarList,struct TreeNode*);
	va_end(VarList);
	return parent;
}

void displayTree(struct TreeNode *ptr, int height) {
	if(ptr == NULL) 
        return;
	int i,j;
    bool flag = true;
	for(i = 0; i < ptr->childnum; i++){
	    for(j = 0; j < height; j++){
		    if(flag)
                printf("  ");
        }
        if(ptr -> line != -1) {//the node's type is not a token
            if(flag){
                printf("%s (%d)\n", ptr->type, ptr->line);
                flag = false;
            }
            displayTree(ptr->childlist[i], height + 1);
        }
        else{//the node's type is a token
            if(strcmp(ptr->type, "INT") == 0)
                printf("%s: %d\n", ptr->type, myatoi(ptr->text));
            else if(strcmp(ptr->type, "FLOAT") == 0)
                printf("%s: %f\n", ptr->type, atof(ptr->text));
            else if(strcmp(ptr->type, "TYPE") == 0 || strcmp(ptr->type,"ID") == 0)
                printf("%s: %s\n", ptr->type, ptr->text);
            else
                printf("%s\n", ptr->type);
        }
    }
}
void analyseTree(struct TreeNode *ptr){
    if(ptr == NULL)
        return;
    int i;
    for(i = 0;i < ptr->childnum; i++){
        if(strcmp(ptr->type,"ExtDef") == 0){
            ExtDef_analyse(ptr,ptr->text);
            return;
        }
        else if(ptr->line != -1){
			analyseTree(ptr->childlist[i]);
		} 
    }
} 

//to generate the intercode
struct InterCodeNode* generateIntercode(struct TreeNode *ptr) {
    if(ptr == NULL)
        return NULL;
    else{
        return translate_ExtDefList(ptr->childlist[0]);
    }
}

int myatoi(char *dest){
    int n,i;
    if(strlen(dest) <= 1){
        n = dest[0] - '0';
    }
    else{
        n = 0;
        if(dest[0] == '0' && (dest[1] == 'x' || dest[1] == 'X')){
            for(i = 2;i < strlen(dest);i++){
                switch(dest[i]){
                    case 'a':case 'A':n = n * 16 + 10;break;
                    case 'b':case 'B':n = n * 16 + 11;break;
                    case 'c':case 'C':n = n * 16 + 12;break;
                    case 'd':case 'D':n = n * 16 + 13;break;
                    case 'e':case 'E':n = n * 16 + 14;break;
                    case 'f':case 'F':n = n * 16 + 15;break;
                    default:n = n*16 + dest[i] - '0';
                }
            }
        }
        else if(dest[0] == '0'){
            for(i = 1;i < strlen(dest);i++)
                n = n*8 + dest[i] - '0';
        }
        else{
            for(i = 0;i < strlen(dest);i++)
                n = n*10 + dest[i] - '0'; 
        }
    }
    return n;
}
yyerror(char *msg) {
    isWrong = true;
	printf("Error type B at line %d: Incomplete definition of function \"func\"\n", yylineno);
}
