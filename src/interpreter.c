#include "include/global.h"
#include "include/interpreter.h"


int a = 0;
int b = 0;
int x = 0;
int y = 0;

int tmp = -999;

bool zero_flag = false;

#define STACK_MAX 512
int stack[STACK_MAX] = { -1 }; // -1 : uninitialized
int stack_pos = 0;

#define SYM_STACK_MAX 32
int sym_stack[SYM_STACK_MAX] = { -1 }; // -1 : uninitialized
int sym_stack_pos = 0;


#define STACK_PUSH(val) stack[stack_pos] = (val); stack_pos++
#define STACK_POP()     stack[--stack_pos] 

#define SYM_STACK_PUSH(val) sym_stack[sym_stack_pos] = (val); sym_stack_pos++
#define SYM_STACK_POP()     sym_stack[--sym_stack_pos] 


#define REG_TOK_VAL(tok)                 \
    ((tok).type == TOKEN_REG_A ? a :     \
     (tok).type == TOKEN_REG_B ? b :     \
     (tok).type == TOKEN_REG_X ? x :     \
     (tok).type == TOKEN_REG_Y ? y : tmp)

#define TOK_VAL(tok)\
  ( T_IS_REG(tok) ? REG_TOK_VAL(tok) : tok.val )

#define REG_TOK_ASSIGN(tok, val)                  \
    switch (tok.type)                             \
    {                                             \
      case TOKEN_REG_A: a = (val);                \
        printf("mov a: %d\n", a); break;          \
      case TOKEN_REG_B: b = (val);                \
        printf("mov b: %d\n", b); break;          \
      case TOKEN_REG_X: x = (val);                \
        printf("mov x: %d\n", x); break;          \
      case TOKEN_REG_Y: y = (val);                \
        printf("mov y: %d\n", y); break;          \
      default: break;                             \
    }

#define REG_ADD(tok1, val)                                  \
    {int result = REG_TOK_VAL(tok1) + (val);                \
     zero_flag = result == 0; P_IL("add ");                 \
     /* if (zero_flag) P_IL("set zero_flag "); */           \
     REG_TOK_ASSIGN((tok1), result);  }

#define REG_ADD_REG(tok1, tok2)                             \
    {int result = REG_TOK_VAL(tok1) + REG_TOK_VAL(tok2);    \
     zero_flag = result == 0; P_IL("add ");                 \
     /* if (zero_flag) P_IL("set zero_flag "); */           \
     REG_TOK_ASSIGN((tok1), result);  }

#define REG_SUB(tok1, val)                                  \
    {int result = REG_TOK_VAL(tok1) - (val);                \
     zero_flag = result == 0; P_IL("sub ");                 \
     /* if (zero_flag) P_IL("set zero_flag "); */           \
     REG_TOK_ASSIGN((tok1), result);  }

#define REG_SUB_REG(tok1, tok2)                             \
    {int result = REG_TOK_VAL(tok1) - REG_TOK_VAL(tok2);    \
     zero_flag = result == 0; P_IL("sub ");                 \
     /* if (zero_flag) P_IL("set zero_flag "); */           \
     REG_TOK_ASSIGN((tok1), result);  }

#define REG_MUL(tok1, val)                                  \
    {int result = REG_TOK_VAL(tok1) * (val);                \
     zero_flag = result == 0; P_IL("mul ");                 \
     /* if (zero_flag) P_IL("set zero_flag "); */           \
     REG_TOK_ASSIGN((tok1), result);  }

#define REG_MUL_REG(tok1, tok2)                             \
    {int result = REG_TOK_VAL(tok1) * REG_TOK_VAL(tok2);    \
     zero_flag = result == 0; P_IL("mul ");                 \
     /* if (zero_flag) P_IL("set zero_flag "); */           \
     REG_TOK_ASSIGN((tok1), result);  }

#define REG_DIV(tok1, val)                                  \
    {int result = REG_TOK_VAL(tok1) / (val);                \
     zero_flag = result == 0; P_IL("div ");                 \
     /* if (zero_flag) P_IL("set zero_flag "); */           \
     REG_TOK_ASSIGN((tok1), result);  }

#define REG_DIV_REG(tok1, tok2)                             \
    {int result = REG_TOK_VAL(tok1) / REG_TOK_VAL(tok2);    \
     zero_flag = result == 0; P_IL("div ");                 \
     /* if (zero_flag) P_IL("set zero_flag "); */           \
     REG_TOK_ASSIGN((tok1), result);  }

#define REG_MOD(tok1, val)                                  \
    {int result = REG_TOK_VAL(tok1) % (val);                \
     zero_flag = result == 0; P_IL("mod ");                 \
     /* if (zero_flag) P_IL("set zero_flag "); */           \
     REG_TOK_ASSIGN((tok1), result);  }

