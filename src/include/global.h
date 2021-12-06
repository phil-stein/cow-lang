#ifndef BEE_GLOBAL_H
#define BEE_GLOABL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

// null
#ifndef NULL
	#define NULL (void*)0
#endif

// bool
#ifndef bool
	#define bool int
#endif
#ifndef true
	#define true 1
#endif
#ifndef false
	#define false 0
#endif

#define INLINE static inline __attribute( (always_inline) )

// print

#define P_INT(v) 	  printf("%s: %d\n", #v, v)
#define P_F32(v) 	  printf("%s: %f\n", #v, v)
#define P_STR(v) 	  printf("%s: %s\n", #v, v)
#define P_BOOL(v) 	printf("%s: %s\n", #v, (v) ? "true" : "false")

#define P(msg)		  printf("%s\n", msg)
#define P_IL(msg)	  printf("%s", msg)
#define PF(...)		  printf(__VA_ARGS__)
#define P_TXT(txt)  printf("%s:\n%s\n", #txt, txt)
#define P_ERR(msg)	printf("[ERROR] %s\n", msg)

bool is_debug();   
bool is_show_src();
bool is_only_tok();


int str_push(const char* str);
void str_put(int idx, const char* str);
const char* str_last();
const char* str_pop();
const char* str_get(int idx);

#endif
