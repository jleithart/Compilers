/*
 * Global Stuffs
 */

#ifndef _GLOBALS_H_

#define _GLOBALS_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//extern int lineno;
extern char *lastScannedToken;
extern int lineno;

//extern int goffset;
//extern int foffset;

typedef enum {
	declK,
	stmtK,
	expK
}NodeKind;

typedef enum {
	VarK,
	FuncK,
	ParamK
} DeclKind;

typedef enum{
	IfK,
	WhileK,
	ForEachK,
	CompoundK,
	ReturnK,
	BreakK
} StmtKind;

typedef enum{
	OpK,
	ConstantK,
	IdK,
	AssignK,
	CallK,
} ExpKind;

typedef enum{
	Integer,
	Char,
	Boolean,
	Void,
	UND
} ExpType;


typedef enum{
	Global,
	Local,
	GStatic,
	Param
} ScopeType;

typedef enum{
	LEQK, GEQK, NEQK, EQK, MODK, ASTERISKK, MINUSK, 
	PLUSK, SLASHK, LTHANK, GTHANK, NOTK, ANDK, ORK,
	DECK, INCK, MASSIGNK, PASSIGNK, EQUALK
}OpKind;

#define MAXCHILDREN 3

//this is copied from the textbook for compilers
// Compiler Construction: Principles and Practice
typedef struct treeNode
{
  // connectivity in the tree
  struct treeNode *child[MAXCHILDREN];   // children of the node
  struct treeNode *sibling;              // siblings for the node

  // what kind of node
  int lineno;                            // linenum relevant to this node
  NodeKind nodekind;                     // type of node
  union                                  // subtype of type
  {
    DeclKind decl;                     // used when DeclK
    StmtKind stmt;                     // used when StmtK
    ExpKind exp;                       // used when ExpK
  } kind;

  // extra properties about the node depending on type of the node
  union                                  // relevant data to type -> attr
  {
    OpKind op;                         // type of token (same as in bison)
    bool bVal;
    int val;                           // used when an integer constant
    char *name;                        // used when IdK
    char *string;                      // used when a string constant
  } attr;
  int iSize; //for arrays
  ExpType expType;                       // used when ExpK for type checking
  bool isStatic;                         // is staticly allocated?
  bool isArray;                          // is this an array
  bool isConstant;                       // can this expression be precomputed?
  bool isPrototype;
  int prototypeID;			// prototype id
  char *scopeName;
  int size;				// the size of the memory allocated
  int loc;

  ScopeType sType;

  // even more semantic stuff will go here in later assignments.
} TreeNode;

#endif
