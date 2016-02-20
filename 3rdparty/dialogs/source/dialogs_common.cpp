/*
  Native File Dialog

  http://www.frogtoss.com/labs
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "internal/dialogs_common.hpp"

static char g_errorstr[NFD_MAX_STRLEN] = {0};

/* public routines */

void NFDi_SetError( const char *msg )
{
    int bTruncate = NFDi_SafeStrncpy( g_errorstr, msg, NFD_MAX_STRLEN );
    assert( !bTruncate );  _NFD_UNUSED(bTruncate);
}


int NFDi_SafeStrncpy( char *dst, const char *src, size_t maxCopy )
{
    size_t n = maxCopy;
    char *d = dst;

    assert( src );
    assert( dst );
    
    while ( n > 0 && *src != '\0' )    
    {
        *d++ = *src++;
        --n;
    }

    /* Truncation case -
       terminate string and return true */
    if ( n == 0 )
    {
        dst[maxCopy-1] = '\0';
        return 1;
    }

    /* No truncation.  Append a single NULL and return. */
    *d = '\0';
    return 0;
}


/* adapted from microutf8 */
size_t NFDi_UTF8_Strlen( const char *str )
{
	/* This function doesn't properly check validity of UTF-8 character 
	sequence, it is supposed to use only with valid UTF-8 strings. */
    
	size_t character_count = 0;
	size_t i = 0; /* Counter used to iterate over string. */
	char maybe_bom[4];
	
	/* If there is UTF-8 BOM ignore it. */
	if (strlen(str) > 2)
	{
		strncpy(maybe_bom, str, 3);
		maybe_bom[3] = 0;
		if (strcmp(maybe_bom, (char*)NFD_UTF8_BOM) == 0)
			i += 3;
	}
	
	while(str[i])
	{
		if (str[i] >> 7 == 0)
        {
            /* If bit pattern begins with 0 we have ascii character. */ 
			++character_count;
        }
		else if (str[i] >> 6 == 3)
        {
		/* If bit pattern begins with 11 it is beginning of UTF-8 byte sequence. */
			++character_count;
        }
		else if (str[i] >> 6 == 2)
			;		/* If bit pattern begins with 10 it is middle of utf-8 byte sequence. */
		else
        {
            /* In any other case this is not valid UTF-8. */
			return -1;
        }
		++i;
	}

	return character_count;	
}

int NFDi_IsFilterSegmentChar( char ch )
{
    return (ch==','||ch==';'||ch=='\0');
}

