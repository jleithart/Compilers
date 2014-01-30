/*
 * create the tree
*/
#include "symtab.h"
#include "globals.h"
#include "util.h"

TreeNode * newExpNode(ExpKind kind, int line){
	TreeNode *t = (TreeNode *) malloc(sizeof(TreeNode));
	int i;
	if(t == NULL)
		fprintf(stderr, "Out of memory error at line %d\n", line);
	else{
		for(i = 0; i < MAXCHILDREN; i++){
			t->child[i] = NULL;
		}
		t->sibling = NULL;
		t->nodekind = expK;
		t->kind.exp = kind;
		t->lineno = line;
		t->isConstant = false;
	}

	return t;
}

TreeNode * newStmtNode(StmtKind kind, int line){
	TreeNode *t = (TreeNode *) malloc(sizeof(TreeNode));
	int i;
	if(t == NULL)
		fprintf(stderr, "Out of memory error at line %d\n", line);
	else{
		for(i = 0; i < MAXCHILDREN; i++){
			t->child[i] = NULL;
		}
		t->sibling = NULL;
		t->nodekind = stmtK;
		t->kind.stmt = kind;
		t->lineno = line;
		t->isConstant = false;
	}

	return t;
}

treeNode *newDeclNode(DeclKind kind, int line){
	TreeNode *t = (TreeNode *) malloc(sizeof(TreeNode));
	int i;
	if(t == NULL)
		fprintf(stderr, "Out of memory error at line %d\n", line);
	else{
		for(i = 0; i < MAXCHILDREN; i++){
			t->child[i] = NULL;
		}
		t->sibling = NULL;
		t->nodekind = declK;
		t->kind.decl = kind;
		t->lineno = line;
		t->isPrototype = false;
		t->isStatic = false;
		t->isConstant = false;
	}
	
	return t;
}

char * copyString(char *s){
	int n;
	char *t;
	if(s == NULL) return NULL;
	n = strlen(s) + 1;
	t = (char *)malloc(n);
	if(t == NULL)
		printf("OUT OF MEMORY AT LINE %d\n", 4);
	else strcpy(t,s);
	return t;
}

/* indentation issues */
static int indentno = -3;

#define INDENT indentno+=3
#define UNINDENT indentno-=3

static void printSpaces(void){
	int i;
	for(i = 0; i<indentno;i++){
		if(i % 3 == 0) printf("|");
		printf(" ");
	}
}

