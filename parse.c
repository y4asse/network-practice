#include <stdio.h>
#include <stdlib.h>
#include <getsym.h>
#include <string.h>

extern TOKEN tok;
extern FILE *infile;
extern FILE *outfile;

typedef struct
{
	int addr;
	char v[MAXIDLEN + 1];
} s_entry;

s_entry s_table[32];
int count = 0;

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

//
// Parser
//

void outblock(void)
{
	if (tok.attr == RWORD && tok.value == VAR)
	{
		do
		{
			getsym();
			if (tok.attr == IDENTIFIER)
			{
				s_table[count].addr = count;
				strcpy(s_table[count].v, tok.charvalue);
				fprintf(outfile, "loadi r0, 0\n");
				fprintf(outfile, "store r0, %d\n", count++);
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

// void var_declaration(void)
// {
// 	do
// 	{
// 		getsym();

// 		if (tok.attr == IDENTIFIER)
// 		{
// 			getsym();

// 			if (tok.attr == SYMBOL && tok.value == SEMICOLON)
// 				getsym();
// 			else
// 				error("Semicolon is needed.");
// 		}
// 		else
// 			error("Variable name is needed.");
// 	} while (tok.attr == IDENTIFIER);
// }

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
			}
		}

		getsym();

		if (tok.attr == SYMBOL && tok.value == BECOMES)
		{
			getsym();
			expression();
			fprintf(outfile, "store r0, %d\n", memoryAddress);
		}
		else if (tok.attr == SYMBOL && tok.value == LPAREN)
		{
			getsym();
			parmlist();
		}
		else
			error("Assignment operator or Left parenthesis is needed.");
	}
	else if (tok.attr == RWORD && tok.value == BEGIN)
	{

		getsym();

		statement();

		while (tok.attr == SYMBOL && tok.value == SEMICOLON)
		{
			getsym();
			statement();
		}

		if (tok.attr == RWORD && tok.value == END)
			getsym();
		else
			error("End is needed.");
	}
	else if (tok.attr == RWORD && tok.value == IF)
	{
		getsym();
		condition();

		if (tok.attr == RWORD && tok.value == THEN)
		{
			getsym();
			statement();

			if (tok.attr == RWORD && tok.value == ELSE)
			{
				getsym();
				statement();
			}
		}
		else
			error("Then is needed.");
	}
	else if (tok.attr == RWORD && tok.value == WHILE)
	{
		getsym();
		condition();

		if (tok.attr == RWORD && tok.value == DO)
		{
			getsym();
			statement();
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
				fprintf(outfile, "loadi r1,'\\n'\n");
				fprintf(outfile, "writec r1\n");
				getsym();
			}
			else if (tok.attr == IDENTIFIER)
			{
				for (int i = 0; i < sizeof(s_table) / sizeof s_table[0]; i++)
				{
					if (strcmp(s_table[i].v, tok.charvalue) == 0)
					{
						fprintf(outfile, "load r0, %d\n", s_table[i].addr);
					}
				}
				fprintf(outfile, "writed r0\n");
				fprintf(outfile, "loadi r1,'\\n'\n");
				fprintf(outfile, "writec r1\n");
				getsym();
			}
			else
				error("Statement is needed");
		} while (tok.attr == SYMBOL && tok.value == COMMA);
	}
	else
		error("Statement is needed.");
}

void expression(void)
{
	// if (tok.attr == NUMBER)
	// {
	// 	fprintf(outfile, "loadi r0 %d", tok.value);
	// 	getsym();
	// 	if (tok.attr == SYMBOL)
	// 	{
	// 	}
	// }
	// else if (tok.attr == IDENTIFIER)
	// {
	// 	int memoryAddress;
	// 	for (int i = 0; i < sizeof(s_table); i++)
	// 	{
	// 		if (strcmp(tok.charvalue, s_table[i].v))
	// 			memoryAddress = s_table[i].addr;
	// 	}
	// 	fprintf(outfile, "load r0 %d", memoryAddress);
	// 	getsym();
	// }
	// else
	// 	error("Number or Identifier is needed");

	if (tok.attr == NUMBER)
	{
		fprintf(outfile, "loadi r0, %d\n", tok.value);
		getsym();
		if (tok.attr == SYMBOL)
		{
			switch (tok.value)
			{
			case PLUS:
				getsym();
				if (tok.attr == NUMBER)
				{
					fprintf(outfile, "addi r0, %d\n", tok.value);
					getsym();
				}
				else
				{
					error("Second operand should be a number or identifier.");
				}
				break;
			case MINUS:
				getsym();
				if (tok.attr == NUMBER)
				{
					fprintf(outfile, "subi r0, %d\n", tok.value);
					getsym();
				}
				else
				{
					error("Second operand should be a number or identifier.");
				}
				break;
			case TIMES:
				getsym();
				if (tok.attr == NUMBER)
				{
					fprintf(outfile, "muli r0, %d\n", tok.value);
					getsym();
				}
				else if (tok.attr == IDENTIFIER)
				{
					for (int i = 0; i < sizeof(s_table) / sizeof s_table[0]; i++)
					{
						if (strcmp(s_table[i].v, tok.charvalue) == 0)
						{
							fprintf(outfile, "mul r0, %d\n", s_table[i].addr);
						}
					}
					getsym();
				}
				else
				{
					error("Second operand should be a number or identifier.");
				}
				break;
			default:
				break;
			}
		}
		else if (tok.attr == RWORD && tok.value == DIV)
		{
			getsym();
			if (tok.attr == NUMBER)
			{
				fprintf(outfile, "divi r0, %d\n", tok.value);
				getsym();
			}
			else
			{
				error("Second operand should be a number or identifier.");
			}
		}
		else
			error("symbol or div is needed.");
	}
	else if (tok.attr == IDENTIFIER)
	{
		for (int i = 0; i < sizeof(s_table) - 1; i++)
		{
			if (strcmp(tok.charvalue, s_table[i].v))
				fprintf(outfile, "load r0 %d", s_table[i].addr);
		}
	}
	else
		error("Number or Identifier is needed");
}

void parmlist(void)
{
	if (tok.attr == SYMBOL && tok.value == LPAREN)
	{
		getsym();
		expression();
		while (tok.attr == SYMBOL && tok.value == COMMA)
		{
			getsym();
			expression();
		}

		if (tok.attr == SYMBOL && tok.value == RPAREN)
			getsym();
		else
			error("Right parenthesis is needed.");
	}
	else
		error("Left parenthesis is needed.");
}

void condition(void)
{
	expression();

	if (tok.attr == SYMBOL)
	{
		switch (tok.value)
		{
		case LESSTHAN:
		case LESSEQL:
		case EQL:
		case NOTEQL:
		case GRTRTHAN:
		case GRTREQL:
		default:
			// その他の場合の処理
			break;
		}
		getsym();
		expression();
	}
	else
		error("Relational operator is needed.");
}