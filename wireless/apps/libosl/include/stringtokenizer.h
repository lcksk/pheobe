/*
 * stringtokenizer.h
 *
 *  Created on: Apr 27, 2011
 *      Author: buttonfly
 */
#ifndef STRINGTOKENIZER_H_
#define STRINGTOKENIZER_H_

/* ---------------------------------------------
 * @author: smkim, buttonfly@gmail.com *
 * release date: 2008.4.14
 * --------------------------------------------*/


#ifdef __cplusplus
extern "C"
{
#endif

typedef void* stringtokenizer;

/**
 * constructs an stringtokenzier with the specified buffer
 * @param buffer the pointer of the pointer which indicates an string buffer
 * @return an stringtokenizer
 */
extern stringtokenizer stringtokenizer_new(const char** buffer);

/**
 * find the next token in this stringtokenizer's string
 * @param stringtokenizer this stringtokenizer
 * @param delims delimiters
 * @param numofdelims the number of delims
 * @param dst the buffer which will contain the next token
 * @return if failed to get the next token, it returns -1
 */
extern int stringtokenizier_next_token(stringtokenizer stringtokenizer, const char* delims, int numofdelims, char* dst);

/**
 * close this stringtokenizer
 * @param stringtokenizer this stringtokenizer
 */
extern void stringtokenizer_delete(stringtokenizer stringtokenizer);

#ifdef __cplusplus
}
#endif

#endif /*STRINGTOKENIZER_H_*/
