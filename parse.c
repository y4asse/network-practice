#include <stdio.h>
#include <stdlib.h>
#include <getsym.h>
#include <string.h>
#define MAX_DATA 32

extern TOKEN tok;
extern FILE *infile;
extern FILE *outfile;

// メモリ
typedef struct
{
	int addr;
	char v[MAXIDLEN + 1];
} s_entry;
s_entry s_table[32];
int tableIndex = 0;
int labelNumber = 0;
int dataLabelNumber[] = {};

void error(char *s);
void statement(void);
void outblock(void);
void inblock(void);
void statement(void);
void expression(void);
void condition(void);
void parmlist(void);

void compiler(void)
{
	init_getsym();

	getsym();

	if (tok.attr == RWORD && tok.value == PROGRAM)
	{

		getsym();

		if (tok.attr == IDENTIFIER)
		{

			getsym();

			if (tok.attr == SYMBOL && tok.value == SEMICOLON)
			{

				getsym();

				outblock();

				if (tok.attr == SYMBOL && tok.value == PERIOD)
				{
					fprintf(stderr, "Parsing Completed. No errors found.\n");
				}
				else
					error("At the end, a period is required.");
			}
			else
				error("After program name, a semicolon is needed.");
		}
		else
			error("Program identifier is needed.");
	}
	else
		error("At the first, program declaration is required.");
}

void error(char *s)
{
	fprintf(stderr, "%s\n", s);
	exit(1);
}

void outblock(void)
{
	if (tok.attr == RWORD && tok.value == VAR)
	{
		do
		{
			getsym();
			if (tok.attr == IDENTIFIER)
			{
				s_table[tableIndex].addr = tableIndex;
				strcpy(s_table[tableIndex].v, tok.charvalue);
				fprintf(outfile, "loadi r0, 0\n");
				fprintf(outfile, "store r0, %d\n", tableIndex++);
				getsym();
			}
			else
				error("Variable name is needed.");
		} while (tok.attr == SYMBOL && tok.value == COMMA);

		if (tok.attr == SYMBOL && tok.value == SEMICOLON)
			getsym();
		else
			error("Semicolon is needed");
	}

	// 手続き定義
	// while (tok.attr == RWORD && tok.value == PROCEDURE)
	// {
	// 	getsym();

	// 	if (tok.attr == IDENTIFIER)
	// 	{
	// 		getsym();
	// 		if (tok.attr == SYMBOL && tok.value == SEMICOLON)
	// 		{
	// 			getsym();
	// 			inblock();
	// 			getsym();

	// 			if (tok.attr == SYMBOL && tok.value == SEMICOLON)
	// 				getsym();
	// 			else
	// 				error("Semicolon is needed.");
	// 		}
	// 		else
	// 			error("Semicolon is needed.");
	// 	}
	// 	else
	// 		error("Procedure name is needed.");
	// }

	// メインのブロック
	statement();
}

void inblock(void)
{
	if (tok.attr == RWORD && tok.value == VAR)
		// var_declaration();

		statement();
}

