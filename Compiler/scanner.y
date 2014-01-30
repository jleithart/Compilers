/*
	scanner.y
*/
%{
#include "globals.h"
#include "util.h"
#include "analyze.h"
#include "emitcode.h"
#include "codegen.h"
#include "symtab.h"
#include "scanType.h"
#include "ourGetopt.h"
#include "yyerror.h"
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#ifdef CPLUSPLUS
extern int yylex();
#endif

#define YYERROR_VERBOSE
extern int numErrors;
int lineno = 1;
static bool isStatic = false;
char *lastScannedToken;
FILE *code;

/*
void yyerror(const char *msg){
	printf("ERROR lineno(%d):%s. I got %s\n", lineno, msg, lastScannedToken);
}
*/


static TreeNode *syntaxTree;
static TreeNode *PPTree = (TreeNode *)malloc(sizeof(TreeNode));

void printerr(void *p){
	printf("[0x%08]", p);

	return;
}

SymTab *symtab = new SymTab(printerr);

void PreProcess(){

	PPTree->lineno = -1;
        PPTree->nodekind = declK;
        PPTree->kind.decl = FuncK;
        PPTree->expType = Integer;
        PPTree->attr.name = (char *)"input";
        PPTree->isPrototype = true;
	PPTree->prototypeID = 0;

	TreeNode *P3Tree = (TreeNode *) malloc(sizeof(TreeNode));
	PPTree->sibling = P3Tree;
	P3Tree->nodekind = declK;
	P3Tree->kind.decl = FuncK;
	P3Tree->lineno = -1;
	P3Tree->expType = Void;
	P3Tree->attr.name = (char *)"output";
	P3Tree->isPrototype = true;
	P3Tree->prototypeID = 1;

	/* Parameter children */
	TreeNode *t = (TreeNode *) malloc(sizeof(TreeNode));
	P3Tree->child[0] = t;
	t->nodekind = declK;
	t->kind.decl = ParamK;
	t->lineno = -1;
	t->expType = Integer;
	t->isPrototype = true;

	/* PPPTree */
	TreeNode * P4Tree = (TreeNode *) malloc(sizeof(TreeNode));
	P3Tree->sibling = P4Tree;
	P4Tree->lineno = -1;
        P4Tree->nodekind = declK;
        P4Tree->kind.decl = FuncK;
        P4Tree->expType = Boolean;
        P4Tree->attr.name = (char *)"inputb";
        P4Tree->isPrototype = true;
	P4Tree->prototypeID = 2;

	/* outputb */
	/* do the rest of the preprocessing functions */
	TreeNode *P5Tree = (TreeNode *) malloc(sizeof(TreeNode));
	P4Tree->sibling = P5Tree;
        P5Tree->lineno = -1;
        P5Tree->nodekind = declK;
        P5Tree->kind.decl = FuncK;
        P5Tree->expType = Void;
        P5Tree->attr.name = (char *)"outputb";
	P5Tree->isPrototype = true;
	P5Tree->prototypeID = 3;

        /* Parameter children */
        TreeNode *tt = (TreeNode *) malloc(sizeof(TreeNode));
        P5Tree->child[0] = tt;
        tt->nodekind = declK;
        tt->kind.decl = ParamK;
        tt->lineno = -1;
        tt->expType = Boolean;
	tt->isPrototype = true;

        /* PPPTree */
        TreeNode *P6Tree = (TreeNode *) malloc(sizeof(TreeNode));
        P5Tree->sibling = P6Tree;

	P6Tree->lineno = -1;
        P6Tree->nodekind = declK;
        P6Tree->kind.decl = FuncK;
        P6Tree->expType = Char;
        P6Tree->attr.name = (char *)"inputc";
        P6Tree->isPrototype = true;
	P6Tree->prototypeID = 4;

	TreeNode *P7Tree = (TreeNode *) malloc(sizeof(TreeNode));
	P6Tree->sibling = P7Tree;

        P7Tree->lineno = -1;
        P7Tree->nodekind = declK;
        P7Tree->kind.decl = FuncK;
        P7Tree->expType = Void;
        P7Tree->attr.name = (char *)"outputc";
	P7Tree->isPrototype = true;
	P7Tree->prototypeID = 5;

        /* Parameter children */
        TreeNode *ttt = (TreeNode *) malloc(sizeof(TreeNode));
        P7Tree->child[0] = ttt;
        ttt->nodekind = declK;
        ttt->kind.decl = ParamK;
        ttt->lineno = -1;
        ttt->expType = Char;
	ttt->isPrototype = true;

        /* PPPTree */
        TreeNode *P8Tree = (TreeNode *) malloc(sizeof(TreeNode));
        P7Tree->sibling = P8Tree;

        P8Tree->lineno = -1;
        P8Tree->nodekind = declK;
        P8Tree->kind.decl = FuncK;
        P8Tree->expType = Void;
        P8Tree->attr.name = (char *)"outnl";
	P8Tree->isPrototype = true;
	P8Tree->prototypeID = 6;

        /* PPPTree */
        P8Tree->sibling = syntaxTree;
	
}


%}
%union{
	ExpType type;
	int number;
	TokenData *tokenData;
	TreeNode *tree;
}

