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

class HTMLFilter : public UnicodeFilter
{
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