void printTree(TreeNode *tree, bool pflag){
	int i;
	char *type;
	char *scopeType;
	char *opchar;
	char *arrayString;
	int siblingno = 1;
	INDENT;
	while(tree != NULL){
		printSpaces();
		if(tree->nodekind == stmtK){
			switch(tree->kind.stmt){
				/* statements */
				case IfK:
					printf("If [line: %d]\n", tree->lineno);
					break;
				case WhileK:
					printf("While [line: %d]\n", tree->lineno);
					break;
				case ForEachK:
					printf("Foreach [line: %d]\n", tree->lineno);
					break;
				case CompoundK:
					printf("Compound [line: %d]\n", tree->lineno);
					break;
				case ReturnK:
					printf("Return [line: %d]\n", tree->lineno);
					break;
				case BreakK:
					printf("Break [line: %d]\n", tree->lineno);
					break;
				default:
					printf("Unknown Node Discovered\n");
					break;
			}
		}
		else if(tree->nodekind == expK){
			switch(tree->kind.exp) {
				/* expressions */
				case OpK:
					switch(tree->attr.op){
						case LEQK:
							opchar = (char *)"<=";
							break;
						case GEQK:
                                                        opchar = (char *)">=";
							break;
						case NEQK:
                                                        opchar = (char *)"!=";
							break;
						case EQK:
                                                        opchar = (char *)"==";
							break;
						case MODK:
                                                        opchar = (char *)"\%";
                                                        break;
						case ASTERISKK:
                                                        opchar = (char *)"*";
                                                        break;
						case MINUSK:
                                                        opchar = (char *)"-";
                                                        break;
						case PLUSK:
                                                        opchar = (char *)"+";
                                                        break;
						case SLASHK:
                                                        opchar = (char *)"/";
                                                        break;
						case LTHANK:
                                                        opchar = (char *)"<";
                                                        break;
						case GTHANK:
                                                        opchar = (char *)">";
                                                        break;
						case NOTK:
                                                        opchar = (char *)"not";
                                                        break;
						case ANDK:
                                                        opchar = (char *)"and";
                                                        break;
						case ORK:
                                                        opchar = (char *)"or";
                                                        break;
					}
					printf("Op: %s [line: %d]\n", opchar, tree->lineno);
					break;
				case ConstantK:
					switch(tree->expType){
						case Integer:
							printf("Const: %d [line: %d]\n", tree->attr.val, tree->lineno);
							break;
						case Char:
							printf("Const: %s [line: %d]\n", tree->attr.string, tree->lineno);
							break;
						case Boolean:
							char *bString = (char *)"false";
							if(tree->attr.bVal)
								bString = (char *)"true";
							printf("Const: %s [line: %d]\n", bString, tree->lineno);
							break;
					}
					break;
				case IdK:
                                        switch(tree->expType){
                                                case Integer:
                                                        type = (char *)"int";
                                                        break;
                                                case Char:
                                                        type = (char *)"char";
                                                        break;
                                                case Boolean:
                                                        type = (char *)"bool";
                                                        break;
                                        }
                                        if(pflag){
                                                if(tree->sType == Global) scopeType = (char *)"global";
                                                if(tree->sType == Local) scopeType = (char *)"local";
						if(tree->sType == Param) scopeType = (char *)"param";
                                                if(tree->sType == GStatic) scopeType = (char *)"static";
                                                printf("Var %s of type %s [line: %d], scope: %s, location: %d, size: %d\n", tree->attr.name, type, tree->lineno, scopeType, tree->loc, tree->size);
                                        }
					else
						printf("Id: %s [line: %d]\n", tree->attr.name, tree->lineno);
					break;
				case AssignK:
					switch(tree->attr.op){
						case DECK:
							opchar = (char *)"--";
							break;
                                                case INCK:
                                                        opchar = (char *)"++";
                                                        break;
                                                case MASSIGNK:
                                                        opchar = (char *)"-=";
                                                        break;
                                                case PASSIGNK:
                                                        opchar = (char *)"+=";
                                                        break;
                                                case EQUALK:
                                                        opchar = (char *)"=";
                                                        break;
					}
					printf("Assign: %s [line: %d]\n", opchar, tree->lineno);
					break;
				case CallK:
					printf("Call: %s [line: %d]\n", tree->attr.name, tree->lineno);
					break;
				default:
					printf("Unknown Node Discovered!\n");
			}
		}
		else if(tree->nodekind == declK){
			switch(tree->kind.decl){
				/* declarations */
				case VarK:
					arrayString = (char *)"";
					if(tree->isArray){
						arrayString = (char *)"is array ";
					}
                                        switch(tree->expType){
                                                case Integer:
                                                        type = (char *)"int";
                                                        break;
                                                case Char:
                                                        type = (char *)"char";
                                                        break;
                                                case Boolean:
                                                        type = (char *)"bool";
                                                        break;
                                        }
					if(!pflag){
						printf("Var %s %sof type %s [line: %d]\n", tree->attr.name, arrayString, type, tree->lineno);
					}
					else{
                                                if(tree->sType == Global) scopeType = (char *)"global";
                                                if(tree->sType == Local) scopeType = (char *)"local";
                                                if(tree->sType == GStatic) scopeType = (char *)"static";
                                                printf("Var %s %sof type %s [line: %d], scope: %s, location: %d, size: %d\n", tree->attr.name, arrayString, type, tree->lineno, scopeType, tree->loc, tree->size);
					}
					break;
				case FuncK:
					switch(tree->expType){
                                                case Integer:
                                                        type = (char *)"int";
                                                        break;
                                                case Char:
                                                        type = (char *)"char";
                                                        break;
                                                case Boolean:
                                                        type = (char *)"bool";
                                                        break;
						default:
							type = (char *)"void";
							break;
                                        }
					if(!pflag){
						printf("Func %s returns type %s [line: %d]\n", tree->attr.name, type, tree->lineno);
					}
					else{
						if(tree->sType == Global) scopeType = (char *)"global";
						if(tree->sType == Local) scopeType = (char *)"local";
						if(tree->sType == GStatic) scopeType = (char *)"static";
						if(tree->sType == Param) scopeType = (char *)"parameter";
						
						printf("Func %s returns type %s [line: %d], scope: %s, location: %d, size: %d\n", tree->attr.name, type, tree->lineno, scopeType, tree->loc, tree->size);
					}
					break;
				case ParamK:
                                        arrayString = (char *)"";
                                        if(tree->isArray){
                                                arrayString = (char *)"is array ";
                                        }
					switch(tree->expType){
						case Integer:
							type = (char *)"int";
							break;
						case Char:
							type = (char *)"char";
							break;
						case Boolean:
							type = (char *)"bool";
							break;
						default:
							type = (char *) "void";
							break;
					}
					if(!pflag){
						printf("Param %s %sof type %s [line: %d]\n", tree->attr.name, arrayString, type, tree->lineno);
					}
					else{
                                                printf("Param %s %sof type %s [line: %d], scope: parameter, location: %d, size %d\n", tree->attr.name, arrayString, type, tree->lineno, tree->loc, tree->size);
					}
					break;
				default:
					printf("Unknown Node discovered!\n");
			}
		}
		else fprintf(stdout, "Unknown node kind\n");
		for(i = 0; i < MAXCHILDREN; i++){
			if(tree->child[i] != NULL){
				printSpaces();
				printf("Child: %d\n", i+1);
			}
			printTree(tree->child[i], pflag);
		}
		if(tree->sibling != NULL && indentno != 0){
			UNINDENT;
			printSpaces();
			printf("Sibling: %d\n", siblingno);
			INDENT;
			siblingno++;
		}
		tree = tree->sibling;
	}
	UNINDENT;
}