void statement(void)
{
	if (tok.attr == IDENTIFIER)
	{
		int memoryAddress;
		for (int i = 0; i < sizeof(s_table) / sizeof s_table[0]; i++)
		{
			if (strcmp(s_table[i].v, tok.charvalue) == 0)
			{
				memoryAddress = s_table[i].addr;
				break;
			}
			if (i == sizeof(s_table) / sizeof s_table[0] - 1)
			{
				error("Undeclared variable is used");
			}
		}

		getsym();

		if (tok.attr == SYMBOL && tok.value == BECOMES)
		{
			getsym();
			expression();
			fprintf(outfile, "store r0, %d\n", memoryAddress);
		}
		// else if (tok.attr == SYMBOL && tok.value == LPAREN)
		// {
		// 	getsym();
		// 	parmlist();
		// }
		else
			error("Assignment operator or Left parenthesis is needed.");
	}
	else if (tok.attr == RWORD && tok.value == BEGIN)
	{
		do
		{
			getsym();
			statement();
		} while (tok.attr == SYMBOL && tok.value == SEMICOLON);

		if (tok.attr == RWORD && tok.value == END)
		{
			getsym();
		}
		else
		{
			printf("%d, %d\n", tok.attr, tok.value);
			error("End is needed.");
		}
	}
	else if (tok.attr == RWORD && tok.value == IF)
	{
		labelNumber += 2;
		int localNumber = labelNumber;
		getsym();
		condition();

		if (tok.attr == RWORD && tok.value == THEN)
		{
			getsym();
			statement();
			fprintf(outfile, "jmp L%d\n", localNumber - 1);
			fprintf(outfile, "L%d:\n", localNumber - 2);

			if (tok.attr == RWORD && tok.value == ELSE)
			{
				getsym();
				statement();
			}
			fprintf(outfile, "L%d:\n", localNumber - 1);
		}
		else
			error("Then is needed.");
	}
	else if (tok.attr == RWORD && tok.value == WHILE)
	{
		labelNumber += 2;
		int localNumber = labelNumber;
		fprintf(outfile, "L%d:\n", localNumber - 1);
		getsym();
		condition();

		if (tok.attr == RWORD && tok.value == DO)
		{
			getsym();
			statement();
			fprintf(outfile, "jmp L%d\n", localNumber - 1);
			fprintf(outfile, "L%d:\n", localNumber - 2);
		}
		else
			error("Do is needed.");
	}
	else if (tok.attr == RWORD && tok.value == WRITE)
	{
		do
		{
			getsym();
			if (tok.attr == NUMBER)
			{
				fprintf(outfile, "loadi r0, %d\n", tok.value);
				fprintf(outfile, "writed r0\n");
				fprintf(outfile, "loadi r0,'\\n'\n");
				fprintf(outfile, "writec r0\n");
				getsym();
			}
			else if (tok.attr == IDENTIFIER)
			{
				for (int i = 0; i < sizeof(s_table) / sizeof s_table[0]; i++)
				{
					if (strcmp(s_table[i].v, tok.charvalue) == 0)
					{
						fprintf(outfile, "load r0, %d\n", s_table[i].addr);
						break;
					}
					if (i == sizeof(s_table) / sizeof s_table[0] - 1)
					{
						error("Undeclared variable is used");
					}
				}
				fprintf(outfile, "writed r0\n");
				fprintf(outfile, "loadi r0,'\\n'\n");
				fprintf(outfile, "writec r0\n");
				getsym();
			}
			else
				error("Statement is needed");
		} while (tok.attr == SYMBOL && tok.value == COMMA);
	}
	else
	{
		error("Statement is needed.");
	}
}

int rIndex = 0;
void factor(void)
{
	if (tok.attr == SYMBOL && tok.value == MINUS)
	{
		getsym();
	}
	if (tok.attr == IDENTIFIER)
	{
		for (int i = 0; i < sizeof(s_table) / sizeof s_table[0]; i++)
		{
			if (strcmp(s_table[i].v, tok.charvalue) == 0)
			{
				fprintf(outfile, "load r%d, %d\n", rIndex, s_table[i].addr);
				break;
			}
			if (i == sizeof(s_table) / sizeof s_table[0] - 1)
			{
				error("Undeclared variable is used");
			}
		}
		rIndex++;
		getsym();
	}
	else if (tok.attr == NUMBER)
	{
		if (tok.value > 32767 || tok.value < -32767)
		{
			fprintf(outfile, "load r%d, DATA1\n", rIndex);
			fprintf(outfile, "DATA1: data 100000\n");
		}
		else
		{
			fprintf(outfile, "loadi r%d, %d\n", rIndex, tok.value);
		}
		rIndex++;
		getsym();
	}
	else if (tok.attr == SYMBOL && tok.value == LPAREN)
	{
		getsym();
		int tempRIndex = rIndex;
		// レジスタ退避
		for (int i = 0; i < tempRIndex; i++)
		{
			fprintf(outfile, "push r%d\n", i);
		}
		rIndex = 0;
		expression();
		rIndex = tempRIndex;
		// expressionの結果をrIndexに格納する
		fprintf(outfile, "loadr r%d, r0\n", rIndex);
		// レジスタ復元
		for (int i = tempRIndex - 1; i >= 0; i--)
		{
			fprintf(outfile, "pop r%d\n", i);
		}
		rIndex++;
		if (tok.attr == SYMBOL && tok.value == RPAREN)
		{
			getsym();
		}
		else
		{
			error(") is needed");
		}
	}
	else
	{
		error("Number, IDENTIFIER or ( is needed");
	}
}

