/* Copyright (c) 2003 IBM. and others, all rights reserved */

/* Run Locale Explorer as a CGI. */
#include "locexp.h"

#ifdef WIN32
#   define _WIN32_WINNT 0x0400 
#   include "windows.h"
#endif

int main(int argc, const char *argv[]);

int main(int argc , const char *argv[])
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
    
    wsprintf (szMessage, "Please attach a debugger to the process %d, 0x%X (%s) and click OK",
              GetCurrentProcessId(), GetCurrentProcessId(),"locexp");  /* msvc 6 seems to show it in decimal */
    MessageBox(NULL, szMessage, "CGI Debug Time!",
               MB_OK|MB_SERVICE_NOTIFICATION);
    
#   endif
#endif

#ifdef WIN32
    if( setmode( fileno ( stdout ), O_BINARY ) == -1 ) {
        perror ( "Cannot set stdout to binary mode" );
        exit(-1);
    }
#endif


  initLX();
  initContext(&localContext);
  localContext.fOUT = stdout;
  initCGIVariables(&localContext);
  initPOSTFromFILE(&localContext, stdin);
  runLocaleExplorer(&localContext);
  closeCGIVariables(&localContext);
  closePOSTFromFILE(&localContext);
  closeLX(&localContext);
  return 0;
}
