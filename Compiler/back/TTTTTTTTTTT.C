%{

int LINE_NUM=1;

typedef   struct variable_type {     char *name;     int typeid; } variable_type;
static variable_type KEYWORD []= {  
{  "BEGIN",  1 },         
{  "DECLARE",  2 },
{  "ELSE",  3 },
{  "EXIT",  4 },
{  "FOR",  5 },
{  "IF",  6 },
{  "IN",  7 },
{  "INT",  8 },
{  "IS",  9 },
{  "LOOP",  10 },
{  "PROCEDURE",  11 },
{  "READ",  12 },
{  "THEN",  13 },
{  "WRITE",  14 },
{  "WHILE",  15 }
};  

static variable_type IDENTIFIER []= {   
{  "IDENTIFIER",  16 }, 
{  "INTEGER_CONSTANT",  17 },
};
 
static variable_type OPERATOR []= {    
{  "COLON",  18},  
{  "DOUBLEDOT",  19},  
{  "SEMICOLON",  20},  
{  "PLUS", 21},  
{  "MINUS",  22},  
{  "ASTERISK",  23}, 
{  "SLASH",  24}, 
{  "INTEGER", 25},
{  "PERCENT",  26}, 
{  "EQ",  27},  
{  "EQQ",  28},  
{  "GR",  29}, 
{  "GREQ",  30}, 
{  "LE",  31},  
{  "LEEQ",  32},
{  "AND", 33},
{  "OR", 34},
{  "QUESTION", 35},
{  "ASIGN", 36},
{  "OPEN", 37},
{  "CLOSE", 38},
{  "BOPEN", 39}, 
{  "BCLOSE", 40},
{  "SLASH",  41},
{  "COMMENT", 42},
{  "COMMA",43},
{  "QUESTIONASSIN" ,44}
}; 

int length_KEYWORD = sizeof(KEYWORD) / sizeof(KEYWORD[0]);
int length_IDENTIFIER = sizeof(IDENTIFIER) / sizeof(IDENTIFIER[0]);
int length_OPERATOR = sizeof(OPERATOR) / sizeof(OPERATOR[0]);


void lex_error(char* msg, int line);
char* install_id(char* ch);
int install_int(char* ch);
int outputtype =0;
union YYLVAL{
	char* str;
	int value;
} yylval;
%}

/*--------------------------------------------------*/
/*Token 6.*/
COMMENT		"//"[^\n]+

/*Token 5.*/
BLANK		[ ]
TAB		[\t]
NEWLINE		[\n]
WIDESPACE	({BLANK}|{TAB})+

/*Token 1.*/
UPPER_LETTER	[a-z]
UNDERSCORE	[_]
DIGIT		[0-9]
IDENTIFIER	({UPPER_LETTER}|{UNDERSCORE})({UPPER_LETTER}|{UNDERSCORE}|{DIGIT})*

/*Token 3.*/
INTCONSTANT	{DIGIT}+

/*--------------------------------------------------*/
%%

{NEWLINE}	{LINE_NUM++;}
{WIDESPACE}	{/*These tokens are ignored and are not returned.*/}
{COMMENT}	{/*return get_type("COMMENT");*/}
"else"		{return get_type("ELSE");}
"exit"		{return get_type("EXIT");}
"int"		{return get_type("INT");}
"if"		{return get_type("IF");}
"read"		{return get_type("READ");}
"while"	{return get_type("WHILE");} 
"write"	{return get_type("WRITE");} 

{IDENTIFIER}	{yylval.str=install_id(yytext);return get_type("IDENTIFIER"); }
{INTCONSTANT}	{yylval.value=install_int(yytext);return get_type("INTEGER_CONSTANT");} 

"+"		{return get_type("PLUS");}
"-"		{return get_type("MINUS");}
"*"		{return get_type("ASTERISK");}
"/"		{return get_type("SLASH");}
"%"		{return get_type("PERCENT");}
"=="		{return get_type("EQQ");}
"!="		{return get_type("QUESTIONASSIN");}
">"		{return get_type("GR");}
">="		{return get_type("GREQ");}
"<"		{return get_type("LE");}
"<="		{return get_type("LEEQ");}
"&&"		{return get_type("AND");}
"||"		{return get_type("OR");}
"!"		{return get_type("QUESTION");}
"="		{return get_type("EQ");}
";"		{return get_type("SEMICOLON");}
","		{return get_type("COMMA");}
"("		{return get_type("OPEN");}
")"		{return get_type("CLOSE");} 
"{"		{return get_type("BOPEN");} 
"}"		{return get_type("BCLOSE");}  
.		{lex_error(yytext,LINE_NUM);}
%%
/*--------------------------------------------------*/
void  print_token(char *token_type,char *token_name){
	if(outputtype==1)
		if(strcmp(token_name,"INTEGER_CONSTANT")==0)
			fprintf(stdout,"%-20s", "Integer Constant");
		else
			fprintf(stdout,"%-20s", token_type);
}

int get_type(char* ch){


	for (int i =0 ;i < length_KEYWORD ; i++)
		if(strcmp(ch,KEYWORD[i].name)==0){
			print_token("Keyword",ch);
			return KEYWORD[i].typeid;
		}
	for (int i =0 ;i < length_IDENTIFIER ; i++)		
		if(strcmp(ch,IDENTIFIER[i].name)==0){
			print_token("Identifier",ch);
			return IDENTIFIER[i].typeid;
		}
	for (int i =0 ;i < length_OPERATOR ; i++)		
		if(strcmp(ch,OPERATOR[i].name)==0){
			print_token("Operator",ch);
			return OPERATOR[i].typeid;
		}

	return -1;
}
char* install_id(char* ch){
	return ch;
}

int install_int(char* ch){

	int temp=atoi(ch);
	return temp;
}

void lex_error(char* msg, int line) {
    if(outputtype==0)
    	fprintf(stderr,"Lexical error: line %d: unknown character %c\n",line,*msg);
}