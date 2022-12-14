//
// Created by Wei Zhang on 7/12/17.
//

#ifndef PDC_STRING_UTILS_H
#define PDC_STRING_UTILS_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "hash_utils.h"
#include "cbase/base_stdlib.h"
#include "cbase/c99_stdlib.h"

// #define PATTERN_EXACT  0
// #define PATTERN_SUFFIX 1
// #define PATTERN_PREFIX 2
// #define PATTERN_MIDDLE 3

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"


typedef enum {
    PATTERN_EXACT   = 2 ,
    PATTERN_PREFIX  = 3 ,
    PATTERN_SUFFIX  = 4 ,
    PATTERN_MIDDLE  = 5
} pattern_type_t;





typedef struct{
    const char *body;
    size_t length;
    pattern_type_t type;
    void *user;
} affix_t;

typedef struct {
    char *start;
    size_t length;
} string;


extern int asprintf(char **strp, const char *fmt, ...);
extern int vasprintf(char **strp, const char *fmt, va_list ap);

// /**
//  * Memory allocation with counter.
//  * allocate 'size' bytes of memory and return a generic pointer.
//  */
// void *ctr_malloc(size_t size, size_t *reg);

// /**
//  * Memory allocation with counter. (Clear allocation)
//  * allocate nitems * size bytes of memory and return a generic pointer.
//  */
// void *ctr_calloc(size_t nitems, size_t size, size_t *reg);

/**
 * take the part after start position
 * you need to free str after use.
 * @param str
 * @param start
 * @return
 */
char *substr(const char* str, int start);
/**
 * take the part before end position
 * you need to free str after use.
 * @param str
 * @param end
 * @return
 */
char *subrstr(const char* str, int end);
/**
 * take the substring starting from start, ending at end-1
 * you need to free str after use.
 * @param str
 * @param start
 * @param end
 * @return
 */
char *substring(const char* str, int start, int end);
/**
 * determine the pattern type.
 * Currently, only support for different types:
 *
 * PREFIX : ab*
 * SUFFIX : *ab
 * MIDDLE : *ab*
 * EXACT  : ab
 *
 * @param pattern
 * @return
 */
pattern_type_t determine_pattern_type(const char *pattern);
/**
 * return the index of token tok in given string str.
 * if not found, return -1
 *
 * @param str
 * @param tok
 * @return
 */
int indexOfStr(const char* str, char *tok);
/**
 * return the index of character c in given string str.
 * if not found, return -1
 *
 * @param str
 * @param c
 * @return
 */
int indexOf(const char* str, char c);

/**
 * to determine if a string str starts with prefix pre
 * @param pre
 * @param str
 * @return
 */
int startsWith(const char *str, const char *pre);
/**
 * to determine if a string str ends with suffix suf
 * @param suf
 * @param str
 * @return
 */
int endsWith(const char *str, const char *suf);
/**
 * to determine if a string str contains token tok
 * @param tok
 * @param str
 * @return
 */
int contains(const char *str, const char *tok);
/**
 * to determine if a string str exactly matches a token tok.
 * @param tok
 * @param str
 * @return
 */
int equals(const char *str, const char *tok);


/**
 * Print anything on stdout.
 * Always put a line feed after the string you want to print.
 *
 * @param format
 * @param ...
 */
void println(const char *format, ...);

/**
 * Print anything on stderr.
 * Always put a line feed after the string you want to print.
 *
 * @param format
 * @param ...
 */
void eprintln(const char *format, ...);
/**
 * Only support expressions like:
 *
 * *ab
 * ab*
 * *ab*
 * ab
 * 
 *
 * @param str
 * @param pattern
 * @return 1 if matched and 0 if not. 
 */
int simple_matches(const char *str, const char *pattern);

/**
 * To test if str is matching with the given affix
 * return 1 if matched, return 0 if not. 
 * 
 */
int is_matching_given_affix(const char *str, affix_t *affix_info);

/**
 * get the reverse of a given string. 
 * a new reversed string of the original will be generated. 
 */
char *reverse_str(char *str);

/**
 * Append string to the string staring with *result_ptr. 
 * 
 * After this operation, the address of result_ptr[0][0] will be changed. 
 * But the string starting at result_ptr[0][0] is still the same. 
 */
int str_append(char **result_ptr, const char *format, ...);

/**
 * Concat one or more strings with str.
 */
char *
concat (const char *str, ...);

/**
 * Generating a series of UUIDs using libuuid
 */
char **gen_uuids_strings(int count);

/**
 * Generating a series of random strings
 */ 
char **gen_random_strings(int count, int maxlen, int alphabet_size);

/**
 *  Generating a series of random string of specified length.
 */
char **gen_rand_strings(int count, int maxlen);

/**
 * Read text file and store each line of the text file as a string
 */
char **read_words_from_text(const char *fileName, int *word_count);

/**
 * get affix info
 */
affix_t *create_affix_info(const char *body, size_t len, pattern_type_t affix_type, void *user);

#endif //PDC_STRING_UTILS_H
