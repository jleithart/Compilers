#ifndef _UTIL_H_
#define _UTIL_H_

/* Function newStmtNode creates a new statement
 *  * node for syntax tree construction
 *   */
TreeNode * newStmtNode(StmtKind, int);

/* Function newExpNode creates a new expression
 *  * node for syntax tree construction
 *   */
TreeNode * newExpNode(ExpKind, int);

TreeNode * newDeclNode(DeclKind, int);

/* Function copyString allocates and makes a new
 *  * copy of an existing string
 *   */
char * copyString( char * );

/* procedure printTree prints a syntax tree to the
 *  * listing file using indentation to indicate subtrees
 *   */
void printTree(TreeNode *, bool);

void scopeAndType(TreeNode *);

void TreeTraverse(TreeNode *);
#endif
