
/* #include a bunch of stuff before this file... */

extern UCNV_FromUCallBack DECOMPOSE_lastResortCallback;

CAPI void 
  MissingUnicodeAction_DECOMPOSE (UConverter * _this,
					    char **target,
					    const char *targetLimit,
					    const UChar ** source,
					    const UChar * sourceLimit,
					    int32_t *offsets,
					    bool_t flush,
					    UErrorCode * err);

/* NOTE: returns USING_FALLBACK_ERROR instead of ZERO_ERROR if 
   decomposition occurs. (does NOT occur in INDEX_OUTOFBOUNDS cases) */
