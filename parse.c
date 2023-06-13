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
int rIndex = 0;
int tableIndex = 0;
int labelNumber = 0;
int dataLabelNumber[100];
int dataLabelIndex = 0;

void error(char *s);
void statement(void);
void outblock(void);
void statement(void);
void expression(void);
void condition(void);

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
					fprintf(outfile, "halt\n");
					for (int i = 0; i < dataLabelIndex; i++)
					{
						fprintf(outfile, "Data%d: data %d\n", i, dataLabelNumber[i]);
					}
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
	// var 変数宣言の処理
	if (tok.attr == RWORD && tok.value == VAR)
	{
		do // コンマになるまで繰り返す
		{
			getsym();
			if (tok.attr == IDENTIFIER)
			{
				// 変数を記号表に保存する処理
				s_table[tableIndex].addr = tableIndex;
				strcpy(s_table[tableIndex].v, tok.charvalue);
				fprintf(outfile, "loadi r0, 0\n");
				fprintf(outfile, "store r0, %d\n", tableIndex++);
				getsym();
			}
			else
				error("Variable name is needed.");
		} while (tok.attr == SYMBOL && tok.value == COMMA);

		if (tok.attr == SYMBOL && tok.value == SEMICOLON) // セミコロンの時終わる
			getsym();
		else
			error("Semicolon is needed");
	}
	statement();
}