%token <tokenData> BOOLEAN CHAR INT ID NUMCONST CHARCONST STRINGCONST IF TRUE FALSE PASSIGN MASSIGN AND LEQ GEQ OR NEQ NOT EQ ELSE WHILE STATIC BREAK DEC INC FOREACH IN RETURN COMMA SEMICOLON LPAREN RPAREN LBRACE RBRACE COLON EQUAL MOD ASTERISK MINUS PLUS SLASH LTHAN GTHAN LBRACK RBRACK

%type <tree> program declaration_list declaration var_declaration scoped_var_declaration var_decl_list var_decl_initialize var_decl_id fun_declaration params param_list param_type_list param_id_list param_id statement other_stmt matched_stmt unmatched_stmt compound_stmt local_declarations statement_list expression_stmt return_stmt break_stmt expression simple_expression and_expression unary_rel_expression rel_expression relop sum_expression sumop term mulop unary_expression unaryop factor mutable immutable call args arg_list constant

%type <type> type_specifier scoped_type_specifier
%%
program: declaration_list	{ syntaxTree = $1; }
	;

declaration_list: declaration_list declaration 
			{
				TreeNode *t = $1;
				if( t != NULL){
					while(t->sibling != NULL)
						t = t->sibling;
					t->sibling = $2;
					$$ = $1;
				}
				else
					$$ = $2;
			}
	| declaration			{ $$ = $1; }
	| error				{ $$ = NULL; } /*ERROR*/
	;

declaration: var_declaration 		{ $$ = $1; }
	| fun_declaration		{ $$ = $1; }
	;

var_declaration: type_specifier var_decl_list SEMICOLON
				{
					TreeNode *t = $2;
					while(t != NULL){
						t->expType = $1;
						t = t->sibling;
					}
                                        $$ = $2;
					yyerrok;
                               	}
	| type_specifier error SEMICOLON 	{ $$ = NULL; yyerrok;} /*ERROR*/
	;

scoped_var_declaration: scoped_type_specifier var_decl_list SEMICOLON 
				{
					TreeNode *t = $2;
		                        while(t != NULL){
                	                	t->expType = $1;
                        		        t = t->sibling;
		                        }
                		        $$ = $2;
					if($$ != NULL)
						$$->isStatic = isStatic;
					yyerrok;
				}
	| scoped_type_specifier error SEMICOLON { $$ = NULL; yyerrok;} /*ERROR*/
	;

