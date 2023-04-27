#include <stdio.h>
#include <stdlib.h>
#include <getsym.h>

extern TOKEN tok;
extern FILE *infile;
extern FILE *outfile;

void error(char *s);
void statement(void);

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

				statement();

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

void statement(void)
{

	if (tok.attr == RWORD && tok.value == BEGIN)
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
			error("Missing 'end' or semicolon in the statement.");
		}
	}
	else if (tok.attr == NUMBER)
	{

		int value = tok.value;
		fprintf(outfile, "loadi r0,%d\n", value);
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
					fprintf(outfile, "writed r0\n");
					fprintf(outfile, "loadi r1,'\\n'\n");
					fprintf(outfile, "writec r1\n");
					getsym();
				}
				else
				{
					error("Second operand should be a number.");
				}
				break;
			case MINUS:
				getsym();
				if (tok.attr == NUMBER)
				{
					fprintf(outfile, "subi r0, %d\n", tok.value);
					fprintf(outfile, "writed r0\n");
					fprintf(outfile, "loadi r1,'\\n'\n");
					fprintf(outfile, "writec r1\n");
					getsym();
				}
				else
				{
					error("Second operand should be a number.");
				}
				break;
			case TIMES:
				getsym();
				if (tok.attr == NUMBER)
				{
					fprintf(outfile, "muli r0, %d\n", tok.value);
					fprintf(outfile, "writed r0\n");
					fprintf(outfile, "loadi r1,'\\n'\n");
					fprintf(outfile, "writec r1\n");
					getsym();
				}
				else
				{
					error("Second operand should be a number.");
				}
				break;
			default:
				error("Invalid operator in the statement.");
			}
		}
		else if (tok.attr == RWORD && tok.value == DIV)
		{
			getsym();
			if (tok.attr == NUMBER)
			{
				fprintf(outfile, "divi r0, %d\n", tok.value);
				fprintf(outfile, "writed r0\n");
				fprintf(outfile, "loadi r1,'\\n'\n");
				fprintf(outfile, "writec r1\n");
				getsym();
			}
			else
			{
				error("Second operand should be a number.");
			}
		}
		else
		{
			error("Operator is required after the number.");
		}
	}
	else
	{
		error("Statement should start with a number or 'begin'.");
	}
}