void statement(void)
{
	rIndex = 0; // レジスタのインデックス
	if (tok.attr == IDENTIFIER)
	{
		int memoryAddress; // メモリアドレスを一時的に保存する
		// 逐次探索により，記号表から変数名に一致するものを取得する
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

		if (tok.attr == SYMBOL && tok.value == BECOMES) //:=の時の処理
		{
			getsym();
			expression();
			fprintf(outfile, "store r0, %d\n", memoryAddress);
		}
		else
			error("Assignment operator or Left parenthesis is needed.");
	}
	else if (tok.attr == RWORD && tok.value == BEGIN) // beginの時の処理
	{
		do
		{
			getsym();
			statement();
		} while (tok.attr == SYMBOL && tok.value == SEMICOLON); // セミコロンになるまで続ける

		if (tok.attr == RWORD && tok.value == END) // endになったら正常に終わる
		{
			getsym();
		}
		else
		{
			printf("%d, %d\n", tok.attr, tok.value);
			error("End is needed.");
		}
	}
	else if (tok.attr == RWORD && tok.value == IF) // ifの時の処理
	{
		labelNumber += 2;			   // 使用する文のラベルを加算する
		int localNumber = labelNumber; // 一時的にラベルの番号を保存する
		getsym();
		condition();

		if (tok.attr == RWORD && tok.value == THEN) // thenの時の処理
		{
			getsym();
			statement();
			fprintf(outfile, "jmp L%d\n", localNumber - 1); // 先ほど保持しておいたラベルの番号を使用して，jmpを作成
			fprintf(outfile, "L%d:\n", localNumber - 2);	// 先ほど保持しておいたラベルの番号を使用して，ifのラベルを作成

			if (tok.attr == RWORD && tok.value == ELSE) // elseの時の処理
			{
				getsym();
				statement();
			}
			fprintf(outfile, "L%d:\n", localNumber - 1); // 先ほど保持しておいたラベルの番号を使用して，ifのラベルを作成
		}
		else
			error("Then is needed.");
	}
	else if (tok.attr == RWORD && tok.value == WHILE) // whileの時
	{
		labelNumber += 2;			   // 使用する文のラベルを加算する
		int localNumber = labelNumber; // 一時的にラベルの番号を保存する
		fprintf(outfile, "L%d:\n", localNumber - 1);
		getsym();
		condition();

		if (tok.attr == RWORD && tok.value == DO)
		{
			getsym();
			statement();
			fprintf(outfile, "jmp L%d\n", localNumber - 1); // 先ほど保持しておいたラベルの番号を使用して，jmpを作成
			fprintf(outfile, "L%d:\n", localNumber - 2);	// 先ほど保持しておいたラベルの番号を使用して，ラベルを作成
		}
		else
			error("Do is needed.");
	}
	else if (tok.attr == RWORD && tok.value == WRITE) // writeの時の処理
	{
		do
		{
			getsym();
			if (tok.attr == NUMBER)
			{
				// numberだったらそのままレジスタに入れて出力
				fprintf(outfile, "loadi r0, %d\n", tok.value);
				fprintf(outfile, "writed r0\n");
				fprintf(outfile, "loadi r0,'\\n'\n");
				fprintf(outfile, "writec r0\n");
				getsym();
			}
			else if (tok.attr == IDENTIFIER)
			{
				// 変数だったら，記号表から逐次探索で合致するものを取得して，レジスタに格納して出力
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

void factor(void)
{
	if (tok.attr == IDENTIFIER) // 変数の時の処理
	{
		// 逐次探索で記号表から合致する変数の値を取得
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
		rIndex++; // レジスタの番号を1個ずらす
		getsym();
	}
	else if (tok.attr == NUMBER)
	{
		// 数字が16bitを超えるときの処理
		if (tok.value > 32767 || tok.value < -32767)
		{
			dataLabelNumber[dataLabelIndex] = tok.value; // dataLabelNumberという変数にデータを保存し，あとで，ラベルとして出力する
			fprintf(outfile, "load r%d, Data%d\n", rIndex, dataLabelIndex++);
		}
		else
		{
			fprintf(outfile, "loadi r%d, %d\n", rIndex, tok.value);
		}
		rIndex++; // レジスタの番号を1個ずらす
		getsym();
	}
	else if (tok.attr == SYMBOL && tok.value == LPAREN) //(の時の処理
	{
		getsym();
		expression();
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
	while ((tok.attr == SYMBOL && tok.value == TIMES) || (tok.attr == RWORD && tok.value == DIV)) //*か，divの間繰り返す
	{
		if (tok.value == TIMES) //*の時の処理
		{
			getsym();
			factor();
			fprintf(outfile, "mulr r%d, r%d\n", rIndex - 2, rIndex - 1);
		}
		else // divの時の処理
		{
			getsym();
			factor();
			fprintf(outfile, "divr r%d, r%d\n", rIndex - 2, rIndex - 1);
		}
		rIndex--; // ずらしていたレジスタインデックスを元に戻す
	}
}

void expression(void)
{
	term();
	while ((tok.attr == SYMBOL && tok.value == PLUS) || (tok.attr == SYMBOL && tok.value == MINUS)) //+か-の間繰り返す
	{
		if (tok.value == PLUS) //+の時の処理
		{
			getsym();
			term();
			fprintf(outfile, "addr r%d, r%d\n", rIndex - 2, rIndex - 1);
		}
		else //-の時の処理
		{
			getsym();
			term();
			fprintf(outfile, "subr r%d, r%d\n", rIndex - 2, rIndex - 1);
		}
		rIndex--; // ずらしていたレジスタインデックスを元に戻す
	}
}

void condition(void)
{
	rIndex = 0; // expressionに入る前にレジスタの番号を初期化する
	expression();
	fprintf(outfile, "loadi r1, 0\n");
	fprintf(outfile, "addr r1, r0\n"); // expressionでr0に入っていた値をとりだす
	if (tok.attr == SYMBOL)
	{
		char comp[10];	   // 一時的に文字列を保存しておく変数
		switch (tok.value) // strcpyを使って，文字列をcompにコピーしておく
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
		fprintf(outfile, "cmpr r0, r1\n");					 // cmprを実行する
		fprintf(outfile, "%s L%d\n", comp, labelNumber - 2); // ラベルナンバーを設定する
	}
	else
		error("Relational operator is needed.");
}