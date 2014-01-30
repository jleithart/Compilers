/*
* Name:			Jordan Leithart
* Class:		CS 445
* Assignment:		Assignment #1
t
*/

%{
#include "globals.h"
#include "yyerror.h"
#include "util.h"
#include "scanType.h"
#include "scanner.tab.h"

extern int lineno; // static means it's local
extern char *lastScannedToken;

char *checkString(char *s, int type){
	char *t = copyString(s);
	int n = strlen(t) + 1;
	for(int i = 0; i < n; i++){
		if(t[i] == '\\'){
			int j = i;
			while(j < n){
				t[j] = t[j+1];
				j++;
			}
			t[j] = '\0';
			switch(t[i]){
				case '0':
					t[i+1] = '\0';
					// 0 == char
					if(type == 0){
						t[i] = '\'';
					}
					else
						t[i] = '\"';
					break;
				case 't':
					t[i] = '\t';
					break;
				case 'n':
					t[i] = '\n';
					break;
			}	
		}
		else if(t[i] == '^'){
			int j = i;
			while(j < n){
				t[j] = t[j+1];
				j++;
			}
			t[j] = '\0';
			t[i] = t[i] ^ 0x40;
		}
	}
	return t;
}

int setValue(int linenum, int tokenClass, char *svalue){
	yylval.tokenData = new TokenData;
	yylval.tokenData->tokenclass = tokenClass;
	yylval.tokenData->linenumber = linenum;
	lastScannedToken = svalue;

	switch(tokenClass){
		case NUMCONST:
			yylval.tokenData->numvalue = atoi(svalue);
			break;
		case ID:
			yylval.tokenData->tokenstr = copyString(svalue);
			break;
		case CHARCONST:
			yylval.tokenData->charvalue = copyString(checkString(svalue, 0));
			break;
		case STRINGCONST:
			yylval.tokenData->strvalue = copyString(checkString(svalue, 1));
			break;
	}

	return tokenClass;
}

%}

letter [a-zA-Z]
digit [0-9]
underbar "_"
letdigitunder {letter}|{digit}|{underbar}

%%

[ \t]                           ;
"//".*                      	;
\n				{ lineno++; }
"int"				{ return setValue(lineno, INT, yytext); }
"bool"				{ return setValue(lineno, BOOLEAN, yytext); }
"char" 				{ return setValue(lineno, CHAR, yytext); }
"if"				{ return setValue(lineno, IF, yytext); }
";"				{ return setValue(lineno, SEMICOLON, yytext); }
","				{ return setValue(lineno, COMMA, yytext); }
"("				{ return setValue(lineno, LPAREN, yytext); }
")"				{ return setValue(lineno, RPAREN, yytext); }
"{"				{ return setValue(lineno, LBRACE, yytext); }
"}"				{ return setValue(lineno, RBRACE, yytext); }
"["				{ return setValue(lineno, LBRACK, yytext); }
"]"				{ return setValue(lineno, RBRACK, yytext); }
":"				{ return setValue(lineno, COLON, yytext); }
"="				{ return setValue(lineno, EQUAL, yytext); }
"%"				{ return setValue(lineno, MOD, yytext); }
"*"				{ return setValue(lineno, ASTERISK, yytext); }
"-"				{ return setValue(lineno, MINUS, yytext); }
"+"				{ return setValue(lineno, PLUS, yytext); }
"true"				{ return setValue(lineno, TRUE, yytext); }
"false"				{ return setValue(lineno, FALSE, yytext); }
"+="				{ return setValue(lineno, PASSIGN, yytext); }
"-="				{ return setValue(lineno, MASSIGN, yytext); }
"/"				{ return setValue(lineno, SLASH, yytext); }
"<"				{ return setValue(lineno, LTHAN, yytext); }
"and"				{ return setValue(lineno, AND, yytext); }
"<="				{ return setValue(lineno, LEQ, yytext); }
">"				{ return setValue(lineno, GTHAN, yytext); }
">="				{ return setValue(lineno, GEQ, yytext); }
"or"				{ return setValue(lineno, OR, yytext); }
"!="				{ return setValue(lineno, NEQ, yytext); }
"not"				{ return setValue(lineno, NOT, yytext); }
"=="				{ return setValue(lineno, EQ, yytext); }
"else"				{ return setValue(lineno, ELSE, yytext); }
"while"				{ return setValue(lineno, WHILE, yytext); }
"static"			{ return setValue(lineno, STATIC, yytext); }
"break"				{ return setValue(lineno, BREAK, yytext); }
"--"				{ return setValue(lineno, DEC, yytext); }
"++"				{ return setValue(lineno, INC, yytext); }
"foreach"			{ return setValue(lineno, FOREACH, yytext); }
"in"				{ return setValue(lineno, IN, yytext); }
"return"			{ return setValue(lineno, RETURN, yytext); }
{letter}{letdigitunder}*        { return setValue(lineno, ID, yytext); }
{digit}+                        { return setValue(lineno, NUMCONST, yytext); }
'([^'\\\n]|^.|\\.)'		{ return setValue(lineno, CHARCONST, yytext); }
\"(\\.|\\|[^\\"])*\"		{ return setValue(lineno, STRINGCONST, yytext); }
.				{ lastScannedToken = yytext; yyerror( (char *) "Invalid input character"); }


%%

