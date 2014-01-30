#include "analyze.h"
#include "symtab.h"
#include "pperror.h"
/* got a lot of my code for this file from the textbook: */
/* Compiler Construction; Page 524 */

int goffset = 0;
int foffset = -2;

extern SymTab *symtab;
static bool returnflag = false;
static bool infunc;
static int loop = 0;
static int paramFlag = 0;
static bool firstcompound = false;
static char *funcname;
static char *scopename;
static char *tmp;

int funcline;
TreeNode * tmpNode;

int NUM_GLOBALS = 7;

void PrintErrors(){
	PrintNumWarning();
	PrintNumError();
}

void TypeCheck(TreeNode *t, bool flag){
	scopename = (char *)"globals";
	TreeTraverse(t);
	if(flag)
		symtab->print();
}

void TreeTraverse(TreeNode *t){
  TreeNode *prev = NULL;
  while(t != NULL){
	switch (t->nodekind){
		case declK:
			switch(t->kind.decl){
				case VarK:
					TreeTraverse(t->child[0]);
					prev = (TreeNode *)symtab->lookup(t->attr.name);
					if( prev == NULL){
						if(!t->isStatic){
							if(!infunc){
								t->sType = Global;
	                                                        t->loc = goffset--;
								NUM_GLOBALS++;
							}
							else{
								t->loc = foffset--;
                                                                t->sType = Local;
							}
						}
						else if(t->isStatic){
							TreeNode * tmp = t;
							t->loc = goffset--;
                                                        //foffset--;
                                                        t->sType = GStatic;
                                                        symtab->insertGlobal(t->attr.name, tmp);
						}
						if(t->isArray){
							t->size = t->iSize + 1;
							if(t->sType != Local){
								NUM_GLOBALS++;
								goffset -= t->iSize;
							}
							t->loc -= 1;
							foffset = foffset - t->size + 1;
						}
						else{
							t->size = 1;
						}
					}
					else if(infunc){
						if(t->isArray){
							t->size = t->iSize + 1;
							foffset = foffset - t->size + 1;
							t->loc -= 1;
						}
						else{
							t->loc = foffset--;
							t->size = 1;
						}
						//printf("%s\n", t->attr.name);
						if(t->isStatic){
							TreeNode *tmp = t;
							t->loc = goffset--;
							//foffset--;
							t->sType = GStatic;
							symtab->insertGlobal(t->attr.name, tmp);
						}
						else{
							t->sType = Local;

						}
					}
					if(t->child[0] != NULL){
						if(!t->isArray){
							if(!t->child[0]->isConstant){
                                        	                NotConstantInitialError(t->lineno, t->attr.name);
                                                        }
							if(t->expType != t->child[0]->expType && t->child[0]->expType != UND){
								WrongTypeInitialError(t->lineno, t->attr.name, t->expType, t->child[0]->expType);
							}
							if(!t->isArray && t->child[0]->isArray){
                                                               	ArrayInitialError(t->lineno, t->attr.name, t->expType);
                                                        }
						}
						else{
							if(!t->child[0]->isConstant ){
								NotConstantInitialError(t->lineno, t->attr.name);
							}
							if(t->isArray && t->expType != Char){
								NotCharArrayInitialError(t->lineno, t->attr.name, t->expType);
							}
							if(t->isArray && (t->child[0]->expType != Char || !t->child[0]->isArray) ){
								NoArrayInitialError(t->lineno, t->attr.name, t->child[0]->expType);
							}
						}
					}
					if(!symtab->insert(t->attr.name, t)){
                                                DefAgainError(t->lineno, t->attr.name, prev->lineno);
                                        }
					break;
				case FuncK:
					funcline = t->lineno;
					prev = (TreeNode *)symtab->lookup(t->attr.name);
					tmpNode = t;
					if(prev != NULL){
						DefAgainError(t->lineno, t->attr.name, prev->lineno);
					}
					else{
						t->sType = Global;
						t->loc = 0;
					}
					/*symtab->leave();*/
					symtab->insert(t->attr.name, t);
					funcname = t->attr.name;
					scopename = funcname;
					infunc = true;
					symtab->enter(t->attr.name);
					foffset = -2;
					TreeTraverse(t->child[0]);
					if(t->child[1] != NULL){
						if(t->child[1]->kind.stmt != CompoundK){
							TreeTraverse(t->child[1]);
						}
						else{
							TreeTraverse(t->child[1]->child[0]);
							firstcompound = true;
							TreeTraverse(t->child[1]->child[1]);
						}
					}
					if(!returnflag && t->expType != Void && !t->isPrototype)
						NoReturnWarning(t->lineno, t->expType, t->attr.name);
					symtab->leave();
					infunc = false;
					t->size = -foffset;
					/*foffset = -2;*/
					scopename = (char *)"globals";
					returnflag = false;
					
					break;
				case ParamK:
					TreeTraverse(t->child[0]);
					TreeTraverse(t->child[1]);
					TreeTraverse(t->child[2]);
					if(t->attr.name != NULL)
						prev = (TreeNode *)symtab->lookup(t->attr.name);
					if(!t->isPrototype){
						if(!symtab->insert(t->attr.name, t))
							DefAgainError(t->lineno, t->attr.name, prev->lineno);
					}
					t->loc = foffset--;
					t->size = 1;
					t->sType = Param;
					break;
			}
			break;
		case stmtK:
			switch(t->kind.stmt){
				case IfK:
					TreeTraverse(t->child[0]);
					if(t->child[0]->expType != Boolean && t->child[0]->expType != UND){
                                                ExpectBoolError(t->lineno, (char *)"if", t->child[0]->expType);
                                        }
					if(t->child[0]->isArray && t->child[0]->child[0] == NULL)
						ArrayConditionError(t->lineno, (char *)"if");
					TreeTraverse(t->child[1]);
                                        TreeTraverse(t->child[2]);
					break;
				case WhileK:
					loop++;
					TreeTraverse(t->child[0]);
					if(t->child[0]->expType != Boolean && t->child[0]->expType != UND)
                                                ExpectBoolError(t->lineno, (char *)"while", t->child[0]->expType);
					if(t->child[0]->isArray && t->child[0]->child[0] == NULL)
						ArrayConditionError(t->lineno, (char *)"while");
					TreeTraverse(t->child[1]);
                                        TreeTraverse(t->child[2]);
					loop--;

					break;
				case ForEachK:
					loop++;
                                        TreeTraverse(t->child[0]);
                                        TreeTraverse(t->child[1]);
                                        TreeTraverse(t->child[2]);
					if(t->child[0]->isArray){
                                                ArrayForeachError(t->lineno);
                                        }
					if(!t->child[1]->isArray){
						if(t->child[0]->expType != Integer && t->child[0]->expType != UND){
                                                	WrongLHSForeachError(t->lineno, t->child[0]->expType);
                                        	}
                                        	if(t->child[1]->expType != Integer && t->child[1]->expType != UND){
                                                	WrongRHSForeachError(t->lineno, t->child[1]->expType);
                                        	}
					}
					else if(t->child[0]->expType != t->child[1]->expType && t->child[0]->expType != UND && t->child[1]->expType != UND){
                                                WrongBothForeachError(t->lineno, t->child[0]->expType, t->child[1]->expType);
                                        }
					loop--;
					break;
				case CompoundK:
					tmp = scopename;
					if(firstcompound){
						symtab->enter( (char *)"compound");
						scopename = (char *)"compound";
						TreeTraverse(t->child[0]);
						TreeTraverse(t->child[1]);
						TreeTraverse(t->child[2]);
						symtab->leave();
					}
					scopename = tmp;
					break;
				case ReturnK:
					returnflag = true;
					prev = (TreeNode *)symtab->lookup(funcname);
					TreeTraverse(t->child[0]);
					TreeTraverse(t->child[1]);
					/* is it an array? */
					if(t->child[0] != NULL){
						/* if the parent function is void */
						if(t->child[0] != NULL && tmpNode->expType == Void && t->child[0]->expType != UND){
							ReturnVoidError(t->lineno, prev->attr.name, funcline);
						}
						/* wrong type */
						else if(t->child[0]->expType != tmpNode->expType && t->child[0]->expType != UND){
							ReturnWrongTypeError(t->lineno, prev->attr.name, funcline, tmpNode->expType, t->child[0]->expType);
						}
						if(t->child[0]->isArray && t->child[0]->child[0] == NULL){
                                                        ReturnArrayError(t->lineno);
                                                }
					}
					/* not returning when should */
					if(t->child[0] == NULL && tmpNode->expType != Void){
						NoReturnValueError(t->lineno, prev->attr.name, funcline, tmpNode->expType);
					}
					break;
				case BreakK:
					if(loop == 0){
						BreakError(t->lineno);
					}
					break;
			}
			break;
		case expK:
			switch(t->kind.exp){
				case OpK:
					switch(t->attr.op){
						case LEQK:
						case GEQK:
						case LTHANK:
						case GTHANK:
							TreeTraverse(t->child[0]);
							TreeTraverse(t->child[1]);
							if((t->child[0]->expType == Boolean || t->child[0]->expType == Void) && t->child[0]->expType != UND){
								LHSOpOrError(t->lineno,
									t->attr.op,
									Char,
									Integer,
									t->child[0]->expType);
							}
                                                        if((t->child[1]->expType == Boolean || t->child[1]->expType == Void) && t->child[0]->expType != UND){
                                                                RHSOpOrError(t->lineno,
                                                                        t->attr.op,
									Char,
                                                                        Integer,
                                                                        t->child[1]->expType);
                                                        }
							if( (t->child[0]->isArray && t->child[0]->child[0] == NULL ) &&
							    (t->child[1]->child[0] == NULL || t->child[1]->isArray)){
								OpNotArrayError(t->lineno, t->attr.op);
							}
							t->expType = Boolean;
							break;
                                                case NEQK:
						case EQK:
							TreeTraverse(t->child[0]);
							TreeTraverse(t->child[1]);
							if(t->child[0]->expType != t->child[1]->expType){
								DiffTypeOpError(t->lineno,
									t->attr.op,
									t->child[0]->expType,
									t->child[1]->expType);
							}
							if( ((t->child[0]->isArray && t->child[0]->child[0] == NULL && !t->child[1]->isArray) ||
							    (!t->child[0]->isArray && t->child[1]->child[0] == NULL && t->child[1]->isArray)) &&
							    ( t->child[0]->expType != UND && t->child[1]->expType != UND) ){
								OpArrayError(t->lineno, t->attr.op);
							}
                                                        t->expType = Boolean;
                                                        break;
                                                case MODK:
						case PLUSK:
						case SLASHK:
							TreeTraverse(t->child[0]);
							TreeTraverse(t->child[1]);
							/*only integers */
							if(t->child[0]->expType != Integer && t->child[0]->expType != UND){
								LHSOpError(t->lineno,
									t->attr.op,
									Integer,
									t->child[0]->expType);
							}
                                                        if(t->child[1]->expType != Integer && t->child[1]->expType != UND){
                                                                RHSOpError(t->lineno,
                                                                        t->attr.op,
                                                                        Integer,
                                                                        t->child[1]->expType);
                                                        }
                                                        if( (t->child[0]->isArray && t->child[0]->child[0] == NULL) || 
							    (t->child[1]->isArray && t->child[1]->child[0] == NULL)){
                                                                OpNotArrayError(t->lineno, t->attr.op);
                                                        }
							t->expType = Integer;
                                                        break;
                                                case ASTERISKK:
						case MINUSK:
							TreeTraverse(t->child[0]);
							TreeTraverse(t->child[1]);
							/*is it unary?*/
							if(t->child[1] != NULL){
	                                                        if((t->child[0]->expType == Boolean || t->child[0]->expType == Char) && t->child[0]->expType != UND){
        	                                                        LHSOpError(t->lineno,
                	                                                        t->attr.op,
                        	                                                Integer,
                                	                                        t->child[0]->expType);
                                        	                }
                                                	        if((t->child[1]->expType == Boolean || t->child[1]->expType == Char)  && t->child[0]->expType != UND){
                                                        	        RHSOpError(t->lineno,
                                                                	        t->attr.op,
                                                                        	Integer,
	                                                                        t->child[1]->expType);
        	                                                }
                	                                        if( (t->child[0]->isArray && t->child[0]->child[0] == NULL ) || 
								    (t->child[1]->isArray && t->child[1]->child[0] == NULL )){
                        	                                        OpNotArrayError(t->lineno, t->attr.op);
                                	                        }
                                        	                t->expType = Integer;
							}
							else{
								if(t->attr.op == ASTERISKK){
									if(!t->child[0]->isArray){
										OpOnlyArrayError(t->lineno, t->attr.op);
									}
									t->expType = Integer;
								}
								else{
									/* otherwise it is unary */
									if(t->child[0]->expType != Integer && t->child[0]->expType != UND){
										OpUnaryError(t->lineno,
											t->attr.op,
											Integer,
											t->child[0]->expType);
									}
									if(t->child[0]->isArray && t->child[0]->child[0] == NULL){
                                                                        	OpNotArrayError(t->lineno, t->attr.op);
                                                                	}
									t->expType = Integer;
								}
							}
                                                        break;
                                                case NOTK:
							TreeTraverse(t->child[0]);
							if(t->child[0]->expType != Boolean && t->child[0]->expType != UND){
								OpUnaryError(t->lineno,
									t->attr.op,
									Boolean,
									t->child[0]->expType);
							}
							if(t->child[0]->isArray && t->child[0]->child[0] == NULL){
                                                        	OpNotArrayError(t->lineno, t->attr.op);
                                                        }
							t->expType = Boolean;
                                                        break;
                                                case ANDK:
						case ORK:
							TreeTraverse(t->child[0]);
							TreeTraverse(t->child[1]);
							if((t->child[0]->expType == Integer || t->child[0]->expType == Char) && t->child[0]->expType != UND){
                                                                LHSOpError(t->lineno,
                                                                        t->attr.op,
                                                                        Boolean,
                                                                        t->child[0]->expType);
                                                        }
                                                        if((t->child[1]->expType == Integer || t->child[1]->expType == Char) && t->child[1]->expType != UND){
                                                                RHSOpError(t->lineno,
                                                                        t->attr.op,
                                                                        Boolean,
                                                                        t->child[1]->expType);
                                                        }
                                                        if( (t->child[0]->isArray && t->child[0]->child[0] == NULL) || 
							    (t->child[1]->isArray && t->child[1]->child[0] == NULL) ){
                                                                OpNotArrayError(t->lineno, t->attr.op);
                                                        }
                                                        t->expType = Boolean;
                                                        break;
					}
					break;
				case ConstantK:
					break;
				case IdK:
					prev = (TreeNode *)symtab->lookup(t->attr.name);
                                        if(prev == NULL){
                                                NotDefError(t->lineno, t->attr.name);
                                                t->expType = UND;
                                        }
					else if(prev->kind.decl == FuncK){
						UseFuncWrongError(t->lineno, prev->attr.name);
					}
					else if(t->isArray && !prev->isArray){
						NotArrayIndexError(t->lineno, t->attr.name);	
					}
					else{
						TreeTraverse(t->child[0]);
						if(t->isArray && t->child[0]->expType != Integer && t->child[0]->expType != UND){
                                                	ArrayIndexError(t->lineno, t->attr.name, t->child[0]->expType);
                                        	}
						if(t->isArray && t->child[0] == NULL){
							ArrayNoIndexError(t->lineno, t->attr.name);
						}
						if(t->isArray && t->child[0]->isArray && t->child[0]->child[0] == NULL){
							ArrayNoIndexError(t->child[0]->lineno, t->child[0]->attr.name);
						}
					}
					if(prev != NULL){
						t->sType = prev->sType;
						t->expType = prev->expType;

/*
						if(prev->isArray){
							if(t->child[0] == NULL)
								t->isArray = true;
							else
								t->isArray = false;
						}
						else
*/
						t->isArray = prev->isArray;
						t->loc = prev->loc;
						t->size = prev->size;
					}
					break;
				case AssignK:
					switch(t->attr.op){
						case PASSIGNK:
						case MASSIGNK:
							TreeTraverse(t->child[0]);
							TreeTraverse(t->child[1]);
							if(t->child[0]->expType != Integer && t->child[0]->expType != UND){
                                                                LHSOpError(t->lineno,
                                                                        t->attr.op,
                                                                        Integer,
                                                                        t->child[0]->expType);
                                                        }
                                                        else if(t->child[1]->expType != Integer && t->child[0]->expType != UND){
                                                                RHSOpError(t->lineno,
                                                                        t->attr.op,
                                                                        Integer,
                                                                        t->child[1]->expType);
                                                        }
							t->expType = t->child[0]->expType;
							break;
						case EQUALK:
							TreeTraverse(t->child[0]);
							TreeTraverse(t->child[1]);
							if(t->child[0]->expType != t->child[1]->expType && t->child[0]->expType != UND && t->child[1]->expType != UND){
                                                                DiffTypeOpError(t->lineno,
                                                                        t->attr.op,
                                                                        t->child[0]->expType,
                                                                        t->child[1]->expType);
                                                        }
							if( ((t->child[0]->isArray && !t->child[1]->isArray && t->child[0]->child[0] == NULL) ||
							    (!t->child[0]->isArray && t->child[1]->isArray && t->child[1]->child[0] == NULL)) &&
							    (t->child[0]->expType != UND && t->child[1]->expType != UND) ){
								OpArrayError(t->lineno, t->attr.op);
							}
                                                        t->expType = t->child[0]->expType;
							break;
						case INCK:
						case DECK:
							TreeTraverse(t->child[0]);
                                                        if(t->child[0]->expType != Integer && t->child[0]->expType != UND){
                                                                OpUnaryError(t->lineno,
                                                                        t->attr.op,
                                                                        Integer,
                                                                        t->child[0]->expType);
                                                        }
							if(t->child[0]->isArray && t->child[0]->child[0] == NULL){
                                                                OpNotArrayError(t->lineno, t->attr.op);
                                                        }
                                                        t->expType = Integer;
                                                        break;
					}
					break;
				case CallK:
					int i = 1;
					prev = (TreeNode *)symtab->lookup(t->attr.name);
					TreeNode *tmp = t->child[0];
					/* check to see if it's predefined or not */
					if(prev == NULL){
						t->expType = UND;
						NotDefError(t->lineno, t->attr.name);
						paramFlag++;
						while(tmp != NULL){
							TreeTraverse(tmp);
							tmp = tmp->sibling;
						}
						paramFlag--;
					}
					else if(prev != NULL){
						t->expType = prev->expType;				
						if(prev->kind.decl != FuncK){
							NotFuncError(t->lineno, t->attr.name);
							paramFlag++;
	                                                while(tmp != NULL){
        	                                                TreeTraverse(tmp);
                	                                        tmp = tmp->sibling;
                        	                        }
                                	                paramFlag--;
						}
						else{
						/*param list*/
						TreeNode *tmp2 = prev->child[0];
						while(tmp != NULL && tmp2 != NULL){
							//tmp->sType = tmp2->sType;
							//tmp->loc = tmp2->loc;
							//tmp->size = tmp2->size;
							if(tmp == NULL || tmp2 == NULL){
								break;
							}
							paramFlag++;
							TreeTraverse(tmp);
							//tmp->sType = tmp2->sType;
							//tmp->loc = tmp2->sType;
							//tmp->size = tmp2->size;
							paramFlag--;
                                                        if(tmp2->expType != tmp->expType && tmp->expType != UND){
                                                                WrongParamError(t->lineno, tmp2->expType, i, prev->attr.name, prev->lineno, tmp->expType);
                                                        }
							if(!tmp->isArray && tmp2->isArray ){
                                                                NotArrayParamError(t->lineno, i, t->attr.name, prev->lineno);
                                                        }
                                                        else if(tmp->isArray && tmp->child[0] == NULL && !tmp2->isArray){
                                                                GotArrayParamError(t->lineno, i, t->attr.name, prev->lineno);
                                                        }
							tmp = tmp->sibling;
							tmp2 = tmp2->sibling;
							i++;
						}
							if(tmp == NULL && tmp2 != NULL){
                                                                NotEnoughParamError(t->lineno, prev->attr.name, prev->lineno);
                                                                break;
                                                        }
                                                        if(tmp != NULL && tmp2 == NULL){
                                                                TooManyParamError(t->lineno, prev->attr.name, prev->lineno);
								paramFlag++;
                                                                while(tmp != NULL){
									TreeTraverse(tmp);
									tmp = tmp->sibling;
                                                                }
                                                                paramFlag--;
                                                                break;
                                                        }
						}
					}
					break;
			}
			break;
	}
	if(paramFlag == 0)
		t = t->sibling;
	else
		t = NULL;
  }
}

