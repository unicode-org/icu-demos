/**********************************************************************
*   Copyright (C) 2002, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/
#include "unicode/translit.h"
#include "unicode/utrans.h"
#include "unicode/unifilt.h"
#include "unicode/uchar.h"
#include "unicode/uniset.h"
#include "unicode/lx_utils.h"
#include "unicode/caniter.h"

class HTMLFilter : public UnicodeFilter
{
  /* post 2.2: - stuff we have to add */
private:
  UnicodeString& toPattern(UnicodeString&, UBool)const { return *(new UnicodeString("Look, Mom, I'm on TV!")); }
  signed char matchesIndexValue(uint8_t m)const { return m<<3;}
  void addMatchSetTo(UnicodeSet&) const {}

public:
	HTMLFilter() :fState(0) {}
	virtual ~HTMLFilter() {}
    	virtual UBool contains(UChar32 c) const
	{
#if 0
#define YELL	fprintf(stderr, ":%d Char '%c' (U+%04X) state %d\n", __LINE__, (char)c, c, fState)
#else
#define YELL
#endif

		YELL;
	
		if(c == '<')
	 	{
			((HTMLFilter*)this)->fState = 1;
			YELL;
			return FALSE;
		}
		else if(c == '>')
		{
			((HTMLFilter*)this)->fState = 0;
			YELL;
			return FALSE;
		}
		else if(fState == 0)
		{
			if(c == '&')
			{
				((HTMLFilter*)this)->fState = 2;
				YELL;
				return FALSE;
			}

			return 	TRUE;
		}
		else
		{
			if(fState == 2)
			{
				if(c == '&')
				{
					((HTMLFilter*)this)->fState = 2;
					YELL;
					return FALSE;
				}
				if(c == ';')
				{
					((HTMLFilter*)this)->fState = 0;
					YELL;
					return FALSE;
				}
				else if(u_isalnum(c) || (c=='#'))
				{
					return FALSE;
				}
				else
				{
					((HTMLFilter*)this)->fState = 0;
					YELL;
					return TRUE;
				}
			}
			return FALSE;
		}
	}

    virtual UnicodeFunctor* clone() const  { return new HTMLFilter(*this); }

private:
	int fState;

};

U_CAPI
void lx_setHTMLFilterOnTransliterator(UTransliterator *xlit, UBool doFilter)
{

	if(doFilter == FALSE)
	{
		((Transliterator*)xlit)->adoptFilter(NULL);
	}
	else
	{
		((Transliterator*)xlit)->adoptFilter(new HTMLFilter());
	}
}

/** UUSet - hopefully this won't last long enough to deserve its own 
    file 
 **/

U_CAPI UUSet* U_EXPORT2 
uuset_openPattern(const UChar* pattern, UErrorCode* status)
{
  UUSet* uset;
  if(U_FAILURE(*status))
  {
    return NULL;
  }

  uset = (UUSet*) new UnicodeSet(UnicodeString(pattern), *status);

  return uset;
}

U_CAPI int32_t U_EXPORT2 
uuset_size(UUSet* _this)
{
  return ((UnicodeSet*)_this)->size();
}

U_CAPI UChar32 U_EXPORT2 
uuset_charAt(UUSet* _this, int32_t index)
{
  return ((UnicodeSet*)_this)->charAt(index);
}

U_CAPI UBool U_EXPORT2 
uuset_contains(UUSet* _this, UChar32 c)
{
  return ((UnicodeSet*)_this)->contains(c);
}

U_CAPI void U_EXPORT2
uuset_close(UUSet* _this)
{
  delete ((UnicodeSet*)_this);
}

/************************/
U_CAPI UCanonicalIterator ucanit_open(const UChar *str, UErrorCode *status)
{
  CanonicalIterator *iter;

  if(!status || U_FAILURE(*status)) return NULL;
  
  iter = new CanonicalIterator(UnicodeString(str), *status);

  if(!iter || U_FAILURE(*status)) {
    delete iter;
    return NULL;
  }

  return iter;
}

/**
 * see UCanonicalIterator::next
 * @param iter the iterator
 * @param buffer the output buffer. Alwyas null ternimated.
 * @param len Length of buffer
 * @param status Error code.  U_BUFFER_OVERFLOW_ERROR if buffer is full.
 * @return actual size of string.  0 if done. 
 */
U_CAPI int32_t ucanit_next(UCanonicalIterator *iter, UChar *buffer, int32_t len, UErrorCode *status)
{
  UnicodeString res;
  int32_t reslen;

  if(!iter || !status || U_FAILURE(*status)) {
    return -1;
  }

  res = ((CanonicalIterator*)iter)->next();

  if(res.isBogus() || !res.length()) {
    return 0;
  }

  if(buffer) {
    reslen = res.extract(buffer, res.length(), *status);
    buffer[reslen]=0;
    if(len) {
      buffer[len-1]=0;
    }
  }

  return res.length();
}

U_CAPI void ucanit_close(UCanonicalIterator *iter)
{
  delete ((CanonicalIterator*)iter);
}
