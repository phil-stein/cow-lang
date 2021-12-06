#include "include/global.h"
#include "include/token.h"
#include "include/interpreter.h"
#include "include/file_io.h"

#include <direct.h>



// ----- state ----
bool debug    = false;    // shows debug messages
bool show_src = false;    // shows source code before running
bool only_tok = false;    // doesnt run, only shows tokens

// string register
// [string idx][chars of string]
#define STR_REG_MAX 32
#define STR_REG_STR_MAX 32
char str_reg[STR_REG_MAX][STR_REG_STR_MAX];
int  str_reg_pos = 0; // not len because u can put items in there at specific locations


int main(int argv, char* argc[])
{
	if(argv < 2)
	{
		printf("[ERROR] have to give the name of the file to be compiled.");
		return 1;
	}
	
	if (!check_file_exists(argc[1]))
	{
		printf("[ERROR] given file does not exist.");
		return 1;
	}

  if (argv > 2)
  {
    for (int i = 0; i < argv; ++i)
    {
      if (!strcmp(argc[i], "-dbg")) { P("OPTION | DEBUG"); debug = true; }
      if (!strcmp(argc[i], "-tok")) { P("OPTION | ONLY TOKENS"); only_tok = true; }
      if (!strcmp(argc[i], "-src")) { P("OPTION | SOURCE"); show_src = true; }
    }
  }

	if (debug) printf("argc[1]: %s\n", argc[1]);

	// ---- source code ---- 
	int src_len = 0;
	// char* source = read_text_file_len(path, &src_len);
	char* source = read_text_file(argc[1]);
  src_len = strlen(source);
  if (show_src)
  {
    printf("source ------------------------------------------------\n");
    printf("%s\n", source);
    printf("-------------------------------------------------------\n");
  }


  if (debug) P(" ---- tokens ----");
  
  tok_arr t;
  int t_len = 0;
  // tokenize_text(src, strlen(src), t, &t_len);
  tokenize_text(source, src_len, t, &t_len);
  free(source);

  // ---- debug ----

  for (int i = 0; i < t_len * (debug || only_tok); ++i) // debug: true 1, no change; false 0, no loop
  {
    printf("%.2d | ", i); P_TOK( t[i] );
  }
  
  // ---------------

  if (only_tok) { exit(1); }

  if (debug) P(" ---- interpret ----");

  interpret_tokens(t, t_len);

}

bool is_debug()    { return debug; }
bool is_show_src() { return show_src; }
bool is_only_tok() { return only_tok; }

int str_push(const char* str)
{
  assert(str_reg_pos < STR_REG_MAX -1);
  strcpy(str_reg[str_reg_pos], str);
  P_STR(str_reg[str_reg_pos]);
  return str_reg_pos++; // inc after
}
void str_put(int idx, const char* str)
{
  assert(idx < STR_REG_MAX -1); 
  strcpy(str_reg[idx], str);
}
const char* str_last()
{
  return str_reg[str_reg_pos -1];
}
const char* str_pop()
{
  return str_reg[--str_reg_pos];
}
const char* str_get(int idx)
{
  assert(idx < STR_REG_MAX -1); 
  return str_reg[idx];
}