var_decl_list: var_decl_list COMMA var_decl_initialize 
			{
				TreeNode * t = $1;
				if(t != NULL){
					while(t->sibling != NULL)
						t = t->sibling;
					t->sibling = $3;
					$$ = $1;
				}
				else
					$$ = $3;
				yyerrok;
			}
	| error COMMA var_decl_initialize { $$ = NULL; yyerrok;} /*ERROR*/
	| var_decl_initialize { $$ = $1; }
	;

var_decl_initialize: var_decl_id { $$ = $1; }
	| var_decl_id COLON simple_expression 
		{
			/* want to go over this later */
			$$ = $1;
			$$->child[0] = $3;
		}
	| error COLON simple_expression
			{ $$ = NULL; yyerrok;} /*ERROR*/
	| var_decl_id COLON error { $$ = NULL; } /*ERROR*/
	;

var_decl_id: ID	{ 
			$$ = newDeclNode(VarK, $1->linenumber);	
			$$->attr.name = copyString($1->tokenstr);
			
		}
	| ID LBRACK NUMCONST RBRACK
		{
                        $$ = newDeclNode(VarK, $1->linenumber);
                        $$->attr.name = copyString($1->tokenstr);
			$$->isArray = true;
			$$->iSize = $3->numvalue;
		}
	| error LBRACK NUMCONST RBRACK { $$ = NULL; yyerrok;} /*ERROR*/
	| ID LBRACK error { $$ = NULL; } /*ERROR*/
	;

scoped_type_specifier: STATIC type_specifier 
		{ 
			$$ = $2;
			isStatic = true;
		}
	| type_specifier		{ $$ = $1; isStatic = false;}
	;

type_specifier: INT { $$ = Integer; }
	| BOOLEAN { $$ = Boolean; }
	| CHAR { $$ = Char; }
	;

fun_declaration: type_specifier ID LPAREN params RPAREN statement 
		{
			$$ = newDeclNode(FuncK, $2->linenumber);
			$$->child[0] = $4;
			$$->child[1] = $6;
			$$->attr.name = copyString($2->tokenstr);
			$$->expType = $1;
		}
	| ID LPAREN params RPAREN statement 
		{
			$$ = newDeclNode(FuncK, $1->linenumber);
			$$->child[0] = $3;
			$$->child[1] = $5;
			$$->attr.name = copyString($1->tokenstr);
			$$->expType = Void;
		}
	| type_specifier error LPAREN params RPAREN statement { $$ = NULL; } /*ERROR*/
	| type_specifier ID LPAREN error { $$ = NULL; } /*ERROR*/
	| ID LPAREN error RPAREN statement { $$ = NULL; yyerrok;} /*ERROR*/
	| error RPAREN statement { $$ = NULL; yyerrok;} /*ERROR*/
	;

params: param_list	{ $$ = $1; }
	| /*epsilon*/	{ $$ = NULL; }
	;

param_list: param_list SEMICOLON param_type_list 
		{ 
			/* loop */
			TreeNode *t = $1;
			if(t != NULL){
				while(t->sibling != NULL)
					t = t->sibling;
				t->sibling = $3;
				$$ = $1;
			}
			else{
				$$ = $3;
			}
		}
	| error SEMICOLON param_type_list { $$ = NULL; yyerrok;} /*ERROR*/
	| param_type_list { $$ = $1; }
	;

param_type_list: type_specifier param_id_list 
		{
			TreeNode *t = $2;
			while(t != NULL){
				t->expType = $1;
				t = t->sibling;
			}		
			$$ = $2;
		}
	| type_specifier error { $$ = NULL;} /*ERROR*/
	;

param_id_list: param_id_list COMMA param_id 
		{
			TreeNode *t = $1;
			if(t != NULL){
				while(t->sibling != NULL)
					t = t->sibling;
				t->sibling = $3;
				$$ = $1;
			}
			else
				$$ = $3;
			yyerrok;
		}
	| error COMMA param_id { $$ = NULL; yyerrok;} /*ERROR*/
	| param_id_list COMMA error { $$ = NULL; yyerrok;} /*ERROR*/
	| param_id { $$ = $1; }
	;