void term(void)
{
	factor();
	while ((tok.attr == SYMBOL && tok.value == TIMES) || (tok.attr == RWORD && tok.value == DIV))
	{
		if (tok.value == TIMES)
		{
			getsym();
			factor();
			fprintf(outfile, "mulr r%d, r%d\n", rIndex - 2, rIndex - 1);
		}
		else
		{
			getsym();
			factor();
			fprintf(outfile, "divr r%d, r%d\n", rIndex - 2, rIndex - 1);
		}
		rIndex--;
	}
}

void expression(void)
{
	term();
	while ((tok.attr == SYMBOL && tok.value == PLUS) || (tok.attr == SYMBOL && tok.value == MINUS))
	{
		if (tok.value == PLUS)
		{
			getsym();
			term();
			fprintf(outfile, "addr r0, r1\n");
		}
		else
		{
			getsym();
			term();
			fprintf(outfile, "subr r0, r1\n");
		}
		rIndex--;
	};
	rIndex = 0;
	// if (tok.attr == NUMBER)
	// {
	// 	fprintf(outfile, "loadi r0, %d\n", tok.value);
	// 	getsym();
	// 	if (tok.attr == SYMBOL)
	// 	{
	// 		switch (tok.value)
	// 		{
	// 		case PLUS:
	// 			getsym();
	// 			if (tok.attr == NUMBER)
	// 			{
	// 				fprintf(outfile, "addi r0, %d\n", tok.value);
	// 				getsym();
	// 			}
	// 			else if (tok.attr == IDENTIFIER)
	// 			{
	// 				for (int i = 0; i < sizeof(s_table) / sizeof s_table[0]; i++)
	// 				{
	// 					if (strcmp(s_table[i].v, tok.charvalue) == 0)
	// 					{
	// 						fprintf(outfile, "add r0, %d\n", s_table[i].addr);
	// 						break;
	// 					}
	// 					if (i == sizeof(s_table) / sizeof s_table[0] - 1)
	// 					{
	// 						error("Undeclared variable is used");
	// 					}
	// 				}
	// 				getsym();
	// 			}
	// 			else
	// 			{
	// 				error("Second operand should be a number or identifier.");
	// 			}
	// 			break;
	// 		case MINUS:
	// 			getsym();
	// 			if (tok.attr == NUMBER)
	// 			{
	// 				fprintf(outfile, "subi r0, %d\n", tok.value);
	// 				getsym();
	// 			}
	// 			else if (tok.attr == IDENTIFIER)
	// 			{
	// 				for (int i = 0; i < sizeof(s_table) / sizeof s_table[0]; i++)
	// 				{
	// 					if (strcmp(s_table[i].v, tok.charvalue) == 0)
	// 					{
	// 						fprintf(outfile, "sub r0, %d\n", s_table[i].addr);
	// 						break;
	// 					}
	// 					if (i == sizeof(s_table) / sizeof s_table[0] - 1)
	// 					{
	// 						error("Undeclared variable is used");
	// 					}
	// 				}
	// 				getsym();
	// 			}
	// 			else
	// 			{
	// 				error("Second operand should be a number or identifier.");
	// 			}
	// 			break;
	// 		case TIMES:
	// 			getsym();
	// 			if (tok.attr == NUMBER)
	// 			{
	// 				fprintf(outfile, "muli r0, %d\n", tok.value);
	// 				getsym();
	// 			}
	// 			else if (tok.attr == IDENTIFIER)
	// 			{
	// 				for (int i = 0; i < sizeof(s_table) / sizeof s_table[0]; i++)
	// 				{
	// 					if (strcmp(s_table[i].v, tok.charvalue) == 0)
	// 					{
	// 						fprintf(outfile, "mul r0, %d\n", s_table[i].addr);
	// 						break;
	// 					}
	// 					if (i == sizeof(s_table) / sizeof s_table[0] - 1)
	// 					{
	// 						error("Undeclared variable is used");
	// 					}
	// 				}
	// 				getsym();
	// 			}
	// 			else
	// 			{
	// 				error("Second operand should be a number or identifier.");
	// 			}
	// 			break;
	// 		default:
	// 			break;
	// 		}
	// 	}
	// 	else if (tok.attr == RWORD && tok.value == DIV)
	// 	{
	// 		getsym();
	// 		if (tok.attr == NUMBER)
	// 		{
	// 			fprintf(outfile, "divi r0, %d\n", tok.value);
	// 			getsym();
	// 		}
	// 		else if (tok.attr == IDENTIFIER)
	// 		{
	// 			for (int i = 0; i < sizeof(s_table) / sizeof s_table[0]; i++)
	// 			{
	// 				if (strcmp(s_table[i].v, tok.charvalue) == 0)
	// 				{
	// 					fprintf(outfile, "div r0, %d\n", s_table[i].addr);
	// 					break;
	// 				}
	// 				if (i == sizeof(s_table) / sizeof s_table[0] - 1)
	// 				{
	// 					error("Undeclared variable is used");
	// 				}
	// 			}
	// 			getsym();
	// 		}
	// 		else
	// 		{
	// 			error("Second operand should be a number or identifier.");
	// 		}
	// 	}
	// }
	// else if (tok.attr == IDENTIFIER)
	// {
	// 	for (int i = 0; i < sizeof(s_table) / sizeof s_table[0]; i++)
	// 	{
	// 		if (strcmp(s_table[i].v, tok.charvalue) == 0)
	// 		{
	// 			fprintf(outfile, "load r0, %d\n", s_table[i].addr);
	// 			break;
	// 		}
	// 		if (i == sizeof(s_table) / sizeof s_table[0] - 1)
	// 		{
	// 			error("Undeclared variable is used");
	// 		}
	// 	}
	// 	getsym();
	// 	if (tok.attr == SYMBOL)
	// 	{
	// 		switch (tok.value)
	// 		{
	// 		case PLUS:
	// 			getsym();
	// 			if (tok.attr == NUMBER)
	// 			{
	// 				fprintf(outfile, "addi r0, %d\n", tok.value);
	// 				getsym();
	// 			}
	// 			else if (tok.attr == IDENTIFIER)
	// 			{
	// 				for (int i = 0; i < sizeof(s_table) / sizeof s_table[0]; i++)
	// 				{
	// 					if (strcmp(s_table[i].v, tok.charvalue) == 0)
	// 					{
	// 						fprintf(outfile, "add r0, %d\n", s_table[i].addr);
	// 						break;
	// 					}
	// 					if (i == sizeof(s_table) / sizeof s_table[0] - 1)
	// 					{
	// 						error("Undeclared variable is used");
	// 					}
	// 				}
	// 				getsym();
	// 			}
	// 			else
	// 			{
	// 				error("Second operand should be a number or identifier.");
	// 			}
	// 			break;
	// 		case MINUS:
	// 			getsym();
	// 			if (tok.attr == NUMBER)
	// 			{
	// 				fprintf(outfile, "subi r0, %d\n", tok.value);
	// 				getsym();
	// 			}
	// 			else if (tok.attr == IDENTIFIER)
	// 			{
	// 				for (int i = 0; i < sizeof(s_table) / sizeof s_table[0]; i++)
	// 				{
	// 					if (strcmp(s_table[i].v, tok.charvalue) == 0)
	// 					{
	// 						fprintf(outfile, "sub r0, %d\n", s_table[i].addr);
	// 						break;
	// 					}
	// 					if (i == sizeof(s_table) / sizeof s_table[0] - 1)
	// 					{
	// 						error("Undeclared variable is used");
	// 					}
	// 				}
	// 				getsym();
	// 			}
	// 			else
	// 			{
	// 				error("Second operand should be a number or identifier.");
	// 			}
	// 			break;
	// 		case TIMES:
	// 			getsym();
	// 			if (tok.attr == NUMBER)
	// 			{
	// 				fprintf(outfile, "muli r0, %d\n", tok.value);
	// 				getsym();
	// 			}
	// 			else if (tok.attr == IDENTIFIER)
	// 			{
	// 				for (int i = 0; i < sizeof(s_table) / sizeof s_table[0]; i++)
	// 				{
	// 					if (strcmp(s_table[i].v, tok.charvalue) == 0)
	// 					{
	// 						fprintf(outfile, "mul r0, %d\n", s_table[i].addr);
	// 						break;
	// 					}
	// 					if (i == sizeof(s_table) / sizeof s_table[0] - 1)
	// 					{
	// 						error("Undeclared variable is used");
	// 					}
	// 				}
	// 				getsym();
	// 			}
	// 			else
	// 			{
	// 				error("Second operand should be a number or identifier.");
	// 			}
	// 			break;
	// 		default:
	// 			break;
	// 		}
	// 	}
	// 	else if (tok.attr == RWORD && tok.value == DIV)
	// 	{
	// 		getsym();
	// 		if (tok.attr == NUMBER)
	// 		{
	// 			fprintf(outfile, "divi r0, %d\n", tok.value);
	// 			getsym();
	// 		}
	// 		else if (tok.attr == IDENTIFIER)
	// 		{
	// 			for (int i = 0; i < sizeof(s_table) / sizeof s_table[0]; i++)
	// 			{
	// 				if (strcmp(s_table[i].v, tok.charvalue) == 0)
	// 				{
	// 					fprintf(outfile, "div r0, %d\n", s_table[i].addr);
	// 					break;
	// 				}
	// 				if (i == sizeof(s_table) / sizeof s_table[0] - 1)
	// 				{
	// 					error("Undeclared variable is used");
	// 				}
	// 			}
	// 			getsym();
	// 		}
	// 		else
	// 		{
	// 			error("Second operand should be a number or identifier.");
	// 		}
	// 	}
	// }
	// else
	// 	error("Number or Identifier is needed");
}

