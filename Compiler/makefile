BIN  = scanner
CC   = g++
CFLAGS = -DCPLUSPLUS -g     # for use with C++ if file ext is .c

SRCS = $(BIN).y $(BIN).l
CODE = util.c analyze.c symtab.cpp ourGetopt.c yyerror.cpp mystrings.cpp emitcode.cpp codegen.cpp
HDRS = scanType.h globals.h util.h symtab.h ourGetopt.h analyze.h pperror.h yyerror.h mystrings.h emitcode.h codegen.h
TSTS = allErrors.c- basicAll.c- bullsandcows.c- everything02.c- factorial.c- tictactoe.c- basicAll2.c- basicExtra.c- chars.c- factor.c- test.c- while03.c-
OBJS = lex.yy.o $(BIN).tab.o
LIBS = -lfl -lm 

$(BIN): $(OBJS)
	$(CC) $(CCFLAGS) $(OBJS) $(LIBS) $(CODE) -o c-

$(BIN).tab.h $(BIN).tab.c: $(BIN).y util.c
	bison -v -t -d $(BIN).y  

lex.yy.c: $(BIN).l $(BIN).tab.h
	flex $(BIN).l

all:    
	touch $(SRCS)
	make

clean:
	rm -f $(OBJS) $(BIN) lex.yy.c $(BIN).tab.h $(BIN).tab.c $(BIN).tar $(BIN).output *~

tar:
	tar -cvf $(BIN).tar $(SRCS) $(HDRS) makefile 
	ls -l $(BIN).tar

test:
	echo $(TSTS) | xargs -n1 runtest

tartests: 
	tar -cvf setOfTests.tar $(TSTS) $(OUTS) $(OUTP)
	tar -cvf justTests.tar $(TSTS)
	ls -l setOfTests.tar justTests.tar

tarall: 
	tar -cvf $(BIN)-all.tar $(SRCS) $(CODE)  $(HDRS)  makefile	
	ls -l $(BIN)-all.tar

add:
	hg add $(SRCS) $(HDRS) makefile	
