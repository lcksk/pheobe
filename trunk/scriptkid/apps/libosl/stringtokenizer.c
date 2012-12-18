/*
 * stringtokenizer.c
 *
 *  Created on: Apr 27, 2011
 *      Author: buttonfly
 */

#include "stringtokenizer.h"
#include <string.h>
#include <stdlib.h>

/* ---------------------------------------------
 * @author: smkim, buttonfly@gmail.com *
 * release date: 2008.4.14
 * --------------------------------------------*/


typedef struct
{
    char**     buffer;
    int        offset; // absolute offset

} stringtokenizer_t;

stringtokenizer stringtokenizer_new(const char** buffer)
{
    stringtokenizer_t* t;
    t = (stringtokenizer_t*) malloc(sizeof(stringtokenizer_t));
    t->buffer = (char**)buffer;
    t->offset = 0;
    return t;
}


// return relative offset
static int stringtokenizer_seek_next_token(stringtokenizer tokenizer, const char* delims, int numofdelims)
{
    stringtokenizer_t*    t;
    t = (stringtokenizer_t*) tokenizer;
    int rel_offset;
    rel_offset = -1;

    char* p = *(t->buffer);
    int flag;
    flag = 0;

    int i;
    char temp;
    for(; (temp = (char)*p) != '\0';)
    {
        for(i = 0; i < numofdelims; i++)
        {
//            if(temp == delims[i] || temp == delims[i])
            if(flag == 0)
            {
                rel_offset = 0;
                flag = 1;
            }
            if(temp == delims[i])
            {
                return rel_offset;
            }
        }
        rel_offset++;
        p++;
    }
    return rel_offset;
//    return -1;
}


int stringtokenizier_next_token(stringtokenizer tokenizer, const char* delims, int numofdelims, char* dst)
{
    stringtokenizer_t* t;
    t = (stringtokenizer_t*) tokenizer;

    int offset;
    offset = stringtokenizer_seek_next_token(tokenizer, delims, numofdelims);
//    if(offset == 0)
    if(offset == -1)
    {
        return -1;
    }
    memcpy(dst, *(t->buffer), offset);
    dst[offset] = '\0';
    (*(t->buffer)) += (offset + 1);
    t->offset += (offset + 1);
    return offset;
}


void stringtokenizer_delete(stringtokenizer tokenizer)
{
    free(tokenizer);
}
