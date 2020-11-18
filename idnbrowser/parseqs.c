
#include "parseqs.h"
#include <string.h>

#define OPTION_SEPARATOR  '&' 
#define VALUE_SEPARATOR   '='
 
U_CAPI int U_EXPORT2
parseQueryString(const char* source,int32_t srcLen, int32_t optionCount, QueryOption options[]){
    int i=0, remaining=0, oldi=0;
    char* src = (char*) source;

    while(i<srcLen){
        /* process an option */
        QueryOption *option=NULL;
        /* search for the option string */
        int j;
        
        if(src[i] != VALUE_SEPARATOR && src[i] != OPTION_SEPARATOR){
            i++;
            continue;
        }
        for(j=0; j<optionCount; ++j) {
            if(strncmp(src+oldi, options[j].name,(i-oldi))==0) {
                oldi = i;
                option=options+j;
                break;
            }
        }
        if(option==NULL) {
            /* no option matches */
            return -i;
        }
        if(src[i] == VALUE_SEPARATOR){
            /* process the value */
            int32_t l=0;
            int32_t k = i+1; /* go past the value separtor */
            while(((k+l) < srcLen ) && src[k+l] != OPTION_SEPARATOR){
                l++;
            }
            option->doesOccur = true;
            option->value = src+(oldi+1);/* go past the value separtor */
            option->valueLen = l;
            oldi = i = l+k+1; /* go past the value separtor */
        }
        if(src[i] == OPTION_SEPARATOR){
            option->doesOccur = true;
            option->value = NULL;
            option->valueLen = 0;
            oldi = i;
        }

        i++;
    }
    return remaining;
}
