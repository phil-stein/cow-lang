#include "include/global.h"
#include "include/token.h"

symbol_arr labels;
int labels_len = 0;

// state, needs to be static so no re-defenition
static bool debug    = false;
static bool show_src = false;
static bool only_tok = false;

// ---- func decls ----
INLINE void gather_symbols(const char* s, int s_len, tok_arr t);
INLINE void gather_tokens(const char* s, int s_len, tok_arr t, int* rtn_len);

// all code here just inline into other functions for ease of use
void tokenize_text(const char* s, int s_len, tok_arr t, int* rtn_len)
{
  debug    = is_debug();
  show_src = is_show_src();
  only_tok = is_only_tok();
  if (debug) P(" ---- symbols ---- ");
  gather_symbols(s, s_len, t);
  
  if (debug) P(" ---- tokens ---- ");
  gather_tokens(s, s_len, t, rtn_len);
}

INLINE void gather_symbols(const char* s, int s_len, tok_arr t)
{
  char buf[32];
  int buf_pos = 0; 
  bool is_decl = false;   // for symbols, decl. vs ref.
  int line = 0;
  bool is_subr = false; // current symbol is a subroutine
  // pre-tokens, get all labels
  for (int i = 0; i < s_len; ++i)
  {
    // skip comments
    if (s[i] == '#' || s[i] == ';')
    { while (s[i] != '\n') { i++; } }
    // skip whitespace
    if (isspace(s[i]) || 
        isdigit(s[i]) ) 
    { line = s[i] == '\n' ? line +1 : line; continue; }


    if (s[i] == '-' && s[i +1] == '>') { is_subr = true; i += 2; }
    buf[0] = '\0'; // removing the prev str
    buf_pos = 0;
    is_decl = false;
    while (isalnum(s[i]) || s[i] == '_' || s[i] == ':')
    {
      buf[buf_pos] = s[i];
      buf_pos++;
      if (s[i] == ':') { is_decl = true; break; } // breaking before i++ so the loop can just continue
      i++;
    }
    // new label
    if (buf_pos > 0 && is_decl)
    {
      PF("%s | ", buf);
      buf[buf_pos -1] = '\0'; // removing the ':'

      symbol l; 
      strcpy(l.name, buf);
      l.pos = line; 

      l.type = is_subr ? SYM_SUBR : SYM_LABEL;
      is_subr = false;

      labels[labels_len] = l;
      labels_len++;
      if (debug) PF("%d | ", labels_len -1);  P_SYMBOL(l);
    }
  
    // check for '\n' after i++
    int old_line = line;
    line = s[i] == '\n' ? line +1 : line;
    is_subr = line != old_line ? false : is_subr; // reset on line change
  }

}


