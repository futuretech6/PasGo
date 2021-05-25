#ifndef UTIL_H
#define UTIL_H
#include <string>

#define ID_MAX_LEN 50
#define SYM_TAB_LEN 50

#define INTEGER int
#define REAL float
#define CHAR char
#define BOOLEAN bool
#define ADDRESS unsigned long

#define OP_ADD 0
#define OP_MINUS 1
#define OP_MULTI 2
#define OP_RDIV 3
#define OP_DDIV 4
#define OP_MOD 5
#define OP_AND 6
#define OP_OR 7
#define OP_SMALL 8
#define OP_LARGE 9
#define OP_SMALL_EQUAL 10
#define OP_LARGE_EQUAL 11
#define OP_EQUAL 12
#define OP_NOT_EQUAL 13
#define OP_DOT 14
#define OP_INDEX 15

#define OP_OPPO 20
#define OP_ABS 21
#define OP_PRED 22
#define OP_SUCC 23
#define OP_ODD 24
#define OP_CHR 25
#define OP_ORD 26
#define OP_NOT 27
#define OP_SQR 28
#define OP_SQRT 29

#define N_PROGRAM 0
#define N_DEFINE 1
#define N_BODY 2
#define N_SITUATION 3
#define N_LABEL_DEF 10
#define N_CONST_DEF 11
#define N_TYPE_DEF 12
#define N_VAR_DEF 13
#define N_FUNCTION_DEF 14
#define N_ARG_DEF 15
#define N_ASSIGN_STM 20
#define N_CALL_STM 21
#define N_CASE_STM 22
#define N_FOR_STM 23
#define N_GOTO_STM 24
#define N_IF_STM 25
#define N_LABEL_STM 26
#define N_REPEAT_STM 27
#define N_WHILE_STM 28
#define N_BINARY_EXP 40
#define N_CALL_EXP 41
#define N_CONSTANT_EXP 42
#define N_UNARY_EXP 43
#define N_VARIABLE_EXP 44
#define N_TYPE 50

#define TY_INTEGER 0
#define TY_REAL 1
#define TY_CHAR 2
#define TY_BOOLEAN 3
#define TY_STRING 4
#define TY_ARRAY 5
#define TY_RECORD 6

std::string getOpNameByID(int id);
extern "C" void yyerror(const char *info);

struct symTabNode {
    char id[ID_MAX_LEN];
    char type;
};

typedef struct symTabNode sym_tab_node;

#endif