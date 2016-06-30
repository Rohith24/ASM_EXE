#include "Header.h"
int data_ptr = 0;
int line_no = -1;
int registers[8];

int perform_opration(FILE *f, int opcode, int lineno){
int res, nextline = lineno;
	int source;
	struct IntermediateLang intlang;
	fseek(f, IL_OFFSET + 4 + ((lineno - 1)*IL_SIZE), 0);
	fread(&intlang, sizeof(intlang), 1, f);
	switch (opcode)
	{
	case 2:
		fseek(f, BLOCK_OFFSET, 0);
		fseek(f, intlang.params[1], 1);
		fread(&source, sizeof(int), 1, f);
		registers[intlang.params[0]] = source;
		break;
	case 1:
		source = registers[intlang.params[1]];
		fseek(f, BLOCK_OFFSET, 0);
		fseek(f, intlang.params[0], 1);
		fwrite(&source, sizeof(int), 1, f);
		break;
	case 3:
		registers[intlang.params[0]] = registers[intlang.params[1]] + registers[intlang.params[2]];
		break;
	case 4:
		registers[intlang.params[0]] = registers[intlang.params[1]] - registers[intlang.params[2]];
		break;
	case 5:
		registers[intlang.params[0]] = registers[intlang.params[1]] * registers[intlang.params[2]];
		break;
	case 6:
		nextline = intlang.params[0] - 1;
		break;
	case 13:
		printf("%d", registers[intlang.params[0]]);
		break;
	case 14:
		scanf("%d", &registers[intlang.params[0]]);
		break;
	default:
		break;
	}
	//nextline = nextline + 1;
	return nextline;
}

void execute(){

	FILE *f;
	f = fopen("store.bin", "r+b");
	int length;

	struct IntermediateLang intlang;
	fseek(f, IL_OFFSET, 0);
	fread(&length, sizeof(int), 1, f);
	int i = 0;
	while (i<length){
		int offset = IL_OFFSET + 4 + (i*IL_SIZE);
		fseek(f, offset, 0);
		fread(&intlang, sizeof(IntermediateLang), 1, f);
		i = perform_opration(f, intlang.opCode, intlang.line_no);
	}
}

