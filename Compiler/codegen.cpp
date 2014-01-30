#include "codegen.h"
#include "symtab.h"
/* got a lot of my code for this file from the textbook: */
/* Compiler Construction; Page 524 */

int currloc = 0;
int skiploc = 0;
int breakloc = 0;
int breakloc2 = 0;

int toff = 0;
int offReg = 1;

bool in_expression = false;

/* POSSIBLE ENUM */
int RETURN_OFFSET = 0;
int OFPOFF = -1;

int paramNum = 0;
int paramOffset = -2;	// one for the return register and one for the PC
int callOffset = 2; 
int frameSize = 0;

bool globals_go = false;

extern SymTab *symtab;
extern int goffset;
extern int NUM_GLOBALS;

void codegen(TreeNode *t){

	emitComment((char *)"C- compiler version C-F13");
	emitComment((char *)"Built: Dec 9, 2013");
	emitComment((char *)"Author: Jordan Leithart");
	emitComment((char *)"File compiled: blank");

	GenerateCode(t);
	PostCodeGen();	

}

void GenerateCode(TreeNode *t){
  TreeNode *prev = NULL;
  int currloc = 0;
  int skiploc = 0;
  int tmpParam = 0;
  bool tmp_expression;
  while(t != NULL){
	switch (t->nodekind){
		case declK:
			switch(t->kind.decl){
				case VarK:
					prev = (TreeNode *)symtab->lookup(t->attr.name);
					if(prev == NULL && !globals_go){
							if(t->sType != GStatic){
								GenerateCode(t->child[0]);
								if(t->isArray){
									emitRM2((char *)"LDC", AC, t->size - 1, AC3, (char *)"Load size of array", t->attr.name);
									emitRM2((char *)"ST", AC, t->loc + 1, FP, (char *)"Save size of array", t->attr.name);
								}
								else if(t->child[0] != NULL){
									emitRM2((char *)"ST", AC, t->loc, FP, (char *)"Store variable", t->attr.name);
								}
							}
					}
					else if(globals_go && t->child[0] != NULL){
						GenerateCode(t->child[0]);
						emitRM2((char *)"ST", AC, t->loc, 0, (char *)"Store variable", t->attr.name);
					}
					break;
				case FuncK:
					prev = (TreeNode *)symtab->lookup(t->attr.name);
					prev->loc = emitSkip(0);
					toff = 0;
					frameSize = prev->size;
					emitComment2((char *)"BEGIN function", t->attr.name);
					emitRM2((char *)"ST", AC, -1, FP, (char *)"Store return address.", (char *)"");
					GenerateCode(t->child[0]);
					GenerateCode(t->child[1]);
					if(!t->isPrototype){
						emitComment((char *)"Add standard closing in case there is no return statement");
						emitRM2((char *)"LDC", RT, 0, AC3, (char *)"Set return value to 0", (char *)"");
						emitRM2((char *)"LD", AC, -1, FP, (char *)"Load return address", (char *)"");
						emitRM2((char *)"LD", FP, 0, FP, (char *)"Adjust fp", (char *)"");
					}
					else{
						switch(t->prototypeID){
							case 0:
								emitRO((char *)"IN", RT, RT, RT, (char *)"Grab int input");
								emitRM((char *)"LD", AC, OFPOFF, FP, (char *)"Load return address");
								emitRM((char *)"LD", FP, RETURN_OFFSET, FP, (char *)"Adjust fp");
								break;
							case 1:
								emitRM((char *)"LD", AC, -2, FP, (char *)"Load parameter");
								emitRO((char *)"OUT", AC, AC, AC, (char *)"Output integer");
								emitRM((char *)"LDC", RT, RETURN_OFFSET, AC3, (char *)"Set return to 0");
								emitRM((char *)"LD", AC, OFPOFF, FP, (char *)"Load return address");
								emitRM((char *)"LD", FP, RETURN_OFFSET, FP, (char *)"Adjust fp");
								break;
							case 2:
								emitRO((char *)"INB", RT, RT, RT, (char *)"Grab bool input");
                                                                emitRM((char *)"LD", AC, OFPOFF, FP, (char *)"Load return address");
                                                                emitRM((char *)"LD", FP, RETURN_OFFSET, FP, (char *)"Adjust fp");
								break;
							case 3:
								emitRM((char *)"LD", AC, -2, FP, (char *)"Load parameter");
                                                                emitRO((char *)"OUTB", AC, AC, AC, (char *)"Output bool");
                                                                emitRM((char *)"LDC", RT, RETURN_OFFSET, AC3, (char *)"Set return to 0");
                                                                emitRM((char *)"LD", AC, OFPOFF, FP, (char *)"Load return address");
                                                                emitRM((char *)"LD", FP, RETURN_OFFSET, FP, (char *)"Adjust fp");
								break;
							case 4:
								emitRO((char *)"INC", RT, RT, RT, (char *)"Grab char input");
                                                                emitRM((char *)"LD", AC, OFPOFF, FP, (char *)"Load return address");
                                                                emitRM((char *)"LD", FP, RETURN_OFFSET, FP, (char *)"Adjust fp");
								break;
							case 5:
								emitRM((char *)"LD", AC, -2, FP, (char *)"Load parameter");
                                                                emitRO((char *)"OUTC", AC, AC, AC, (char *)"Output char");
                                                                emitRM((char *)"LDC", RT, RETURN_OFFSET, AC3, (char *)"Set return to 0");
                                                                emitRM((char *)"LD", AC, OFPOFF, FP, (char *)"Load return address");
                                                                emitRM((char *)"LD", FP, RETURN_OFFSET, FP, (char *)"Adjust fp");
								break;
							case 6:
                                                                emitRO((char *)"OUTNL", AC, AC, AC, (char *)"Output a newline");
                                                                emitRM((char *)"LD", AC, OFPOFF, FP, (char *)"Load return address");
                                                                emitRM((char *)"LD", FP, RETURN_OFFSET, FP, (char *)"Adjust fp");
								break;
						}
					}
					emitRM2((char *)"LDA", PC, 0, AC, (char *)"Return", (char *)"");
					emitComment2((char *)"END of function", t->attr.name);
					toff = 0;
					break;
				case ParamK:
					//emitRM2((char *)"LD", AC, t->loc, FP, (char *)"Load parameter", (char *)"");
					break;
			}
			break;
		case stmtK:
			switch(t->kind.stmt){
				case IfK:
					int breakl;
					emitComment((char *)"IF");
					currloc = emitSkip(0);
					GenerateCode(t->child[0]);
					emitRM((char *)"JGT", AC, FP, PC, (char *)"Jump to then part");
					emitComment((char *)"THEN");
					skiploc = breakloc;
					breakloc = emitSkip(1);
					GenerateCode(t->child[1]);
					if(t->child[2] != NULL){
						emitComment((char *)"ELSE");
						breakl = emitSkip(1);
						backPatchJumpToHere(breakloc, (char *)"Jump around the THEN [backpatch]");
						GenerateCode(t->child[2]);
						backPatchJumpToHere(breakl, (char *)"Jump around the ELSE [backpatch]");
					}
					else
						backPatchJumpToHere(breakloc, (char *)"Jump around the THEN [backpatch]");
					breakloc = skiploc;
					emitComment((char *)"ENDIF");
					break;
				case WhileK:
					int skiploc;
					emitComment((char *)"WHILE");
					currloc = emitSkip(0);
					GenerateCode(t->child[0]);

					emitRM((char *)"JGT", AC, 1, PC, (char *)"Jump to while part");
					skiploc = breakloc2;
					breakloc2 = emitSkip(1);

					emitComment((char *)"DO");
					GenerateCode(t->child[1]);
					emitRMAbs((char *)"LDA", PC, currloc, (char *)"go to beginning of loop");
					backPatchJumpToHere(breakloc2, (char *)"No more loop [backpatch]");
					breakloc2 = skiploc;

					emitComment((char *)"ENDWHILE");
					break;
				case ForEachK:
					break;
				case CompoundK:
					emitComment((char *)"BEGIN compound statement");
					GenerateCode(t->child[0]);
					GenerateCode(t->child[1]);
					emitComment((char *)"END compound statement");
					break;
				case ReturnK:
					tmp_expression = in_expression;
					in_expression = true;
					emitComment((char *)"RETURN");
					if(t->child[0]){
						GenerateCode(t->child[0]);
						if(t->child[0]->sType == Local || t->child[0]->sType == Param)
							offReg = 1;
						else
							offReg = 0;
						if(t->child[0]->isArray){
							if(t->child[0]->sType == Param)
                                                               emitRM2((char *)"LD", AC1, t->child[0]->loc, 1, (char *)"Load address of base of array", t->child[0]->attr.name);
	                                                else
                                                               emitRM2((char *)"LDA", AC1, t->child[0]->loc, offReg, (char *)"Load address of base of array", t->child[0]->attr.name);

          	         	                        emitRO((char *)"SUB", AC, AC1, AC, (char *)"Compute offset of value");
                	                                emitRM((char *)"LD", AC, 0, AC, (char *)"Load the value");
						}
							
						emitRM((char *)"LDA", RT, 0, AC, (char *)"Copy result to rt register");
					}
					/* emit return sequence */
					emitRM((char *)"LD", AC, OFPOFF, FP, (char *)"Load return address");
					emitRM((char *)"LD", FP, RETURN_OFFSET, FP, (char *)"Adjust fp");
					emitRM((char *)"LDA", PC, 0, AC, (char *)"Return");
					in_expression = tmp_expression;
					break;
				case BreakK:
					emitComment((char *)"BREAK");
					emitRMAbs((char *)"LDA", PC, breakloc2, (char *)"break");
					break;
			}
			break;
		case expK:
			int offReg = 1;
			switch(t->kind.exp){
				case OpK:
					tmp_expression = in_expression;
					if(t->attr.op == LEQK || t->attr.op == GEQK)
						in_expression = true;
					else
						in_expression = false;
					/* process LHS */
                                        GenerateCode(t->child[0]);
                                        /* process RHS if binary operator */
                                        if(t->child[1] != NULL){
	                                        emitRM((char *)"ST", AC, -frameSize + toff--, FP, (char *)"Save left side");
                                                GenerateCode(t->child[1]);
                                                emitRM((char *)"LD", AC1, -frameSize + ++toff, FP, (char *)"Load left into ac1");
					}
					switch(t->attr.op){
						case LEQK:
                                                        emitRO((char *)"SUB", AC1, AC1, AC, (char *)"Op <=");
							emitRM((char *)"LDC", AC, 1, AC3, (char *)"True case");
	                                                emitRM((char *)"JLE", AC1, 1, PC, (char *)"Jump if true");
        	                                        emitRM((char *)"LDC", AC, 0, AC3, (char *)"False case");
							break;
						case GEQK:
                                                        emitRO((char *)"SUB", AC1, AC1, AC, (char *)"Op >=");
							emitRM((char *)"LDC", AC, 1, AC3, (char *)"True case");
	                                                emitRM((char *)"JGE", AC1, 1, PC, (char *)"Jump if true");
        	                                        emitRM((char *)"LDC", AC, 0, AC3, (char *)"False case");
							break;
						case LTHANK:
                                                        emitRO((char *)"SUB", AC1, AC1, AC, (char *)"Op <");
							emitRM((char *)"LDC", AC, 1, AC3, (char *)"True case");
	                                                emitRM((char *)"JLT", AC1, 1, PC, (char *)"Jump if true");
        	                                        emitRM((char *)"LDC", AC, 0, AC3, (char *)"False case");
							break;
						case GTHANK:
                                                        emitRO((char *)"SUB", AC1, AC1, AC, (char *)"Op >");
							emitRM((char *)"LDC", AC, 1, AC3, (char *)"True case");
	                                                emitRM((char *)"JGT", AC1, 1, PC, (char *)"Jump if true");
        	                                        emitRM((char *)"LDC", AC, 0, AC3, (char *)"False case");
							break;
                                                case NEQK:
                                                        emitRO((char *)"SUB", AC, AC1, AC, (char *)"Op !=");
							emitRM((char *)"JEQ", AC, 1, PC, (char *)"Jump if true");
                                                        emitRM((char *)"LDC", AC, 1, AC3, (char *)"True case");
							break;
						case EQK:
                                                        emitRO((char *)"SUB", AC1, AC1, AC, (char *)"Op ==");
							emitRM((char *)"LDC", AC, 1, AC3, (char *)"True case");
	                                                emitRM((char *)"JEQ", AC1, 1, PC, (char *)"Jump if true");
        	                                        emitRM((char *)"LDC", AC, 0, AC3, (char *)"False case");
                                                        break;
                                                case MODK:
                                                        emitRO((char *)"DIV", AC2, AC1, AC, (char *)"Op %");
							emitRO((char *)"MUL", AC2, AC2, AC, (char *)"");
							emitRO((char *)"SUB", AC, AC1, AC2, (char *)"");
							break;
						case PLUSK:
							emitRO((char *)"ADD", AC, AC1, AC, (char *)"Op +");
							break;
						case SLASHK:
                                                        emitRO((char *)"DIV", AC, AC1, AC, (char *)"Op /");
                                                        break;
                                                case ASTERISKK:
							if(t->child[1] == NULL){
								emitRM((char *)"LD", AC, t->size + 1, AC, (char *)"Load array size");
							}
							else{
	                                                        emitRO((char *)"MUL", AC, AC1, AC, (char *)"Op *");
							}
							break;
						case MINUSK:
							if(t->child[1] != NULL){
								emitRO((char *)"SUB", AC, AC1, AC, (char *)"Op -");
							}
							else{
								emitRM((char *)"LDC", AC1, 0, AC3, (char *)"Load 0");
								emitRO((char *)"SUB", AC, AC1, AC, (char *)"Op unary -");
							}
                                                        break;
                                                case NOTK:
							emitRM((char *)"LDC", AC1, 1, AC3, (char *)"Load 1");
                                                        emitRO((char *)"SUB", AC, AC1, AC, (char *)"Op NOT");
                                                        break;
                                                case ANDK:
                                                        emitRM2((char *)"JEQ", AC, 1, PC, (char *)"Op AND", (char *)"");
							emitRM((char *)"LDA", AC, 0, AC1, (char *)"");
							break;
						case ORK:
                                                        emitRM((char *)"JGT", AC, 1, PC, (char *)"Op OR");
							emitRM((char *)"LDA", AC, 0, AC1, (char *)"");
                                                        break;
					}
					in_expression = tmp_expression;
					break;
				case ConstantK:
						if(t->expType == Integer){
							emitRM((char *)"LDC", AC, t->attr.val, AC3, (char *)"Load constant");
						}
						else if(t->expType == Char){
							emitRM((char *)"LDC", AC, t->attr.string[1], AC3, (char *)"Load constant");
						}
						else if(t->expType == Boolean){
							if(t->attr.bVal)
								emitRM((char *)"LDC", AC, 1, AC3, (char *)"Load constant");
							else
								emitRM((char *)"LDC", AC, 0, AC3, (char *)"Load constant");
						}
					break;
				case IdK:
						GenerateCode(t->child[0]);
						if(t->sType != Local && t->sType != Param){
                                                        offReg = 0;
                                                }
                                                else{
                                                        offReg = 1;
                                                }
						if(t->isArray){
								if(!in_expression){
									if(t->sType == Param){
										if(t->child[0] != NULL)
											emitRM2((char *)"LD", AC1, t->loc, 1, (char *)"Load address of base of array", t->attr.name);
										else
											emitRM2((char *)"LD", AC, t->loc, 1, (char *)"Load address of base of array", t->attr.name);
									}
									else{
										if(t->child[0] != NULL)
											emitRM2((char *)"LDA", AC1, t->loc, offReg, (char *)"Load address of base of array", t->attr.name);
										else
											emitRM2((char *)"LDA", AC, t->loc, offReg, (char *)"Load address of base of array", t->attr.name);
									}
									if(t->child[0] != NULL){
	        		                                                emitRO((char *)"SUB", AC, AC1, AC, (char *)"Compute offset of value");
        	        	                                        	emitRM((char *)"LD", AC, 0, AC, (char *)"Load the value");
									}
								}
						}
						else{
							emitRM2((char *)"LD", AC, t->loc, offReg, (char *)"Load variable", t->attr.name);
						}
					break;
				case AssignK:
					tmp_expression = in_expression;
					in_expression = true;
					switch(t->attr.op){
						GenerateCode(t->child[0]);
						case PASSIGNK:
							if(t->child[0]->isArray){
								in_expression = false;
								GenerateCode(t->child[0]->child[0]);
								emitRM((char *)"ST", AC, -frameSize + toff--, 1, (char *)"Save index");
							}
							in_expression = false;
							GenerateCode(t->child[1]);
							if(t->child[0]->sType == Local || t->child[0]->sType == Param){
                                                                offReg = 1;
                                                        }
                                                        else{
                                                                offReg = 0;
                                                        }
							if(t->child[0]->isArray){
								emitRM((char *)"LD", AC1, -frameSize + ++toff, 1, (char *)"Restore index");
                                                                if(t->child[0]->sType == Param)
                                                                	emitRM2((char *)"LD", AC2, t->child[0]->loc, 1, (char *)"Load address of base of array", t->child[0]->attr.name);
                                                                else
                                                                        emitRM2((char *)"LDA", AC2, t->child[0]->loc, offReg, (char *)"Load address of base of array", t->child[0]->attr.name);
                                                                emitRO((char *)"SUB", AC2, AC2, AC1, (char *)"Compute offset of value");
								emitRM2((char *)"LD", AC1, 0, AC2, (char *)"load lhs variable", t->child[0]->attr.name);
                                                                emitRO((char *)"ADD", AC, AC1, AC, (char *)"Op +=");
                                                                emitRM2((char *)"ST", AC, 0, AC2, (char *)"Store variable", t->child[0]->attr.name);
							}
							else{
								emitRM2((char *)"LD", AC1, t->child[0]->loc, offReg, (char *)"load lhs variable", t->child[0]->attr.name);
	                                                        emitRO((char *)"ADD", AC, AC1, AC, (char *)"Op +=");
                                                        	emitRM2((char *)"ST", AC, t->child[0]->loc, offReg, (char *)"Store variable", t->child[0]->attr.name);
							}
							break;
						case MASSIGNK:
							if(t->child[0]->isArray){
								in_expression = false;
								GenerateCode(t->child[0]->child[0]);
								emitRM((char *)"ST", AC, -frameSize + toff--, 1, (char *)"Save index");
							}
							in_expression = false;
							GenerateCode(t->child[1]);
							if(t->child[0]->sType == Local || t->child[0]->sType == Param){
                                                                offReg = 1;
                                                        }
                                                        else{
                                                                offReg = 0;
                                                        }
                                                        if(t->child[0]->isArray){
								emitRM((char *)"LD", AC1, -frameSize + ++toff, 1, (char *)"Restore index");
								if(t->child[0]->sType == Param)
									emitRM2((char *)"LD", AC2, t->child[0]->loc, 1, (char *)"Load address of base of array", t->child[0]->attr.name);
								else
	                                                                emitRM2((char *)"LDA", AC2, t->child[0]->loc, offReg, (char *)"Load address of base of array", t->child[0]->attr.name);
                                                                emitRO((char *)"SUB", AC2, AC2, AC1, (char *)"Compute offset of value");
								emitRM2((char *)"LD", AC1, 0, AC2, (char *)"load lhs variable", t->child[0]->attr.name);
                                                                emitRO((char *)"SUB", AC, AC1, AC, (char *)"Op -=");
                                                                emitRM2((char *)"ST", AC, 0, AC2, (char *)"Store variable", t->child[0]->attr.name);
							}
							else{
								emitRM2((char *)"LD", AC1, t->child[0]->loc, offReg, (char *)"load lhs variable", t->child[0]->attr.name);
        	                                                emitRO((char *)"SUB", AC, AC1, AC, (char *)"Op -=");
	                                                        emitRM2((char *)"ST", AC, t->child[0]->loc, offReg, (char *)"Store variable", t->child[0]->attr.name);
							}
							break;
						case EQUALK:
							if(t->child[0]->isArray){
								in_expression = false;
								GenerateCode(t->child[0]->child[0]);
								emitRM((char *)"ST", AC, -frameSize + toff--, 1, (char *)"Save index");
							}
							in_expression = false;
							GenerateCode(t->child[1]);
							if(t->child[0]->sType == Local || t->child[0]->sType == Param){
                                                                offReg = 1;
                                                        }
                                                        else{
                                                        	offReg = 0;
                                                        }
                                                        if(t->child[0]->isArray){
                		                                emitRM((char *)"LD", AC1, -frameSize + ++toff, 1, (char *)"Restore index");
                                                                if(t->child[0]->sType == Param)
                                                                        emitRM2((char *)"LD", AC2, t->child[0]->loc, 1, (char *)"Load address of base of array", t->child[0]->attr.name);
                                                                else
                                                                        emitRM2((char *)"LDA", AC2, t->child[0]->loc, offReg, (char *)"Load address of base of array", t->child[0]->attr.name);

	                                	                emitRO((char *)"SUB", AC2, AC2, AC1, (char *)"Compute offset of value");
								emitRM2((char *)"ST", AC, 0, AC2, (char *)"Store variable", t->child[0]->attr.name);
                                        		}
							else{
								emitRM2((char *)"ST", AC, t->child[0]->loc, offReg, (char *)"Store variable", t->child[0]->attr.name);
							}
							break;
						case INCK:
							if(t->child[0]->isArray){
								in_expression = false;
                                                                GenerateCode(t->child[0]->child[0]);
                                                                emitRM((char *)"ST", AC, -frameSize + toff--, 1, (char *)"Save index");
                                                        }
                                                        if(t->child[0]->sType == Local || t->child[0]->sType == Param){
                                                                offReg = 1;
                                                        }
                                                        else{
                                                                offReg = 0;
                                                        }
                                                        if(t->child[0]->isArray){
                                                                emitRM((char *)"LD", AC1, -frameSize + ++toff, 1, (char *)"Restore index");
                                                                if(t->child[0]->sType == Param)
                                                                        emitRM2((char *)"LD", AC2, t->child[0]->loc, 1, (char *)"Load address of base of array", t->child[0]->attr.name);
                                                                else
                                                                        emitRM2((char *)"LDA", AC2, t->child[0]->loc, offReg, (char *)"Load address of base of array", t->child[0]->attr.name);
								emitRO((char *)"SUB", AC2, AC2, AC1, (char *)"Compute offset of value");
                                                                emitRM2((char *)"LD", AC, 0, AC2, (char *)"load lhs variable", t->child[0]->attr.name);
								emitRM2((char *)"LDA", AC, 1, AC, (char *)"increment value of", t->child[0]->attr.name);
                                                                emitRM2((char *)"ST", AC, 0, AC2, (char *)"Store variable", t->child[0]->attr.name);
							}
							else{
								emitRM2((char *)"LD", AC, t->child[0]->loc, offReg, (char *)"load lhs variable", t->child[0]->attr.name);
        	                                                emitRM2((char *)"LDA", AC, 1, AC, (char *)"increment value of", t->child[0]->attr.name);
	                                                        emitRM2((char *)"ST", AC, t->child[0]->loc, offReg, (char *)"Store variable", t->child[0]->attr.name);
							}
							break;
						case DECK:
							if(t->child[0]->isArray){
								in_expression = false;
                                                                GenerateCode(t->child[0]->child[0]);
                                                                emitRM((char *)"ST", AC, -frameSize + toff--, 1, (char *)"Save index");
                                                        }
                                                        if(t->child[0]->sType == Local || t->child[0]->sType == Param){
                                                                offReg = 1;
                                                        }
                                                        else{
                                                                offReg = 0;
                                                        }
                                                        if(t->child[0]->isArray){
                                                                emitRM((char *)"LD", AC1, -frameSize + ++toff, 1, (char *)"Restore index");
                                                                if(t->child[0]->sType == Param)
                                                                        emitRM2((char *)"LD", AC2, t->child[0]->loc, 1, (char *)"Load address of base of array", t->child[0]->attr.name);
                                                                else
                                                                        emitRM2((char *)"LDA", AC2, t->child[0]->loc, offReg, (char *)"Load address of base of array", t->child[0]->attr.name);
                                                                emitRO((char *)"SUB", AC2, AC2, AC1, (char *)"Compute offset of value");
                                                                emitRM2((char *)"LD", AC, 0, AC2, (char *)"load lhs variable", t->child[0]->attr.name);
                                                                emitRM2((char *)"LDA", AC, -1, AC, (char *)"decrement value of", t->child[0]->attr.name);
                                                                emitRM2((char *)"ST", AC, 0, AC2, (char *)"Store variable", t->child[0]->attr.name);
                                                        }
                                                        else{
                                                                emitRM2((char *)"LD", AC, t->child[0]->loc, offReg, (char *)"load lhs variable", t->child[0]->attr.name);
                                                                emitRM2((char *)"LDA", AC, -1, AC, (char *)"decrement value of", t->child[0]->attr.name);
                                                                emitRM2((char *)"ST", AC, t->child[0]->loc, offReg, (char *)"Store variable", t->child[0]->attr.name);
                                                        }
                                                        break;
					}
					in_expression = tmp_expression;
					break;
				case CallK:
					int tmpoff = toff;
					tmp_expression = in_expression;
					in_expression = false;
					TreeNode *tmp = t->child[0];
					prev = (TreeNode *)symtab->lookup(t->attr.name);
					emitComment2((char *)"\t\t\tBegin call to", t->attr.name);
					emitRM((char *)"ST", FP, -frameSize + toff, 1, (char *)"Store old fp in ghost frame");
					toff -= 2;
					while(tmp != NULL){
						paramNum++;
						emitComment((char *)"\t\t\tLoad param");
						GenerateCode(tmp);
						/*	emitRM2((char *)"LDA", AC1, tmp->loc + 1, offReg, (char *)"Load address of base of array", tmp->attr.name);
							emitRO((char *)"SUB", AC, AC1, AC, (char *)"Compute offset of value");
							emitRM((char *)"LD", AC, 0, AC, (char *)"Load the value");
						*/
						emitRM((char *)"ST", AC, -frameSize + toff-- , FP, (char *)"Store parameter");
						tmp = tmp->sibling;
						paramNum--;
					}
					toff = tmpoff;
					emitComment2((char *)"\t\t\tJump to", t->attr.name);
					emitRM((char *)"LDA", FP, -frameSize + toff, 1, (char *)"Load address of new frame");
					emitRM((char *)"LDA", AC, FP, PC, (char *)"Return address in ac");
					emitRMAbs2((char *)"LDA", PC, prev->loc, (char *)"CALL", t->attr.name);
					emitRM((char *)"LDA", AC, RETURN_OFFSET, RT, (char *)"Save the result in ac");
					emitComment2((char *)"\t\t\tEnd call to", t->attr.name);
					in_expression = tmp_expression;
					
					break;
			}
			break;
	}
	if(paramNum == 0 && !globals_go){
		t = t->sibling;
	}
	else{
		t = NULL;
	}
  }
}

