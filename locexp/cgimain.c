/* Copyright (c) 2000 IBM, Inc. and others, all rights reserved */

/* Run Locale Explorer as a CGI. */
#include "locexp.h"

#ifdef WIN32
#   define _WIN32_WINNT 0x0400 
#   include "windows.h"
#endif

int main(const char *argv[], int argc);

int main(const char *argv[], int argc)
{
    
  LXContext  localContext;

/* on win32 systems to debug uncomment the block below
 * Invoke the CGI application. 
 * Attach a debugger (such as Visual C) to the CGI process while a message box is on the screen. 
 * When the debugger is attached, open the source file and set a break point. 
 * Click OK to dismiss the message box. When the message box is dismissed, 
 * the CGI execution will resume and the break point will be hit. 
 */
#if 0
#   ifdef WIN32
  
    char szMessage [256];
    
    wsprintf (szMessage, "Please attach a debbuger to the process 0x%X (%s) and click OK",
              GetCurrentProcessId(),"locexp");
    MessageBox(NULL, szMessage, "CGI Debug Time!",
               MB_OK|MB_SERVICE_NOTIFICATION);
    
#   endif
#endif
  initLX();
  initContext(&localContext);
  localContext.fOUT = stdout;
  runLocaleExplorer(&localContext);
  closeLX(&localContext);
  return 0;
}