INLINE void gather_tokens(const char* s, int s_len, tok_arr t, int* rtn_len)
{
  // @TODO: resize token arr when too small

#define ADD(_type, _val)    \
  { token tok; tok.val = (_val); tok.type = (_type); t[t_len] = tok; } t_len++

#define ADD_T(_type)    \
  { token tok; tok.val = 0; tok.type = (_type); t[t_len] = tok; } t_len++
  

  int t_len = 0; 
  char buf[32];
  int buf_pos = 0; 
  bool is_subr = false; // to look for subroutine starts

  // used to check labels whose pos has been found
  bool label_done[SYMBOL_ARR_MAX] = { false };
  // get all tokens
  int line = 0;
  int line_old = 0;
  for (int i = 0; i < s_len; ++i)
  {
    // skip comments
    if (s[i] == '#' || s[i] == ';')
    { while (s[i] != '\n') { i++; } }
    // skip whitespace
    if (isspace(s[i])) 
    { 
      if (s[i] == '\n') 
      {
        line++; 
        // check if this is label line
        if (line_old != line)
        {
          is_subr = false; // reset because has to be in same line
          for (int i = 0; i < labels_len; ++i)
          {
            if (labels[i].pos == line && !label_done[i])
            {
              labels[i].pos = t_len;
              label_done[i] = true;
              if (debug) P_IL("repos | ");
              if (debug) P_SYMBOL(labels[i]);
            }
          }
        }
        continue; 
      } 
    } 

    // ---- number ----
    buf_pos = 0;
    while (isdigit(s[i]))
    {
      buf[buf_pos] = s[i];
      buf_pos++;
      i++;
    }
    buf[buf_pos] = '\0';
    if (buf_pos > 0)
    {
      int n = strtol(buf, NULL, 10);
      ADD(TOKEN_NUM, n);
      i--; continue; // i--, because already at last char of num
    }
    // char, i.e. 'a'
    if (s[i] == '\'' && s[i +2] == '\'') 
    {
      ADD(TOKEN_NUM, (int)s[i +1]);
      i += 2; continue;
    }

    // ---- instructions ----
    if ((s[i +0] == 'm' || s[i +0] == 'M') &&
        (s[i +1] == 'o' || s[i +1] == 'O') &&
        (s[i +2] == 'v' || s[i +2] == 'V'))
    { ADD_T(TOKEN_MOV); i += 2; continue; }
    if ((s[i +0] == 'a' || s[i +0] == 'A') &&
        (s[i +1] == 'd' || s[i +1] == 'D') &&
        (s[i +2] == 'd' || s[i +2] == 'D'))
    { ADD_T(TOKEN_ADD); i += 2; continue; }
    if ((s[i +0] == 's' || s[i +0] == 'S') &&
        (s[i +1] == 'u' || s[i +1] == 'U') &&
        (s[i +2] == 'b' || s[i +2] == 'B'))
    { ADD_T(TOKEN_SUB); i += 2; continue; }
    if ((s[i +0] == 'j' || s[i +0] == 'J') &&
        (s[i +1] == 'm' || s[i +1] == 'M') &&
        (s[i +2] == 'p' || s[i +2] == 'P'))
    { ADD_T(TOKEN_JMP); i += 2; continue; }
    if ((s[i +0] == 'j' || s[i +0] == 'J') &&
        (s[i +1] == 'm' || s[i +1] == 'M') &&
        (s[i +2] == 'z' || s[i +2] == 'Z'))
    { ADD_T(TOKEN_JMZ); i += 2; continue; }
    if ((s[i +0] == 'j' || s[i +0] == 'J') &&
        (s[i +1] == 'n' || s[i +1] == 'N') &&
        (s[i +2] == 'z' || s[i +2] == 'Z'))
    { ADD_T(TOKEN_JNZ); i += 2; continue; }
    if ((s[i +0] == 'j' || s[i +0] == 'J') &&
        (s[i +1] == 's' || s[i +1] == 'S') &&
        (s[i +2] == 'r' || s[i +2] == 'R'))
    { ADD_T(TOKEN_JSR); i += 2; continue; } 
    if ((s[i +0] == 'r' || s[i +0] == 'R') &&
        (s[i +1] == 't' || s[i +1] == 'T') &&
        (s[i +2] == 's' || s[i +2] == 'S'))
    { ADD_T(TOKEN_RTS); i += 2; continue; }
    if ((s[i +0] == 'e' || s[i +0] == 'E') &&
        (s[i +1] == 'x' || s[i +1] == 'X') &&
        (s[i +2] == 't' || s[i +2] == 'T'))
    { ADD_T(TOKEN_EXT); i += 2; continue; }
    if ((s[i +0] == 'p' || s[i +0] == 'P') &&
        (s[i +1] == 's' || s[i +1] == 'S') &&
        (s[i +2] == 'h' || s[i +2] == 'H'))
    { ADD_T(TOKEN_PSH); i += 2; continue; }
    if ((s[i +0] == 'p' || s[i +0] == 'P') &&
        (s[i +1] == 'o' || s[i +1] == 'O') &&
        (s[i +2] == 'p' || s[i +2] == 'P'))
    { ADD_T(TOKEN_POP); i += 2; continue; }
    if ((s[i +0] == 'p' || s[i +0] == 'P') &&
        (s[i +1] == 'u' || s[i +1] == 'U') &&
        (s[i +2] == 't' || s[i +2] == 'T'))
    { ADD_T(TOKEN_PUT); i += 2; continue; }
    if ((s[i +0] == 'g' || s[i +0] == 'G') &&
        (s[i +1] == 'e' || s[i +1] == 'E') &&
        (s[i +2] == 't' || s[i +2] == 'T'))
    { ADD_T(TOKEN_GET); i += 2; continue; }
    if ((s[i +0] == 'o' || s[i +0] == 'O') &&
        (s[i +1] == 'u' || s[i +1] == 'U') &&
        (s[i +2] == 't' || s[i +2] == 'T'))
    { ADD_T(TOKEN_OUT); i += 2; continue; }
    if ((s[i +0] == 'm' || s[i +0] == 'M') &&
        (s[i +1] == 'u' || s[i +1] == 'U') &&
        (s[i +2] == 'l' || s[i +2] == 'L'))
    { ADD_T(TOKEN_MUL); i += 2; continue; }
    if ((s[i +0] == 'd' || s[i +0] == 'D') &&
        (s[i +1] == 'i' || s[i +1] == 'I') &&
        (s[i +2] == 'v' || s[i +2] == 'V'))
    { ADD_T(TOKEN_DIV); i += 2; continue; }
    if ((s[i +0] == 'm' || s[i +0] == 'M') &&
        (s[i +1] == 'o' || s[i +1] == 'O') &&
        (s[i +2] == 'd' || s[i +2] == 'D'))
    { ADD_T(TOKEN_MOD); i += 2; continue; }
    // ---- registers ----
    if (isspace(s[i +1])) // check to not catch label / etc.
    {
      if      (s[i] == 'a' || s[i] == 'A') { ADD_T(TOKEN_REG_A); continue; } 
      else if (s[i] == 'b' || s[i] == 'B') { ADD_T(TOKEN_REG_B); continue; } 
      else if (s[i] == 'x' || s[i] == 'X') { ADD_T(TOKEN_REG_X); continue; } 
      else if (s[i] == 'y' || s[i] == 'Y') { ADD_T(TOKEN_REG_Y); continue; }
    }

    // ---- labels ----
    if (s[i] == '-' && s[i +1] == '>') { is_subr = true; i += 1; continue; }
    
    buf[0] = '\0'; // removing the prev str
    buf_pos = 0;
    while (isalnum(s[i]) || s[i] == '_' || s[i] == ':')
    {
      buf[buf_pos] = s[i];
      buf_pos++;
      buf[buf_pos] = '\0';
      if (s[i] == ':') break; // breaking before i++ so the loop can just continue
      i++;
    }
    // subroutine start
    if (buf_pos > 0 && buf[buf_pos -1] == ':' && is_subr)
    {
      buf[buf_pos -1] = '\0'; // removing the ':'
      ADD(TOKEN_SRT, i);
      is_subr = false;
      // i--; 
      continue; // i--, because already at last char of num
    }
    // label reference
    else if (buf_pos > 0)
    {
      for (int i = 0; i < labels_len; ++i)
      {
        if (!strcmp(labels[i].name, buf))
        {
          ADD(TOKEN_SYMBOL, i);  // i is label idx
          if (debug) PF("S ref: %s, lne: %d, sym: %d\n", buf, line, i);
          continue; // i--, because already at last char of num
        }
      }
    } 
    
    // ---- strings ----
    buf[0] = '\0'; // removing the prev str
    buf_pos = 0;
    if (s[i] == '\"')
    {
      i++; // skip first "
      while (s[i] != '\"')
      {
        buf[buf_pos] = s[i];
        buf_pos++;
        buf[buf_pos] = '\0';
        i++;
      }
      i++; // skip last "
    }
    if (buf_pos > 0)
    {
      int idx = str_push(buf);
      ADD(TOKEN_STR, idx);
      // i--; 
      continue; // i--, because already at last char of num
    }

  }

  *rtn_len = t_len; 
}


int get_label_pos(int idx)
{
  if (idx < 0 || idx > labels_len) { return -1; }

  return labels[idx].pos;
}

int get_label_pos_name(const char* name)
{
  for (int i = 0; i < labels_len; ++i)
  {
    if (!strcmp(labels[i].name, name))
    {
      return labels[i].pos;
    }
  }

  return -1;
}