param_id: ID 
		{
			$$ = newDeclNode(ParamK, $1->linenumber);
			$$->attr.name = $1->tokenstr;
		}
	| ID LBRACK RBRACK
		{
			$$ = newDeclNode(ParamK, $1->linenumber);
			$$->attr.name = copyString($1->tokenstr);
			$$->isArray = true;
		}
	| error LBRACK RBRACK { $$ = NULL; yyerrok;} /*ERROR*/
	| ID LBRACK error { $$ = NULL; } /*ERROR*/
	;

statement: matched_stmt	{ $$ = $1; }		
	| unmatched_stmt { $$ = $1; }
	| matched_stmt error { $$ = $1;} /*ERROR*/
	| unmatched_stmt error { $$ = $1;} /*ERROR*/
	;

other_stmt: break_stmt { $$ = $1; }
	| return_stmt { $$ = $1; }
	| compound_stmt { $$ = $1; }
	| expression_stmt { $$ = $1; }
	;

matched_stmt: IF LPAREN simple_expression RPAREN matched_stmt ELSE matched_stmt
		{
			$$ = newStmtNode(IfK, $1->linenumber);
                        $$->child[0] = $3;
                        $$->child[1] = $5;
                        $$->child[2] = $7;
		}
	| IF LPAREN error { $$ = NULL; } /*ERROR*/
	| error RPAREN matched_stmt ELSE matched_stmt { $$ = NULL; yyerrok;} /*ERROR*/
	| WHILE LPAREN simple_expression RPAREN matched_stmt 
		{
			$$ = newStmtNode(WhileK, $1->linenumber);
                        $$->child[0] = $3;
                        $$->child[1] = $5;
		}
	| WHILE LPAREN error { $$ = NULL;} /*ERROR*/
	| FOREACH LPAREN mutable IN simple_expression RPAREN matched_stmt 
		{
			$$ = newStmtNode(ForEachK, $1->linenumber);
			$$->child[0] = $3;
			$$->child[1] = $5;
			$$->child[2] = $7;
		}
	| FOREACH LPAREN error { $$ = NULL; } /*ERROR*/
        | other_stmt { $$ = $1; }
        ;

unmatched_stmt: IF LPAREN simple_expression RPAREN statement 
		{
			$$ = newStmtNode(IfK, $1->linenumber);
			$$->child[0] = $3;
			$$->child[1] = $5;
		}
	| IF LPAREN simple_expression RPAREN error ELSE unmatched_stmt { $$ = NULL; yyerrok;} /*ERROR*/
	| IF LPAREN error { $$ = NULL;} /*ERROR*/
        | IF LPAREN simple_expression RPAREN matched_stmt ELSE unmatched_stmt 
		{
			$$ = newStmtNode(IfK, $1->linenumber);
			$$->child[0] = $3;
			$$->child[1] = $5;
			$$->child[2] = $7;
		}
	| WHILE LPAREN simple_expression RPAREN unmatched_stmt 
		{
			$$ = newStmtNode(WhileK, $1->linenumber);
			$$->child[0] = $3;
			$$->child[1] = $5;
		}
	| WHILE LPAREN error { $$ = NULL;} /*ERROR*/
	| FOREACH LPAREN mutable IN simple_expression RPAREN unmatched_stmt 
		{
			$$ = newStmtNode(ForEachK, $1->linenumber);
			$$->child[0] = $3;
			$$->child[1] = $5;
			$$->child[2] = $7;
		}
	| FOREACH LPAREN error { $$ = NULL; } /*ERROR*/
	| error RPAREN statement { $$ = NULL; yyerrok;} /*ERROR*/
        ;

