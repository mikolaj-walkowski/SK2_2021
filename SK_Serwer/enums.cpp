#include "enums.h"
#include <string.h>
#include <stdarg.h>


int startsWith(const char* s, const char* search){
    uint i = 0;
    while (search[i] != '\0')
    {
        if(s[i] == '\0'||s[i] != search[i]){
            return 0;
        }
        ++i;
    }
    return 1;
}

protocol_keywords resolve_pkw(const char* s)
{
    for(int i = 0; i < sizeof(PKW_converter)/sizeof(PKW_converter[0]); ++i){
        if(startsWith(s,PKW_converter[i].key)){
            return PKW_converter[i].val;
        }
    }
    return bad;
}

/**
 * @brief Create a Buffer long enough to contain combined cstrings. Will cause memory leak on target.
 * 
 * 
 * 
 * @param target target char pointer
 * @param size int that will contain total length OR null
 * @param paramN number of strings
 * @param ... c_strings
 */
char* createBuffer(int * size, int paramN,...){
    va_list ptr;
    char* target;
    int isize = 1;
    va_start(ptr,paramN);
    int i;
    for(i = 0 ; i < paramN ; ++i){
        char* arg = va_arg(ptr,char*);
        isize += strlen(arg);
    }
    target = (char*)malloc(sizeof(char)* isize);
    if(size != NULL){
        *size = isize;
    }
    return target; 
}