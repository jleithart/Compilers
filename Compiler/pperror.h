#include "globals.h"

#ifndef PPERRORS_H
#define PPERRORS_H
int numErrors = 0;
int numWarnings = 0;

char *TYPE_ARRAY(ExpType type){
        char *retval;
        switch(type){
                case Integer:
                        retval = (char *)"type int";
                        break;
                case Char:
                        retval = (char *)"type char";
                        break;
                case Boolean:
                        retval = (char *)"type bool";
                        break;
                case Void:
                        retval = (char *)"type void";
                        break;
        }
        return retval;
}

char *OP_NAME(OpKind op){
	char *retval;
	switch(op){
		case LEQK:
			retval = (char *)"<=";
			break;
                case GEQK:
                        retval = (char *)">=";
                        break;
                case NEQK:
                        retval = (char *)"!=";
                        break;
                case EQK:
                        retval = (char *)"==";
                        break;
                case MODK:
                        retval = (char *)"\%";
                        break;
                case ASTERISKK:
                        retval = (char *)"*";
                        break;
                case MINUSK:
                        retval = (char *)"-";
                        break;
                case PLUSK:
                        retval = (char *)"+";
                        break;
                case SLASHK:
                        retval = (char *)"/";
                        break;
                case LTHANK:
                        retval = (char *)"<";
                        break;
                case GTHANK:
                        retval = (char *)">";
                        break;
                case NOTK:
                        retval = (char *)"not";
                        break;
                case ANDK:
                        retval = (char *)"and";
                        break;
                case ORK:
                        retval = (char *)"or";
                        break;
                case DECK:
                        retval = (char *)"--";
                        break;
                case INCK:
                        retval = (char *)"++";
                        break;
                case MASSIGNK:
                        retval = (char *)"-=";
                        break;
                case PASSIGNK:
                        retval = (char *)"+=";
                        break;
                case EQUALK:
                        retval = (char *)"=";
                        break;
	}
	return retval;
}

//DECLARATIONS
void DefAgainError(int lineno, char *name, int l){
	numErrors++;
	printf("ERROR(%d): Symbol '%s' is already defined at line %d.\n", lineno, name, l);
}
void NotDefError(int lineno, char *name){
        numErrors++;
	printf("ERROR(%d): Symbol '%s' is not defined.\n", lineno, name);
}

//EXPRESSIONS
void LHSOpOrError(int lineno, OpKind name, ExpType orType, ExpType type, ExpType typeGot){
        numErrors++;
        printf("ERROR(%d): '%s' requires operands of %s or %s but lhs is of %s.\n", lineno, OP_NAME(name), TYPE_ARRAY(orType), TYPE_ARRAY(type), TYPE_ARRAY(typeGot));
}
void RHSOpOrError(int lineno, OpKind name, ExpType orType, ExpType type, ExpType typeGot){
        numErrors++;
        printf("ERROR(%d): '%s' requires operands of %s or %s but rhs is of %s.\n", lineno, OP_NAME(name), TYPE_ARRAY(orType), TYPE_ARRAY(type), TYPE_ARRAY(typeGot));
}
void LHSOpError(int lineno, OpKind name, ExpType type, ExpType typeGot){
        numErrors++;
	printf("ERROR(%d): '%s' requires operands of %s but lhs is of %s.\n", lineno, OP_NAME(name), TYPE_ARRAY(type), TYPE_ARRAY(typeGot));
}
void RHSOpError(int lineno, OpKind name, ExpType type, ExpType typeGot){
        numErrors++;
	printf("ERROR(%d): '%s' requires operands of %s but rhs is of %s.\n", lineno, OP_NAME(name), TYPE_ARRAY(type), TYPE_ARRAY(typeGot));
}
void DiffTypeOpError(int lineno, OpKind name, ExpType lhs, ExpType rhs){
        numErrors++;
	printf("ERROR(%d): '%s' requires operands of the same type but lhs is %s and rhs is %s.\n", lineno, OP_NAME(name), TYPE_ARRAY(lhs), TYPE_ARRAY(rhs));
}
void OpArrayError(int lineno, OpKind name){
        numErrors++;
	printf("ERROR(%d): '%s' requires that if one operand is an array so must the other operand.\n", lineno, OP_NAME(name));
}
void OpNotArrayError(int lineno, OpKind name){
        numErrors++;
	printf("ERROR(%d): The operation '%s' does not work with arrays.\n", lineno, OP_NAME(name));
}
void OpOnlyArrayError(int lineno, OpKind name){
        numErrors++;
	printf("ERROR(%d): The operation '%s' only works with arrays.\n", lineno, OP_NAME(name));
}
void OpUnaryError(int lineno, OpKind name, ExpType op, ExpType opGot){
        numErrors++;
	printf("ERROR(%d): Unary '%s' requires an operand of %s but was given %s.\n", lineno, OP_NAME(name), TYPE_ARRAY(op), TYPE_ARRAY(opGot));
}

//TEST CONDITIONS
void ArrayConditionError(int lineno, char *name){
        numErrors++;
	printf("ERROR(%d): Cannot use array as test condition in %s statement.\n", lineno, name);
}
void ExpectBoolError(int lineno, char *name, ExpType got){
        numErrors++;
	printf("ERROR(%d): Expecting Boolean test condition in %s statement but got %s.\n", lineno, name, TYPE_ARRAY(got));
}