#define REG_MOD_REG(tok1, tok2)                             \
    {int result = REG_TOK_VAL(tok1) % REG_TOK_VAL(tok2);    \
     zero_flag = result == 0; P_IL("mod ");                 \
     /* if (zero_flag) P_IL("set zero_flag "); */           \
     REG_TOK_ASSIGN((tok1), result);  }


void interpret_tokens(tok_arr t, int t_len)
{
 
  for (int i = 0; i < t_len; ++i)
  {
    // skip subroutines
    if (t[i].type == TOKEN_SRT)
    { while (t[i].type != TOKEN_RTS) { i++; }  continue; }  // continue for rts

    if (t[i].type == TOKEN_MOV)
    {
      // next token isnt a register
      // if (!T_IS_REG(t[i +1])) { assert(0); continue; }
      assert(T_IS_REG(t[i +1]));
      // next token isnt a number 
      // if (!T_CMP(t[i +2], TOKEN_NUM)) { assert(0); continue; }
      // assert(T_CMP(t[i +2], TOKEN_NUM));
      assert(T_CMP(t[i +2], TOKEN_NUM) || T_IS_REG(t[i +2]));

      if (T_CMP(t[i +2], TOKEN_NUM))
      { REG_TOK_ASSIGN(t[i +1], t[i +2].val); }
      else
      { REG_TOK_ASSIGN(t[i +1], REG_TOK_VAL(t[i +2])); }
      
      i += 2;
      continue;
    }
    if (t[i].type == TOKEN_ADD)
    {
      // next token isnt a register
      assert(T_IS_REG(t[i +1]));
      // next token isnt a register or number 
      assert(T_CMP(t[i +2], TOKEN_NUM) || T_IS_REG(t[i +2]));
    
      if (T_CMP(t[i +2], TOKEN_NUM))
      {
        REG_ADD(t[i +1], t[i +2].val);
      }
      else      
      {
        // REG_TOK_ASSIGN(t[i +1], reg + REG_TOK_VAL(t[i +2]));
        REG_ADD_REG(t[i +1], t[i +2])
      }
     
      i += 2;
      continue;
    }
    if (t[i].type == TOKEN_SUB)
    {
      // next token isnt a register
      assert(T_IS_REG(t[i +1]));
      // next token isnt a regidter or number 
      assert(T_CMP(t[i +2], TOKEN_NUM) || T_IS_REG(t[i +2]));
      
      if (T_CMP(t[i +2], TOKEN_NUM))
      {
        REG_SUB(t[i +1], t[i +2].val);
      }
      else      
      {
        // REG_TOK_ASSIGN(t[i +1], reg - REG_TOK_VAL(t[i +2]));
        REG_SUB_REG(t[i +1], t[i +2]);
      }
      
      i += 2;
      continue;
    }
    if (t[i].type == TOKEN_MUL)
    {
      // next token isnt a register
      assert(T_IS_REG(t[i +1]));
      // next token isnt a regidter or number 
      assert(T_CMP(t[i +2], TOKEN_NUM) || T_IS_REG(t[i +2]));
      
      if (T_CMP(t[i +2], TOKEN_NUM))
      {
        REG_MUL(t[i +1], t[i +2].val);
      }
      else      
      {
        REG_MUL_REG(t[i +1], t[i +2]);
      }
      
      i += 2;
      continue;
    }
    if (t[i].type == TOKEN_DIV)
    {
      // next token isnt a register
      assert(T_IS_REG(t[i +1]));
      // next token isnt a regidter or number 
      assert(T_CMP(t[i +2], TOKEN_NUM) || T_IS_REG(t[i +2]));
      
      if (T_CMP(t[i +2], TOKEN_NUM))
      {
        REG_DIV(t[i +1], t[i +2].val);
      }
      else      
      {
        REG_DIV_REG(t[i +1], t[i +2]);
      }
      
      i += 2;
      continue;
    }
    if (t[i].type == TOKEN_MOD)
    {
      // next token isnt a register
      assert(T_IS_REG(t[i +1]));
      // next token isnt a regidter or number 
      assert(T_CMP(t[i +2], TOKEN_NUM) || T_IS_REG(t[i +2]));
      
      if (T_CMP(t[i +2], TOKEN_NUM))
      {
        REG_MOD(t[i +1], t[i +2].val);
      }
      else      
      {
        REG_MOD_REG(t[i +1], t[i +2]);
      }
      
      i += 2;
      continue;
    }
    if (t[i].type == TOKEN_JMP)
    {
      // @UNSURE: make it also accept line numbers
      // next token isnt a label
      assert(T_CMP(t[i +1], TOKEN_SYMBOL));
      
      i = get_label_pos(t[i +1].val) -1;  // -1 because continue 
      if (is_debug()) printf("jmp %d\n", i +1);
      // i += 1;
      continue;
    }
    if (t[i].type == TOKEN_JMZ)
    {
      // @UNSURE: make it also accept line numbers
      // next token isnt a label
      assert(T_CMP(t[i +1], TOKEN_SYMBOL));
      
      if (zero_flag)
      { i = get_label_pos(t[i +1].val) -1; if (is_debug()) {printf("jmz %d\n", i +1);}  }  // -1 because continue 
      else 
      { i += 1; }   // just skip
      continue;
    }
    if (t[i].type == TOKEN_JNZ)
    {
      // @UNSURE: make it also accept line numbers
      // next token isnt a label
      assert(T_CMP(t[i +1], TOKEN_SYMBOL));
      
      if (!zero_flag)
      { i = get_label_pos(t[i +1].val) -1;  if (is_debug()) {printf("jnz %d\n", i +1);} }  // -1 because continue 
      else 
      { i += 1; }   // just skip 
      continue;
    }
    if (t[i].type == TOKEN_JSR)
    {
      // next token isnt a label
      assert(T_CMP(t[i +1], TOKEN_SYMBOL));
      
      SYM_STACK_PUSH(i +1);
      i = get_label_pos(t[i +1].val);  // no -1 bc. continue, bc skipping label
      if (is_debug()) printf("jsr %d, rst: %d\n", i +1, sym_stack[sym_stack_pos -1]); 
      continue;
    }
    if (t[i].type == TOKEN_RTS)
    {
      i = SYM_STACK_POP(); // -1 because continue
      if (is_debug()) P("rts"); 
      continue;
    }
    if (t[i].type == TOKEN_EXT)
    {
      int code = 1;
      if (T_CMP(t[i +1], TOKEN_NUM)) { code = t[i +1].val; }
      if (is_debug()) P("ext"); P("..."); 
      exit(code);
      continue;
    }
    if (t[i].type == TOKEN_PSH)
    {
      // next token isnt a label
      assert(T_CMP(t[i +1], TOKEN_NUM));
      STACK_PUSH(t[i +1].val); 
      if (is_debug()) printf("psh %d\n", t[i +1].val); 
      continue;
    }
    if (t[i].type == TOKEN_POP)
    {
      assert(T_IS_REG(t[i +1]));
      REG_TOK_ASSIGN(t[i +1], STACK_POP()); 
      if (is_debug()) printf("pop %d\n", t[i +1].val); 
      continue;
    }
    if (t[i].type == TOKEN_PUT)
    {
      // next token is num/reg and the next too
      bool b = ( T_CMP(t[i +1], TOKEN_NUM) || T_IS_REG(t[i +1]) ) &&
               ( T_CMP(t[i +2], TOKEN_NUM) || T_IS_REG(t[i +2]) );
      assert(b);
      stack[TOK_VAL(t[i +1])] = TOK_VAL(t[i +2]);
      printf("put[%d]: %d\n", TOK_VAL(t[i +1]), TOK_VAL(t[i +2]));
      continue;
    }
    if (t[i].type == TOKEN_GET)
    {
      // get address register, i.e. get 3 a
      assert(T_CMP(t[i +1], TOKEN_NUM));
      assert(T_IS_REG(t[i +2]));
      int get = stack[t[i +1].val];
      printf("get[%d] ", t[i +1].val);
      REG_TOK_ASSIGN(t[i +2], get);
      continue;
    }
    if (t[i].type == TOKEN_OUT)
    { 
      assert(T_CMP(t[i +1], TOKEN_STR) || 
             T_CMP(t[i +1], TOKEN_NUM) ||
             T_IS_REG(t[i +1]));
      if (T_CMP(t[i +1], TOKEN_NUM))
      {
        if (is_debug()) printf("out: num | ");
        printf("%d\n", t[i +1].val);
        continue;
      }
      if (T_IS_REG(t[i +1]))
      {
        if (is_debug()) printf("out: reg | ");
        switch (t[i +1].type)
        {
          case TOKEN_REG_A: P_INT(a); break;
          case TOKEN_REG_B: P_INT(b); break;
          case TOKEN_REG_X: P_INT(x); break;
          case TOKEN_REG_Y: P_INT(y); break;
          default: break;
        }
        continue;
      }
      if (is_debug()) printf("out: str[%d] | ", t[i +1].val);
      P(str_get(t[i +1].val));  // val is idx
      continue;
    }
  }
}