compound_stmt: LBRACE local_declarations statement_list RBRACE 
			{
				$$ = newStmtNode(CompoundK, $1->linenumber);
				$$->child[0] = $2;
				$$->child[1] = $3;
				yyerrok;
			}
	| error RBRACE { $$ = NULL; yyerrok;} /*ERROR*/
	| LBRACE error statement_list RBRACE { $$ = NULL; yyerrok;} /*ERROR*/
	;

local_declarations: local_declarations scoped_var_declaration 
			{
                                TreeNode *t = $1;
                                if(t != NULL){
                                        while(t->sibling != NULL)
                                                t = t->sibling;
                                        t->sibling = $2;
                                        $$ = $1;
                                }
                                else
                                        $$ = $2;
                        }
	| /*epsilon*/ { $$ = NULL; }
	;

statement_list: statement_list statement 
			{
				TreeNode *t = $1;
				if(t != NULL){
					while(t->sibling != NULL)
						t = t->sibling;
					t->sibling = $2;
					$$ = $1;
				}
				else
					$$ = $2;
			}
	| statement_list error { $$ = NULL;} /*ERROR*/
	| /*epsilon*/ { $$ = NULL; }
	;

expression_stmt: expression SEMICOLON	{ $$ = $1; yyerrok;}
	| error SEMICOLON { $$ = NULL; yyerrok;} /*ERROR*/
	| SEMICOLON	{ $$ = NULL; yyerrok; /* or try $$ = $$ */}
	;

return_stmt: RETURN SEMICOLON 
		{	
			yyerrok;
			$$ = newStmtNode(ReturnK, $1->linenumber); 
		}
	| RETURN expression SEMICOLON 
		{
			yyerrok;
			$$ = newStmtNode(ReturnK, $1->linenumber);
			$$->child[0] = $2;
		}
	| RETURN error	{ $$ = NULL;} /*ERROR*/
	;


break_stmt: BREAK SEMICOLON	
		{ 
			$$ = newStmtNode(BreakK, $1->linenumber); 
		}
	;

expression: mutable EQUAL expression
			{
                                $$ = newExpNode(AssignK, $2->linenumber);
				$$->attr.op = EQUALK;
                                $$->child[0] = $1;
                                $$->child[1] = $3;
                        }
	| mutable PASSIGN expression
			{
                                $$ = newExpNode(AssignK, $2->linenumber);
				$$->attr.op = PASSIGNK;
                                $$->child[0] = $1;
                                $$->child[1] = $3;
                        }
	| mutable MASSIGN expression	
			{
                                $$ = newExpNode(AssignK, $2->linenumber);
				$$->attr.op = MASSIGNK;
                                $$->child[0] = $1;
                                $$->child[1] = $3;
                        }
	| error EQUAL expression { $$ = NULL; yyerrok;} /*ERROR*/
	| error PASSIGN expression { $$ = NULL; yyerrok;} /*ERROR*/
	| error MASSIGN expression { $$ = NULL; yyerrok;} /*ERROR*/
	| mutable EQUAL error { $$ = NULL; } /*ERROR*/
	| mutable PASSIGN error { $$ = NULL; } /*ERROR*/
	| mutable MASSIGN error { $$ = NULL; } /*ERROR*/
	| mutable INC	
			{
                                $$ = newExpNode(AssignK, $2->linenumber);
				$$->attr.op = INCK;
                                $$->child[0] = $1;
                        }
	| error INC	{ $$ = NULL; yyerrok;} /*ERROR*/	
	| mutable DEC	
			{
				$$ = newExpNode(AssignK, $2->linenumber);
				$$->attr.op = DECK;
				$$->child[0] = $1;
			}
	| error DEC	{ $$ = NULL; yyerrok;} /*ERROR*/
	| simple_expression { $$ = $1; }
	| mutable error expression { $$ = NULL; yyerrok;} /*ERROR*/
	;

