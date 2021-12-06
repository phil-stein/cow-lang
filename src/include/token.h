#ifndef TOKEN_H
#define TOKEN_H

// ---- token ----

typedef enum token_type 
{
  TOKEN_EOF,

  TOKEN_REG_A,
  TOKEN_REG_B,
  TOKEN_REG_X,
  TOKEN_REG_Y,

  TOKEN_SYMBOL,
  TOKEN_SRT,      // start of subroutine
  TOKEN_EXT,      // exit
  TOKEN_NUM,      // number value
  TOKEN_STR,      // val is idx to string in array
  TOKEN_MOV,      // assign to register, i.e. mov a 1
  TOKEN_ADD,      
  TOKEN_SUB,
  TOKEN_MUL,
  TOKEN_DIV,
  TOKEN_MOD,      // modulo, i.e. '%'
  TOKEN_JMP,      // jump to label
  TOKEN_JMZ,      // jump if zero_flag set
  TOKEN_JNZ,      // jump if zero_flag not set
  TOKEN_JSR,      // jump to subroutine
  TOKEN_RTS,      // return from subroutine
  TOKEN_PSH,      // push value onto stack, i.e. add to end
  TOKEN_POP,      // pop value from stack,  i.e rm and rtn last
  TOKEN_PUT,      // put value in stack at position
  TOKEN_GET,      // get value at position
  TOKEN_OUT

}token_type;

typedef struct token_t
{
  token_type type;
  int val;
}token;

#define TOKEN_ARR_MAX 100
typedef token tok_arr[TOKEN_ARR_MAX];


#define P_TOK(v) \
    { char* name =                                 \
      v.type == TOKEN_EOF    ? "EOF" :             \
      v.type == TOKEN_REG_A  ? "REG_A" :           \
      v.type == TOKEN_REG_B  ? "REG_B" :           \
      v.type == TOKEN_REG_X  ? "REG_X" :           \
      v.type == TOKEN_REG_Y  ? "REG_Y" :           \
      v.type == TOKEN_SYMBOL ? "SYMBOL" :          \
      v.type == TOKEN_SRT    ? "SRT" :             \
      v.type == TOKEN_EXT    ? "EXT" :             \
      v.type == TOKEN_NUM    ? "NUM" :             \
      v.type == TOKEN_STR    ? "STR" :             \
      v.type == TOKEN_MOV    ? "MOV" :             \
      v.type == TOKEN_ADD    ? "ADD" :             \
      v.type == TOKEN_SUB    ? "SUB" :             \
      v.type == TOKEN_MUL    ? "MUL" :             \
      v.type == TOKEN_DIV    ? "DIV" :             \
      v.type == TOKEN_MOD    ? "MOD" :             \
      v.type == TOKEN_JMP    ? "JMP" :             \
      v.type == TOKEN_JMZ    ? "JMZ" :             \
      v.type == TOKEN_JNZ    ? "JNZ" :             \
      v.type == TOKEN_JSR    ? "JSR" :             \
      v.type == TOKEN_RTS    ? "RTS" :             \
      v.type == TOKEN_PSH    ? "PSH" :             \
      v.type == TOKEN_POP    ? "POP" :             \
      v.type == TOKEN_PUT    ? "PUT" :             \
      v.type == TOKEN_GET    ? "GET" :             \
      v.type == TOKEN_OUT    ? "OUT" :             \
      "ERROR";                                    \
      char n[32];                                 \
      if (v.type == TOKEN_NUM ||                  \
          v.type == TOKEN_STR ||                  \
          v.type == TOKEN_SYMBOL)                 \
      { snprintf(n, 32, "%s: %d", name, v.val); } \
      else { snprintf(n, 32, "%s", name); }       \
      printf("T | %s\n", n); }

#define T_IS_REG(tok)                \
    (tok.type == TOKEN_REG_A ||      \
     tok.type == TOKEN_REG_B ||      \
     tok.type == TOKEN_REG_X ||      \
     tok.type == TOKEN_REG_Y)        \

#define T_CMP(tok, _type)   (tok.type == (_type))

// ---- label ----

#define SYMBOL_ARR_MAX 32
#define SYMBOL_MAX  32
typedef enum symbol_type { SYM_LABEL, SYM_SUBR } symbol_type;
typedef struct symbol
{
  symbol_type type;
  int pos;
  char name[SYMBOL_MAX];

}symbol;

typedef symbol symbol_arr[SYMBOL_ARR_MAX];

#define P_SYMBOL(l)                                                 \
  { char* type = l.type == SYM_LABEL ? "LABEL" :                    \
    l.type == SYM_SUBR ? "SUBR" : "ERROR";                          \
    printf("S | '%s', pos: %d, type: %s\n", l.name, l.pos, type); }



void tokenize_text(const char* s, int s_len, tok_arr t, int* rtn_len);

int get_label_pos(int idx);
int get_label_pos_name(const char* name);

#endif
