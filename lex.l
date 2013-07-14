%{
    /*definitons of mainfest constants
      INT,FLOAT,ID,SEMI,COMMA,RELOP,PLUS
      MINUS,STAR,DIV,AND,OR,DOT,NOT,TYPE
      LP,RP,LB,RB,LC,RC,STRUCT,RETURN,IF,ELSE,WHILE
     */
    #include"syntax.tab.h"
    #include"head.h"
    

    int yycolumn = 1;
    #define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno;\
        yylloc.first_column = yycolumn;yylloc.last_column = yycolumn + yyleng -1;\
        yycolumn += yyleng;
    
    bool isWrong;               //编译错误标志
    
    void createTreeNode(char *type, char *text);
    
%}
/* regular definitions */
%option yylineno
delim   [ \t\n]
ws      {delim}+
digit   [0-9]
digit8  [0-7]
digit16 [89a-zA-Z]
letter  [_a-zA-Z]
int     0|([1-9]{digit}*)
int8    0[0-7]*
int16   0[xX][1-9a-fA-F][0-9a-fA-F]*
float   {digit}+(\.{digit}+)
id      {letter}({letter}|{digit})*
wrong8  0{digit8}*{digit16}+{digit8}*
wrong16 0x[1-9a-fA-F]*[g-zG-Z]+[1-9a-fA-F]*
%%

{ws}        {/* no action and no return */}
{int}       {createTreeNode("INT", yytext);return(INT);}
{int8}      {createTreeNode("INT", yytext);return(INT);}
{int16}     {createTreeNode("INT", yytext);return(INT);}
{float}     {createTreeNode("FLOAT", yytext);return(FLOAT);}
int         {createTreeNode("TYPE", yytext);return(TYPE);}
float       {createTreeNode("TYPE", yytext);return(TYPE);}
struct      {createTreeNode("STRUCT", yytext);return(STRUCT);}
return      {createTreeNode("RETURN", yytext);return(RETURN);}
if          {createTreeNode("IF", yytext);return(IF);}
else        {createTreeNode("ELSE", yytext);return(ELSE);}
while       {createTreeNode("WHILE", yytext);return(WHILE);}
{id}        {createTreeNode("ID", yytext);return(ID);}
";"         {createTreeNode("SEMI", yytext);return(SEMI);}
","         {createTreeNode("COMMA", yytext);return(COMMA);}
"="         {createTreeNode("ASSIGNOP", yytext);return(ASSIGNOP);}
">"         {createTreeNode("RELOP", yytext);return(RELOP);}
"<"         {createTreeNode("RELOP", yytext);return(RELOP);}
">="        {createTreeNode("RELOP", yytext);return(RELOP);}
"<="        {createTreeNode("RELOP", yytext);return(RELOP);}
"=="        {createTreeNode("RELOP", yytext);return(RELOP);}
"!="        {createTreeNode("RELOP", yytext);return(RELOP);}
"+"         {createTreeNode("PLUS", yytext);return(PLUS);}
"-"         {createTreeNode("MINUS", yytext);return(MINUS);}
"*"         {createTreeNode("STAR", yytext);return(STAR);}
"/"         {createTreeNode("DIV", yytext);return(DIV);}
"&&"        {createTreeNode("AND", yytext);return(AND);}
"||"        {createTreeNode("OR", yytext);return(OR);}
"."         {createTreeNode("DOT", yytext);return(DOT);}
"!"         {createTreeNode("NOT", yytext);return(NOT);}
"("         {createTreeNode("LP", yytext);return(LP);}
")"         {createTreeNode("RP", yytext);return(RP);}
"["         {createTreeNode("LB", yytext);return(LB);}
"]"         {createTreeNode("RB", yytext);return(RB);}
"{"         {createTreeNode("LC", yytext);return(LC);}
"}"         {createTreeNode("RC", yytext);return(RC);}
{wrong8}    {printf("Error type A at line %d:Illegal octal number \'%s\'\n",yylineno,yytext);isWrong = true;}
{wrong16}   {printf("Error type A at line %d:Illegal hexadeccimal number \'%s\'\n",yylineno,yytext);isWrong = true;}
.           {printf("Error type A at line %d:Mysterious character \'%s\'\n",yylineno,yytext);isWrong = true;}
%%

void createTreeNode(char *type, char *text) {
    //分配地址空间，大小为树节点
	yylval.type_node = (struct TreeNode *)malloc(sizeof(struct TreeNode));
	/*******填充树节点属性值********/
	yylval.type_node->line = -1;
	yylval.type_node->childnum = 1;
	strcpy(yylval.type_node->type, type);
	strcpy(yylval.type_node->text, text);
	yylval.type_node->childlist = NULL;
}