simple_expression: simple_expression OR and_expression 
			{
                                $$ = newExpNode(OpK, $2->linenumber);
                                $$->attr.op = ORK;
                                $$->child[0] = $1;
                                $$->child[1] = $3;
				if($1 != NULL && $3 != NULL)
					$$->isConstant = $1->isConstant && $3->isConstant;
                        }
	| error OR and_expression { $$ = NULL; yyerrok;} /*ERROR*/
	| simple_expression OR error { $$ = NULL; } /*ERROR*/
	| and_expression { $$ = $1; if($1 != NULL) $$->isConstant = $1->isConstant; }
	;

and_expression: and_expression AND unary_rel_expression 
			{
				$$ = newExpNode(OpK, $2->linenumber);
				$$->attr.op = ANDK;
				$$->child[0] = $1;
				$$->child[1] = $3;
				if($1 != NULL && $3 != NULL)
					$$->isConstant = $1->isConstant && $3->isConstant;
                        }
	| error AND unary_rel_expression { $$ = NULL; yyerrok;} /*ERROR*/
	| and_expression AND error { $$ = NULL; } /*ERROR*/
	| unary_rel_expression	{ $$ = $1; if($1 != NULL){ 
						$$->isConstant = $1->isConstant; 
						$$->isArray = $1->isArray;
						}
				}
	;

unary_rel_expression: NOT unary_rel_expression 
			{
				$$ = newExpNode(OpK, $1->linenumber);
				$$->attr.op = NOTK;
				$$->child[0] = $2;
				if($2 != NULL){
					$$->isConstant = $2->isConstant;
					$$->isArray = $2->isArray;
				}
                        }
	| NOT error { $$ = NULL; } /*ERROR*/
	| rel_expression { $$ = $1; if($1 != NULL){ 
					$$->isConstant = $1->isConstant; 
					$$->isArray = $1->isArray;
					}
			}
	;

rel_expression: sum_expression relop sum_expression 
			{
				$$ = $2;
				$$->child[0] = $1;
                                $$->child[1] = $3;
				if($1 != NULL && $3 != NULL)
					$$->isConstant = $1->isConstant && $3->isConstant;
                        }
	| error relop sum_expression { $$ = NULL; yyerrok;} /*ERROR*/
	| sum_expression relop error { $$ = NULL; } /*ERROR*/
	| sum_expression { $$ = $1; if($1 != NULL){ 
					$$->isConstant = $1->isConstant; 
					$$->isArray = $1->isArray;
					}
			}
	;

relop: LEQ 
		{
                        $$ = newExpNode(OpK, $1->linenumber);
                        $$->attr.op = LEQK;
                }
	| LTHAN  
		{
                        $$ = newExpNode(OpK, $1->linenumber);
                        $$->attr.op = LTHANK;
                }
	| GTHAN  
		{
                        $$ = newExpNode(OpK, $1->linenumber);
                        $$->attr.op = GTHANK;
                }
	| GEQ                           
		{
                        $$ = newExpNode(OpK, $1->linenumber);
                        $$->attr.op = GEQK;
                }
	| EQ    
		{
                        $$ = newExpNode(OpK, $1->linenumber);
                        $$->attr.op = EQK;
                }
	| NEQ	
		{
			$$ = newExpNode(OpK, $1->linenumber);
			$$->attr.op = NEQK;
		}
	;

sum_expression: sum_expression sumop term 
		{
			$$ = $2;
			$$->child[0] = $1;
			$$->child[1] = $3;
			if($3 != NULL && $1 != NULL)
				$$->isConstant = $3->isConstant && $1->isConstant;
		}
	| error sumop term { $$ = NULL; yyerrok;} /*ERROR*/
	| sum_expression sumop error { $$ = NULL; } /*ERROR*/
	| term	{ $$ = $1; if($1 != NULL){ 
				$$->isConstant = $1->isConstant; 
				$$->isArray = $1->isArray;
				}
		}
	;