void PostCodeGen(){
	globals_go = true;
	TreeNode *glbl;
	TreeNode *main = (TreeNode *)symtab->lookup((char *)"main");

	if(main == NULL){
		printf("ERROR(LINKER): Procedure main is not defined.");
	}
	else{
		backPatchJumpToHere(0, (char *)"Jump to init [backpatch]");
		emitComment((char *)"BEGIN Init");
		emitRM((char *)"LD", 0, 0, 0, (char *)"Set the global pointer");
		emitComment((char *)"BEGIN init of globals");
		SymTabEntry *entryPtr = (SymTabEntry *)symtab->firstSymTabEntry();
		entryPtr = (SymTabEntry *)symtab->nextSymTabEntry(entryPtr);
		glbl = (TreeNode *)symtab->lookup(entryPtr->name);
		while(glbl != NULL){
			if(glbl->kind.decl == VarK){
				if(glbl->isArray){
                                	emitRM2((char *)"LDC", AC, glbl->size - 1, AC3, (char *)"Load size of array", entryPtr->name);
                                        emitRM2((char *)"ST", AC, glbl->loc + 1, 0, (char *)"Save size of array", entryPtr->name);
				}
				else{
					paramNum = 1;
					GenerateCode(glbl);
				}
			}
			entryPtr = (SymTabEntry *)symtab->nextSymTabEntry(entryPtr);
			if(entryPtr->name == NULL)
				break;
			glbl = (TreeNode *)symtab->lookup(entryPtr->name);
			if(glbl == NULL){
					break;
			}
		}

		emitComment((char *)"END init of globals");
		emitRM((char *)"LDA", FP, goffset, 0, (char *)"set first frame at end of globals");
		emitRM((char *)"ST", FP, 0, FP, (char *)"store old fp (point to self)");
		emitRM((char *)"LDA", AC, FP, PC, (char *)"Return address in ac");
		emitRMAbs((char *)"LDA", PC, main->loc, (char *)"Jump to main");
		emitRO((char *)"HALT", 0, 0, 0, (char *)"DONE!");
		emitComment((char *)"END Init");
	}

}