// void parmlist(void)
// {
// 	if (tok.attr == SYMBOL && tok.value == LPAREN)
// 	{
// 		do
// 		{
// 			getsym();
// 			expression();
// 		} while (tok.attr == SYMBOL && tok.value == COMMA);
// 		if (tok.attr == SYMBOL && tok.value == RPAREN)
// 			getsym();
// 		else
// 		{
// 			error("Right parenthesis is needed.");
// 		}
// 	}
// 	else
// 		error("Left parenthesis is needed.");
// }

void condition(void)
{
	expression();
	fprintf(outfile, "loadi r1, 0\n");
	fprintf(outfile, "addr r1, r0\n");
	if (tok.attr == SYMBOL)
	{
		char comp[10];
		switch (tok.value)
		{
		case LESSTHAN:
			strcpy(comp, "jge");
			getsym();
			break;
		case LESSEQL:
			strcpy(comp, "jgt");
			getsym();
			break;
		case EQL:
			strcpy(comp, "jnz");
			getsym();
			break;
		case NOTEQL:
			strcpy(comp, "jz");
			getsym();
			break;
		case GRTRTHAN:
			strcpy(comp, "jle");
			getsym();
			break;
		case GRTREQL:
			strcpy(comp, "jlt");
			getsym();
			break;
		default:
			error("Relational operator is needed.");
			break;
		}
		expression();
		fprintf(outfile, "cmpr r1, r0\n");
		fprintf(outfile, "%s L%d\n", comp, labelNumber - 2);
	}
	else
		error("Relational operator is needed.");
}