void charToString(char c, char *res){

	char str1[2] = { c, '\0' };
	char str2[5] = "";
	strcpy(str2, str1);
	strcpy(res, str2);
}
void GetWord(char *line, int *i, char *word){

	int word_ptr = 0;
	while (1)
	{
		word[word_ptr] = line[*i];
		word_ptr++;
		*i = *i + 1;
		if (line[*i] == ' ' || line[*i] == '\0') {
			word[word_ptr] = '\0';
			*i = *i + 1;
			return;
		}
	}
}
void trim(char *line)
{

	int i = strlen(line) - 1;

	while (i > 0)
	{
		if (isspace(line[i]) || line[i] == ',')
		{
			line[i] = '\0';
		}
		else
		{
			break;
		}
		i--;
	}
}
int GetOpCode(char *check){

	for (int i = 0; i <= OPCODE; i++){
		if (strcmp(check, opcodes[i]) == 0) return i+2;
	}
	return -1;
}
void data_instr(char *word1){

	char word[6];
	strcpy(word, word1);
	FILE *fp;
	fp = fopen("store.bin", "r+b");
	// enter the var in the symbol table
	fseek(fp, SYMBOLE_OFFSET, SEEK_SET);
	
	struct SymbolTable entry;
	entry.data_addr = data_ptr;
	if (word[1] == '['){
		entry.size = word[2] - '0';
		char name[5];
		charToString(word[0], name);
		strcpy(entry.name, name);
	}
	else{
		entry.size = 1;
		strcpy(entry.name, word);
	}
	data_ptr+=4;

	int no;
	fread(&no, sizeof(int), 1, fp);
	//fseek(fp, -4, SEEK_CUR);
	// update the no and insert the entry
	// offset to free space
	fseek(fp, no*SYMBOLE_SIZE, SEEK_CUR);
	no++;
	fwrite(&entry, sizeof(SymbolTable), 1, fp);
	fseek(fp, SYMBOLE_OFFSET, SEEK_SET);
	fwrite(&no, sizeof(int), 1, fp);
	fclose(fp);
}
void read_instr(char *word){

	FILE *fp;
	fp = fopen("store.bin", "r+b");
	struct IntermediateLang entry;
	memset(&entry, -1, sizeof(entry));
	entry.line_no = line_no;
	entry.opCode = GetOpCode("READ");
	if (word[1] != 'X') {
		printf("wrong syntax");
		return;
	}
	else{
		entry.params[0] = word[0] - 'A';
	}
	fseek(fp, IL_OFFSET, SEEK_SET);
	int no;
	fread(&no, sizeof(int), 1, fp);
	fseek(fp, no*IL_SIZE, SEEK_CUR);
	fwrite(&entry, sizeof(IntermediateLang), 1, fp);
	fseek(fp, IL_OFFSET, SEEK_SET);
	no++;
	fwrite(&no, sizeof(int), 1, fp);
	fclose(fp);
}
void print_instr(char *word){
	FILE *fp;
	fp = fopen("store.bin", "r+b");
	struct IntermediateLang entry;
	memset(&entry, -1, sizeof(entry));
	entry.line_no = line_no;
	entry.opCode = GetOpCode("PRINT");
	if (word[1] != 'X') {
		printf("wrong syntax");
		return;
	}
	else{
		entry.params[0] = word[0] - 'A';
	}
	fseek(fp, IL_OFFSET, SEEK_SET);
	int no;
	fread(&no, sizeof(int), 1, fp);
	fseek(fp, no*IL_SIZE, SEEK_CUR);
	fwrite(&entry, sizeof(IntermediateLang), 1, fp);
	fseek(fp, IL_OFFSET, SEEK_SET);
	no++;
	fwrite(&no, sizeof(int), 1, fp);
	fclose(fp);
}
int SearchSymTable(char *var){

	FILE *fp;
	fp = fopen("store.bin", "r+b");
	fseek(fp, SYMBOLE_OFFSET, SEEK_SET);
	int no;
	fread(&no, sizeof(int), 1, fp);
	struct SymbolTable entry;

	for (int i = 0; i <= no; i++){

		memset(&entry, -1, sizeof(SymbolTable));
		int t = ftell(fp);
		//fseek(fp, i*SYMBOLE_SIZE, SEEK_CUR);
		fread(&entry, sizeof(SymbolTable), 1, fp);
		if (strcmp(entry.name, var) == 0)
			return entry.data_addr;
	}
	fclose(fp);
	return -1;
}
void mov_instr(char *oper1, char *oper2){

	FILE *fp;
	fp = fopen("store.bin", "r+b");
	// find the right instr
	int opcode;
	char is_register[5];
	char is_var[5];
	// register to mem
	if (oper1[1] == 'X'){
		opcode = 2;
		strcpy(is_register, oper1);
		strcpy(is_var, oper2);
	}		
	// mem to register
	else if (oper2[1] == 'X'){
		opcode = 1;
		strcpy(is_register, oper2);
		strcpy(is_var, oper1);
	}
		
	else {
		printf("wrong syntax..exiting..");
		_getch();
		exit(0);
	}

	struct IntermediateLang entry;
	memset(&entry, -1, sizeof(entry));
	entry.line_no = line_no;
	entry.opCode = opcode;
	if (opcode == 1){
		entry.params[1] = is_register[0] - 'A';
		entry.params[0] = SearchSymTable(is_var);
	}
	else{
		entry.params[0] = is_register[0] - 'A';
		entry.params[1] = SearchSymTable(is_var);
	}
	

	// update the values
	fseek(fp, IL_OFFSET, SEEK_CUR);
	int no;
	fread(&no, sizeof(int), 1, fp);
	// update the no and insert the entry
	// offset to free space
	fseek(fp, no*IL_SIZE, SEEK_CUR);	
	fwrite(&entry, sizeof(IntermediateLang), 1, fp);
	no++;
	fseek(fp, IL_OFFSET, SEEK_SET);
	fwrite(&no, sizeof(int), 1, fp);
	fclose(fp);
}
void aggreate_instr(char *oper1, char *oper2, char *oper3, int is_operator){

	FILE *fp;
	fp = fopen("store.bin", "r+b");
	struct IntermediateLang entry;
	memset(&entry, -1, sizeof(entry));
	entry.line_no = line_no;
	entry.opCode = is_operator;
	entry.params[0] = oper1[0] - 'A';
	entry.params[1] = oper2[0] - 'A';
	entry.params[2] = oper3[0] - 'A';

	// update the values
	fseek(fp, IL_OFFSET, SEEK_CUR);
	int no;
	fread(&no, sizeof(int), 1, fp);
	fseek(fp, no*IL_SIZE, SEEK_CUR);
	fwrite(&entry, sizeof(IntermediateLang), 1, fp);
	no++;
	fseek(fp, IL_OFFSET, SEEK_SET);
	fwrite(&no, sizeof(int), 1, fp);

	fclose(fp);
}
void label_instr(char word){

	// update in the block table
	FILE *fp;
	fp = fopen("store.bin", "r+b");

	struct BlockAddr entry;
	char res[5];
	charToString(word, res);
	strcpy(entry.block_name, res);
	entry.IL_addr = line_no;

	fseek(fp, BLOCK_OFFSET, SEEK_SET);
	int no;
	fread(&no, sizeof(int), 1, fp);
	no++;
	fseek(fp, no*BLOCK_SIZE, SEEK_CUR);
	fwrite(&entry, sizeof(BlockAddr), 1, fp);
	fseek(fp, BLOCK_OFFSET, SEEK_SET);
	fwrite(&no, sizeof(int), 1, fp);

	fclose(fp);
}
int ParseLine(char *line){

	int line_ptr = 0;
	char word[10];
	memset(word, 0, sizeof(word));
	GetWord(line, &line_ptr, word);
	//int opcode = GetOpCode(word);
	if (strcmp(word, "DATA") == 0){
		GetWord(line, &line_ptr, word);
		trim(word);
		data_instr(word);
	}
	else if (strcmp(word, "START:\n") == 0){

		// go to start
		line_no++;
		return 1;
	}
	else if (strcmp(word, "END") == 0){
		printf("compling is done...\n executing\n");
		execute();
	}
	else if (strcmp(word, "READ") == 0){

		GetWord(line, &line_ptr, word);
		trim(word);
		read_instr(word);
	}
	else if (strcmp(word, "MOV") == 0){

		char oper1[5];
		char oper2[5];
		GetWord(line, &line_ptr, oper1);
		GetWord(line, &line_ptr, oper2);
		trim(oper1);
		trim(oper2);
		mov_instr(oper1, oper2);
	}
	else if (strcmp(word, "ADD") == 0 || strcmp(word, "SUB") == 0 || strcmp(word, "MUL") == 0){
		char oper1[5];
		char oper2[5];
		char oper3[5];
		GetWord(line, &line_ptr, oper1);
		GetWord(line, &line_ptr, oper2);
		GetWord(line, &line_ptr, oper3);
		trim(oper1);
		trim(oper2);
		trim(oper3);
		int is_operator;
		if (strcmp(word, "ADD") == 0){
			is_operator = 3;
		}
		else if (strcmp(word, "SUB") == 0){
			is_operator = 4;
		}
		else if (strcmp(word, "MUL") == 0){
			is_operator = 5;
		}
		aggreate_instr(oper1, oper2, oper3, is_operator);
	}
	else if (strcmp(word,"PRINT")==0)
	{
		GetWord(line, &line_ptr, word);
		trim(word);
		print_instr(word);
	}
	else if (word[1] == ':'){
		label_instr(word[0]);
	}
	else {
		printf("wrong");
		return NULL;
	}
}
void start_instr(int start){

	FILE *fp, *input;
	fp = fopen("store.bin", "r+b");
	input = fopen("asmcode.txt", "r");
	fseek(input, start, SEEK_SET);

	while (!feof(input))
	{
		char line[50];
		fgets(line, 50, input);
		line_no++;
		ParseLine(line);
	}


	fclose(fp);
	fclose(input);
}
void Compile(){

	FILE *fp, *input;
	fp = fopen("store.bin", "r+b");
	input = fopen("asmcode.txt", "r");
	int j;
	int start;
	
	while (!feof(input))
	{
		// reading the data definitons
		char line[50];
		fgets(line, 50, input);
		// parse line
		j = ParseLine(line);
		if (j == 1) {
			start = ftell(input);
			break;
		}
	}

	start_instr(start);

	fclose(fp);
	fclose(input);
}
void main(){

	Compile();
	getchar();
}