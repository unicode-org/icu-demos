#ifndef __UNAMES_H__
#define __UNAMES_H__

/* public API --------------------------------------------------------------- */

typedef enum UCharNameChoice {
    U_UNICODE_CHAR_NAME,
    U_UNICODE_10_CHAR_NAME,
    UCharNameChoiceCount
} UCharNameChoice;

U_CAPI UTextOffset U_EXPORT2
u_charName(uint32_t code, UCharNameChoice nameChoice,
           char *buffer, UTextOffset bufferLength,
           UErrorCode *pErrorCode);

#endif