sumop: PLUS	
		{
			$$ = newExpNode(OpK, $1->linenumber);
			$$->attr.op = PLUSK;
		}
	| MINUS	
		{
			$$ = newExpNode(OpK, $1->linenumber);
                        $$->attr.op = MINUSK;
		}
	;

term: term mulop unary_expression	
		{
			$$ = $2;
			$$->child[0] = $1;
			$$->child[1] = $3;
			if($1 != NULL && $3 != NULL)
				$$->isConstant = ($1->isConstant && $3->isConstant);
		}
	| error mulop unary_expression { $$ = NULL; yyerrok;} /*ERROR*/
	| term mulop error { $$ = NULL; } /*ERROR*/
	| unary_expression { $$ = $1; if($1 != NULL){ 
					$$->isConstant = $1->isConstant; 
					$$->isArray = $1->isArray;
					}}
	;

mulop: ASTERISK	
		{
			$$ = newExpNode(OpK, $1->linenumber);
			$$->attr.op = ASTERISKK;
		}
	| SLASH	
		{
			$$ = newExpNode(OpK, $1->linenumber);
                	$$->attr.op = SLASHK;
		}
	| MOD				
		{
			$$ = newExpNode(OpK, $1->linenumber);
                	$$->attr.op = MODK;
		}
	;

unary_expression: unaryop unary_expression 
		{
			$$ = $1;
			$$->child[0] = $2;
			if($2 != NULL){
				$$->isConstant = $2->isConstant;
				$$->isArray = $2->isArray;
			}
		}
	| unaryop error { $$ = NULL; } /*ERROR*/
	| factor { $$ = $1; if($1 != NULL) $$->isArray = $1->isArray; }
	;

unaryop: MINUS	
		{
			$$ = newExpNode(OpK, $1->linenumber);
			$$->attr.op = MINUSK;
		}
	| ASTERISK			
		{
			$$ = newExpNode(OpK, $1->linenumber);
                	$$->attr.op = ASTERISKK;
		}
	;

factor: immutable { $$ = $1; }
	| mutable { $$ = $1; if($1 != NULL) $$->isArray = $1->isArray; }
	;

mutable: ID				
		{
			$$ = newExpNode(IdK, $1->linenumber);
			$$->attr.name = copyString($1->tokenstr);
			$$->isArray = false;
		}
	| ID LBRACK expression RBRACK	
		{
			$$ = newExpNode(IdK, $1->linenumber);
			$$->attr.name = copyString($1->tokenstr);
			$$->child[0] = $3;
			$$->isArray = true;
		}
	| ID LBRACK error { $$ = NULL; } /*ERROR*/
	| error LBRACK { $$ = NULL; yyerrok;} /*ERROR*/
	;

immutable: LPAREN expression RPAREN { $$ = $2; yyerrok; }
	| LPAREN error { $$ = NULL; } /*ERROR*/
	| error RPAREN { $$ = NULL; yyerrok;} /*ERROR*/
	| call		{ $$ = $1; }
	| constant	
		{ 
			$$ = $1;
			$$->isConstant = true;
			if($1 != NULL)
				$$->isArray = $1->isArray;
		}
	;

call:	ID LPAREN args RPAREN	
		{
			yyerrok;
			/* function call */
			$$ = newExpNode(CallK, $1->linenumber);
			//$$->attr.name = yylval.tokenData->strvalue;
			$$->attr.name = copyString($1->tokenstr);
			/* there are children here */
			$$->child[0] = $3;
		}
	| error LPAREN args RPAREN { $$ = NULL; yyerrok;} /*ERROR*/
	| ID LPAREN error { $$ = NULL; } /*ERROR*/
	;

args: arg_list	{ $$ = $1; }
	| /*epislon*/ {	$$ = NULL; }
	;

