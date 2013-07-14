parser:lex.l main.c symbol_table.h symbol_table.c syntax.y intercode.h intercode.c mipscode.h mipscode.c
	flex lex.l
	bison -d syntax.y
	gcc main.c syntax.tab.c symbol_table.c intercode.c mipscode.c -lfl -ly -o parser
	
clean:
	rm parser syntax.tab.c syntax.tab.h lex.yy.c