//RETURN
void ReturnArrayError(int lineno){
        numErrors++;
	printf("ERROR(%d): Cannot return an array.\n", lineno);
}
void ReturnVoidError(int lineno, char *name, int l){
        numErrors++;
	printf("ERROR(%d): Function '%s' at line %d is expecting no return value, but return has return value.\n", lineno, name, l);
}
void ReturnWrongTypeError(int lineno, char *name, int l, ExpType ret, ExpType retGot){
        numErrors++;
	printf("ERROR(%d): Function '%s' at line %d is expecting to return %s but got %s.\n", lineno, name, l, TYPE_ARRAY(ret), TYPE_ARRAY(retGot));
}
void NoReturnValueError(int lineno, char *name, int l, ExpType ret){
        numErrors++;
	printf("ERROR(%d): Function '%s' at line %d is expecting to return %s but return has no return value.\n", lineno, name, l, TYPE_ARRAY(ret));
}
void NoReturnWarning(int lineno, ExpType type, char *name){
        numWarnings++;
	printf("WARNING(%d): Expecting to return %s but function '%s' has no return statement.\n", lineno, TYPE_ARRAY(type), name);
}

//BREAK
void BreakError(int lineno){
        numErrors++;
	printf("ERROR(%d): Cannot have a break statement outside of loop.\n", lineno);
}

//FUNCTION INVOCATION
void NotFuncError(int lineno, char *name){
        numErrors++;
	printf("ERROR(%d): '%s' is a simple variable and cannot be called.\n", lineno, name);
}
void UseFuncWrongError(int lineno, char *name){
        numErrors++;
	printf("ERROR(%d): Cannot use function '%s' as a simple variable.\n", lineno, name);
}

//ARRAY INDEXING
void ArrayIndexError(int lineno, char *name, ExpType type){
        numErrors++;
	printf("ERROR(%d): Array '%s' should be indexed by type int but got %s.\n", lineno, name, TYPE_ARRAY(type));
}
void ArrayNoIndexError(int lineno, char *name){
        numErrors++;
	printf("ERROR(%d): Array index is the unindexed array '%s'.\n", lineno, name);
}
void NotArrayIndexError(int lineno, char *name){
        numErrors++;
	printf("ERROR(%d): Cannot index nonarray '%s'.\n", lineno, name);
}

//PARAMETER LIST
void WrongParamError(int lineno, ExpType type, int par, char *name, int l, ExpType typeGot){
        numErrors++;
	printf("ERROR(%d): Expecting %s in parameter %i of call to '%s' defined on line %d but got %s.\n", lineno, TYPE_ARRAY(type), par, name, l, TYPE_ARRAY(typeGot));
}
void NotArrayParamError(int lineno, int par, char *name, int l){
        numErrors++;
	printf("ERROR(%d): Expecting array in parameter %i of call to '%s' defined on line %d.\n", lineno, par, name, l);
}
void GotArrayParamError(int lineno, int par, char *name, int l){
        numErrors++;
	printf("ERROR(%d): Not expecting array in parameter %i of call to '%s' defined on line %d.\n", lineno, par, name, l);
}
void NotEnoughParamError(int lineno, char *name, int l){
        numErrors++;
	printf("ERROR(%d): Too few parameters passed for function '%s' defined on line %d.\n", lineno, name, l);
}
void TooManyParamError(int lineno, char *name, int l){
        numErrors++;
	printf("ERROR(%d): Too many parameters passed for function '%s' defined on line %d.\n", lineno, name, l);
}

//FOREACH
void WrongBothForeachError(int lineno, ExpType lhs, ExpType rhs){
        numErrors++;
	printf("ERROR(%d): Foreach requires operands of 'in' be the same type but lhs is %s and rhs array is %s.\n", lineno, TYPE_ARRAY(lhs), TYPE_ARRAY(rhs));
}
void WrongLHSForeachError(int lineno, ExpType lhs){
        numErrors++;
	printf("ERROR(%d): If not an array, foreach requires lhs of 'in' be of type int but it is %s.\n", lineno, TYPE_ARRAY(lhs));
}
void WrongRHSForeachError(int lineno, ExpType rhs){
        numErrors++;
	printf("ERROR(%d): If not an array, foreach requires rhs of 'in' be of type int but it is %s.\n", lineno, TYPE_ARRAY(rhs));
}
void ArrayForeachError(int lineno){
        numErrors++;
	printf("ERROR(%d): In foreach statement the variable to the left of 'in' must not be an array.\n", lineno);
}

//INITIALIZERS
void NoArrayInitialError(int lineno, char *name, ExpType type){
        numErrors++;
	printf("ERROR(%d): Initializer for array variable '%s' must be a string, but is of nonarray %s.\n", lineno, name, TYPE_ARRAY(type));
}
void ArrayInitialError(int lineno, char *name, ExpType type){
        numErrors++;
	printf("ERROR(%d): Initializer for nonarray variable '%s' of %s cannot be initialized with an array.\n", lineno, name, TYPE_ARRAY(type));
}
void NotConstantInitialError(int lineno, char *name){
        numErrors++;
	printf("ERROR(%d): Initializer for variable '%s' is not a constant expression.\n", lineno, name);
}
void WrongTypeInitialError(int lineno, char *name, ExpType type, ExpType gotType){
        numErrors++;
	printf("ERROR(%d): Variable '%s' is of %s but is being initialized with an expression of %s.\n", lineno, name, TYPE_ARRAY(type), TYPE_ARRAY(gotType));
}
void NotCharArrayInitialError(int lineno, char *name, ExpType type){
	numErrors++;
	printf("ERROR(%d): Array '%s' must be of type char to be initialized, but is of %s.\n", lineno, name, TYPE_ARRAY(type));
}

void PrintNumWarning(){
	printf("Number of warnings: %d\n", numWarnings);
}

void PrintNumError(){
	printf("Number of errors: %d\n", numErrors);
}

#endif