arg_list: arg_list COMMA expression	
		{ TreeNode *t = $1;
		  if( t != NULL){
			while(t->sibling != NULL)
				t = t->sibling;
			t->sibling = $3;
			$$ = $1;
		  }
		  else
			$$ = $3;
		  yyerrok;
		}
	| error COMMA expression { $$ = NULL; yyerrok;} /*ERROR*/
	| expression { $$ = $1; }
	;

constant: NUMCONST	
		{
			$$ = newExpNode(ConstantK, $1->linenumber);
			$$->attr.val = $1->numvalue;
			$$->expType = Integer;
		}
	| CHARCONST
		{
			$$ = newExpNode(ConstantK, $1->linenumber);
			$$->attr.string = copyString($1->charvalue);
			$$->expType = Char;
		}
	| STRINGCONST	
		{
			$$ = newExpNode(ConstantK, $1->linenumber);
			$$->attr.string = copyString($1->strvalue);
			$$->expType = Char;
			$$->isArray = true;
		}
	| TRUE	{
			$$ = newExpNode(ConstantK, $1->linenumber);
			$$->expType = Boolean;
			$$->attr.bVal = true;
		}
	| FALSE	{
			$$ = newExpNode(ConstantK, $1->linenumber);
			$$->expType = Boolean;
			$$->attr.bVal = false;
		}
	;
	

%% 


int main(int argc, char *argv[]){
	extern char *optarg;
	extern int optind, opterr, optopt;	
	
	extern FILE *yyin;
	if(argc > 1){
		yyin = fopen(argv[1], "r");
	}


	int c;
	int index = 0;
	bool dflag = false;
	bool pflag = false;
	bool sflag = false;
	bool oflag = false;
	bool Pflag = false;
	char *ovalue = NULL;
	char *file;
	yydebug = 0;
	opterr = 0;
	
	while( (c = ourGetopt(argc, argv, (char *)"dpPso:")) != EOF){
//gotta open the file still
		index++;
		switch(c){
			case 'd':
				dflag = true;
				yydebug = 1;
				break;
			case 'p':
				pflag = true;
				break;
			case 's':
				sflag = true;
				break;
			case 'o':
				oflag = true;
				code = fopen(optarg, "w");
				break;
			case 'P':
				Pflag = true;
				break;
			case '?':
				if(optopt == 'o')
					printf("Option -%c requires an argument.\n", optopt);
			default:
				break;
		}
	}

	if(optind < argc){
                yyin = fopen(argv[optind], "r");
        }
/*
	if(!oflag){
		char *filename = argv[optind];
		if(strlen(filename) > 0){
			for(int i = 0; i < strlen(filename); i++ ){
				if(filename[i] == '.'){
					filename[i+1] = 't';
					filename[i+2] = 'm';
					break;
				}
			}
			code = fopen(filename, "w");
		}
		else{
			code = stdout;
		}
		optind++;
	}
*/

	initYyerror();
	yyparse();
	if(pflag)
		printTree(syntaxTree, false);
	else{
		PreProcess();

		syntaxTree = PPTree;
		if( !numErrors ){
			TypeCheck(syntaxTree, sflag);
			if(symtab->lookup((char *)"main") == NULL){
				printf("ERROR(LINKER): Procedure main is not defined.\n");
				numErrors++;
			}
			if(Pflag){
				printTree(syntaxTree, true);
			}
		}
		PrintErrors();

		if(numErrors == 0){
			if(!oflag){
	                        char *filename = argv[optind];
                        	if(strlen(filename) > 0){
                	                for(int i = 0; i < strlen(filename); i++ ){
        	                                if(filename[i] == '.'){
	                                                filename[i+1] = 't';
                                                	filename[i+2] = 'm';
                                        	        break;
                                	        }
                        	        }
                	                code = fopen(filename, "w");
        	                }
	                        else{
                        	        code = stdout;
                	        }
        	                optind++;
	                }


			codegen(syntaxTree);
		}
	}
	return 0;
}
