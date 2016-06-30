#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<conio.h>
#include<math.h>
#include<stack>
#include<malloc.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#define SYMBOLE_OFFSET 0
#define BLOCK_OFFSET 512
#define IL_OFFSET 1024
#define DATA_OFFSET 2048
#define OPCODE 13
#define SYMBOLE_SIZE 12
#define IL_SIZE 24
#define BLOCK_SIZE 8

char opcodes[][6] = {
	"MOV",
	"ADD",
	"SUB",
	"MUL",
	"JMP",
	"IF",
	"EQ",
	"LT",
	"GT",
	"LTEQ",
	"GTEQ",
	"PRINT",
	"READ"
};

struct SymbolTable{

	char name[4];
	int data_addr;
	int size;
};

struct IntermediateLang{

	int line_no;
	int opCode;
	int params[4];
};

struct BlockAddr{

	char block_name[4];
	int IL_addr;
};


/*START:
READ AX
READ BX
MOV A, AX
MOV B, BX
ADD CX, AX, BX
END*/