/* Copyright (c) 2010-2012 IBM Corporation and Others. All Rights Reserved. */

/*
  Note: for help, run 'ucd h'
*/

#include "ucd.h"
#include <unicode/ustdio.h>
#include <unicode/utypes.h>
#include <unicode/putil.h>
#include <unicode/ustring.h>
#include <unicode/uchar.h>
#include <unicode/uset.h>
#include <unicode/uclean.h>

#include <stdlib.h>
#include <string.h>

#if defined U_WINDOWS
#include "windows.h"
#endif

#include <stdlib.h>
#include <string.h>


/**
 * Globals
 */
int VERBOSE     = 0; /* -v  verbose mode */
int PARSERC     = 1; /* -n INHIBITS parsing of .ucdrc */
int INTERACTIVE = 0; /* -i - interactive even if other commands */
int DONE        = 0; /* are we there yet? */
int commands_processed = 0; /* total commands (inc. push) processed */
int printed = 0; /* did we print the workbuffer? */

UFILE *ustdout = NULL;
UFILE *ustderr = NULL;
UFILE *ustdin  = NULL;

UConverter *def = NULL;

int32_t workmax = 0;
UChar *workbuffer = 0;
int32_t worklen = 0;

void realloc_work(int32_t len) {
  if(len > workmax) {
    workbuffer = (UChar*)realloc(workbuffer,len*sizeof(workbuffer[0]));
    workmax = len;
  }
}
/**
 * If there are line endings at the end, remove them.
 */
void striplf(UChar *s)
{
  int32_t l = u_strlen(s);
  if((s[l-1]==UCD_CMD_CR) || (s[l-1]==UCD_CMD_LF)) {
    l--;
    if((s[l-1]==UCD_CMD_CR) || (s[l-1]==UCD_CMD_LF)) {
      l--;
    }
    s[l]=0;
  }
}

/**
 * Show the prompt 
 */
void prompt(void) {
  if(worklen>0) {
    u_fprintf(ustdout, "ucd %d> ", worklen);
  } else {
    u_fprintf(ustdout, "ucd> ");
  }
  u_fflush(ustdout);
}

/**
 * Parse a file (including ustdin)
 */
void ucd_parseFile(UFILE *f, const char *fileName, UErrorCode &status)
{
  int line = 0;
  UChar linebuf[2048];
  UBool isStdin = (f==ustdin);
  UBool ignoredErr = FALSE;
  if(!DONE&&U_SUCCESS(status)&&isStdin) {
    prompt();
  }
  while(U_SUCCESS(status)&&!DONE&&u_fgets(linebuf,2048,f)) {
    line++;
    if(VERBOSE) u_fprintf(ustderr, "%s:%d: ", fileName, line);
    striplf(linebuf);
    ucd_parseCommand(linebuf, status);
    if(isStdin&&!DONE) { /* special handling for stdin (interactive) */
      if(U_FAILURE(status)) {
        if(!ignoredErr) {
          u_fprintf(ustderr, "Note: use the 'h' command for help.\n");
          ignoredErr=TRUE;
        }
        status = U_ZERO_ERROR; /* we're in interactive mode - swallow it. */
      }
      /* auto print */
      if(!printed && (worklen>0)) {
        if(worklen < 100) {
          ucd_cmd_info(workbuffer, worklen, status); // show info
        } else {
          u_fprintf(ustdout, "(Result: %d chars. Use '%C' to print all.)\n", worklen, UCD_CMD_I);
        }
        set_printed();
      }
      prompt();
    }
  }
}

void ucd_parseFile(const char *fileName, UErrorCode &status)
{
  UFILE *f;
  f = u_fopen(fileName, "r", NULL, NULL);

  if(f==NULL) {
    status = U_FILE_ACCESS_ERROR;
    return;
  }

  ucd_parseFile(f, fileName, status);

  u_fclose(f);
}


/** Blocks & Property names **/

const char *getUCharCategoryName(int8_t type)
{
    const char *f;

    f = u_getPropertyValueName(UCHAR_GENERAL_CATEGORY,
                                  type,
                                  U_LONG_PROPERTY_NAME);

    if (f == NULL) {
        char *c;
        c = (char*)malloc(200);
        sprintf(c,"unknown%d",(int)type);
        f = c;
    }

    return f;
}

const char *getUBlockCodeName(UBlockCode block)
{
    const char *f;

    f = u_getPropertyValueName(UCHAR_BLOCK,
                               block,
                               U_LONG_PROPERTY_NAME);

    if (f == NULL) {
        char *c;
        c = (char*)malloc(200);
        sprintf(c,"unknown%d",(int)block);
        f = c;
    }

    return f;
}

const char *getUPropertyName(UProperty u)
{
    return u_getPropertyName(u,
                             U_LONG_PROPERTY_NAME);
}

int maxlen =0;

void ucd_cmd_info_long(const UChar *buf, int32_t len, UErrorCode &status)
{
  int32_t i = 0;
  UChar32 ch;
  
  if(i>=len) return;

  U16_NEXT(buf,i,len,ch);

  if(maxlen ==0) {
    for(i=UCHAR_BINARY_START;i<UCHAR_STRING_LIMIT;i++) {
      const char *n = getUPropertyName((UProperty)i);
      if(n!=NULL&&*n) {
        int l = strlen(n);
        if(l>maxlen) maxlen = l;
      }
    }
  }

  {
    UVersionInfo va;
    char age[U_MAX_VERSION_STRING_LENGTH];
    u_charAge(ch, va);
    u_versionToString(va, age);
    u_fprintf(ustdout, "%*s: %10s\n",
              maxlen, "Derived Age", age);
  }
  
  for(i=UCHAR_BINARY_START;i<UCHAR_BINARY_LIMIT;i++) {
#if 0
    UBool has;
    has = u_hasBinaryProperty(ch, i);
    
    u_fprintf(ustdout, "%d</td><td>%s</td><td>%s%s%s</td><td>%s</td><td>%s\n",
              i, "bin", has?"<b>":"", getUPropertyName(i), has?"</b>":"",  has?"T":"f", u_getPropertyValueName(i,u_getIntPropertyValue(ch,i),U_LONG_PROPERTY_NAME));
#endif
    int32_t has;
    has = u_getIntPropertyValue(ch, (UProperty)i);
    
    u_fprintf(ustdout, "%*s: %10ld (%ld..%ld)\t%s\n",
              /*  i, "bin", */ maxlen, getUPropertyName((UProperty)i), has,
              u_getIntPropertyMinValue((UProperty)i),
              u_getIntPropertyMaxValue((UProperty)i) ,u_getPropertyValueName((UProperty)i,u_getIntPropertyValue(ch,(UProperty)i),U_LONG_PROPERTY_NAME));
    
  }
  
  for(i=UCHAR_INT_START;i<UCHAR_INT_LIMIT;i++) {
    int32_t has;
    has = u_getIntPropertyValue(ch, (UProperty)i);
    
    u_fprintf(ustdout, "%*s: %10ld (%ld..%ld)\t%s\n",
              /*  i, "int",*/ maxlen, getUPropertyName((UProperty)i), has,
              u_getIntPropertyMinValue((UProperty)i),
              u_getIntPropertyMaxValue((UProperty)i) ,u_getPropertyValueName((UProperty)i,u_getIntPropertyValue(ch,(UProperty)i),U_LONG_PROPERTY_NAME));
    
    
  }
  
  for(i=UCHAR_MASK_START;i<UCHAR_MASK_LIMIT;i++) {
    int32_t has;
    has = u_getIntPropertyValue(ch, (UProperty)i);
    
    u_fprintf(ustdout, "%*s: %10ld (%ld..%ld)\t%s\n",
              maxlen, getUPropertyName((UProperty)i), has,
              u_getIntPropertyMinValue((UProperty)i),
              u_getIntPropertyMaxValue((UProperty)i) ,u_getPropertyValueName((UProperty)i,u_getIntPropertyValue(ch,(UProperty)i),U_LONG_PROPERTY_NAME));
    
  }
  
#if 0
  for(i=UCHAR_DOUBLE_START;i<UCHAR_DOUBLE_LIMIT;i++) {
    double has;
    has = u_getDoublePropertyValue(ch, i);
    
    u_fprintf(ustdout, "%d:%s\t%s\t%g (%ld..%ld)\t%s\n",
              i, "int", getUPropertyName((UProperty)i), has,
              u_getIntPropertyMinValue((UProperty)i),
              u_getIntPropertyMaxValue((UProperty)i) ,u_getPropertyValueName((UProperty)i,u_getIntPropertyValue(ch,(UProperty)i),U_LONG_PROPERTY_NAME));
    
  }
  
  for(i=UCHAR_STRING_START;i<UCHAR_STRING_LIMIT;i++) {
    int32_t has;
    has = u_getIntPropertyValue(ch, i);
    
    u_fprintf(ustdout, "#%d:%s\t%s\t%ld (%ld..%ld)\t%s\n",
              i, "int", getUPropertyName((UProperty)i), has,
              u_getIntPropertyMinValue((UProperty)i),
              u_getIntPropertyMaxValue((UProperty)i) ,u_getPropertyValueName((UProperty)i,u_getIntPropertyValue(ch,(UProperty)i),U_LONG_PROPERTY_NAME));
    
  }
#endif
  /*  
  u_fprintf(ustdout, "<br /> The PN column and the PVN column refer to the internal property number and property value number. Except in the case of the Canonical_Combining_Class, these numbers have no relevance outside of ICU. ");
  */
 if(i<len) {
   u_fprintf(ustderr, "Note: only the first char is printed in long form.\n");
 }
}

typedef struct { 
  UChar32 cmd;          // command character
  const char *meaning;  // Explanation
  const char *example;  // The rest of the example command (not including the command char itself). Could be "".
} Help;

void ucd_cmd_help(UErrorCode &status)
{
  u_fprintf(ustdout, "ucd help\n-----\n");
  Help help[] = {
    { UCD_CMD_HASH, "comment",  "This is a comment" },
    { UCD_CMD_DBLQUOTE, "load string",  "abc" },
    { UCD_CMD_QUESTION, "show buffer length", "" },
    { UCD_CMD_SLASH, "search char names", "smiling" },
    { UCD_CMD_LEFTSQ, "load uniset", ":Latn:]-[aeiou]]" },
    { UCD_CMD_0, "clear buffer", "" },
    { UCD_CMD_H, "show this help", "" },
    { UCD_CMD_P, "print buffer as a string", "" },
    { UCD_CMD_U, "load a single hex codepoint", "+0127" },
    { UCD_CMD_U, "load a single dec codepoint", "#295" },
    { UCD_CMD_UU, "(same as 'u')", "+0127" },
    { UCD_CMD_UI, "show FULL info on the first codepoint in the buffer", "" },
    { UCD_CMD_I, "show name&number on every codepoint in the buffer\n(this is what happens if you don't give any other commands at the end)", "" },
    { UCD_CMD_Q, "quit ucd", "" }
  };

  for(unsigned int i=0;i<(sizeof(help)/sizeof(help[0]));i++) {
    u_fprintf(ustdout, "%C: %s\n\tExample: %C%s\n",
              help[i].cmd,
              help[i].meaning,
              help[i].cmd,
              help[i].example);
  }
}

/**
 * Show information (name) on every char in the workbuffer 
 */
void ucd_cmd_info(const UChar *buf, int32_t len, UErrorCode &status)
{
  int32_t i = 0;
  UChar32 c;
  
  while(i<len) {
    U16_NEXT(buf, i, len, c);
    
    u_fprintf(ustdout, "(U+%04X) ", c);

    char junkbuf[1024];
    u_charName(c, U_UNICODE_CHAR_NAME, junkbuf, 512, &status);
    if(U_FAILURE(status)) strcpy(junkbuf,u_errorName(status));
    u_fprintf(ustdout, "%s", junkbuf);
    u_fprintf(ustdout, "\n");
  }
}

/**
 * Mark that some output of the current workbuffer was printed
 */
void set_printed() {
  printed++;
}

/**
 * Clear the workbuffer
 */
void clear_wb(void) {
    worklen=0;
    workbuffer[0]=0;
    printed=0;
}

/**
 * If something was already printed, clear the workbuffer.
 */
void clear_if_printed() {
  if(printed && (worklen>0)) {
    clear_wb();
  }
  printed =0;
}

/**
 * For search
 */
int words_count = 0;
/**
 * For search
 */
char words[16][50];
/**
 * For search
 */
char gSearchName[655];

/**
 * Set up a ubrowser-like search.
 */
void search_setup(const UChar *buf, UErrorCode &status)
{
    const char *p, *q;
    int32_t len;

    ucnv_fromUChars(def, gSearchName, 655, buf, -1, &status);
    p = gSearchName;
    while(isspace(*p)) p++;
    for(int i=0;gSearchName[i]&&i<655;i++) {
      gSearchName[i]=toupper(gSearchName[i]);
    }
    words_count = 0;
    while(p && *p)
    {
#if defined (UB_DEBUG)
      fprintf(stderr, "[P=%s count=%d\n", p, words_count);
#endif
        q = strchr(p, ' ');
        if(q == NULL)  /* last one */
        {
          strncpy(&(words[words_count][0]), p, 49);
        }
        else
        {
            len = q - p;
            if(len>48) 
            {
                len = 48;
            }
            strncpy(words[words_count], p, len);
            q++;
        }
        words[words_count][49]=0;
#if defined (UB_DEBUG)
        fprintf(stderr, " w=%s\n", words[words_count]);
#endif
        words_count++;
        p = q;
        if(p != NULL) {
            while(isspace(*p)) p++;
        }
        if(words_count >= 16) {
            return;
        }
    }
    
}

/**
 * Callback function for ubrowse-like search.
 */
extern "C" {
UBool search_fn(void *context,
                UChar32 code,
                UCharNameChoice nameChoice,
                const char *name,
                int32_t length)
{
  UBool isFound = FALSE;
  //enumHits++;
  if(strstr(name, gSearchName)) {
    isFound = TRUE;
  } else if(words_count>1) {
    const char *k = name;
    int n;
    for(n=0;k&&(n<words_count);n++) {
      if((k=strstr(k,words[n]))) {
        const char *nextk = k+strlen(words[n]);
#if defined (UB_DEBUG)
        fprintf(stderr, "[%d] considering %s -> %s\n", n, k, nextk);
#endif
        if((n>0)&&(k>name)&&(k[-1]!=' ')) { /* subsequent words must be on the beginning of a word boundary */
          n--; /* retry n */
        }
        k = nextk;
      }
    }
    if(k != NULL) {
      isFound = TRUE;
    }
  }
  if(isFound == TRUE) {
    U16_APPEND_UNSAFE(workbuffer, worklen, code);
  }
  return TRUE;
 }
}


/**
 * Parse a single command
 */
void ucd_parseCommand(const UChar *cmd, UErrorCode &status)
{
  if(cmd == NULL) {
    if(VERBOSE) u_fprintf(ustderr, " #<NULL> (cmds:%d, wklen:%d)\n", commands_processed, worklen);
  } else {
    if(VERBOSE) u_fprintf(ustderr, " #%S\n", cmd);
  }
  
  UChar cmdChar;

  if(cmd == NULL) {
    if(commands_processed == 0) {
      u_fprintf(ustderr, "Nothing to do. (informative message here?)\n");
      return;
    } else if(worklen==0) {
      return; /* nothing to do - no chars in buffer */
    } else if(printed==0) { /* if we didn't print it */
      cmdChar = UCD_CMD_I; /* for info */
    } else {
      return; /* nothing to do. Already printed */
    }
  } else {
    cmdChar = *cmd;
    cmd++; /* skip over */
  }

  switch(cmdChar) {
  case 0:
    return;

  case UCD_CMD_DBLQUOTE:  // "
    clear_if_printed();
    // TODO: unescape
    realloc_work(u_strlen(cmd)-worklen);
    if((workmax-worklen)<=u_strlen(cmd)) {
      status = U_BUFFER_OVERFLOW_ERROR;
      u_fprintf(ustderr, "Overran internal workbuf of %d (%d chars short)\n", workmax, (u_strlen(cmd)-(workmax-worklen)));
    } else {
      u_strcat(workbuffer+worklen, cmd); // append the rest of the command as a string
      worklen += u_strlen(cmd);
    }
    break;

  case UCD_CMD_HASH: // #
    return; // don't count it
    break; // comment, nothing to do

  case UCD_CMD_QUESTION:
    {
      int32_t i = 0;
      UChar32 c;
      int32_t n =0;
      while(i<worklen) {
        U16_NEXT(workbuffer, i, worklen, c);
        n++;
      }
      if(n==worklen) {
        u_fprintf(ustdout, "Workbuf has %d code points in it.\n", worklen);
      } else {
        u_fprintf(ustdout, "Workbuf has %d code units and %d code points in it.\n", worklen, n);
      }
    }
    break;

  case UCD_CMD_SLASH: // /
    clear_wb(); // prepare buffer
    if(*cmd ==0) {
      u_fprintf(ustderr, "ERR: need something to search for after /\n");
      status = U_PARSE_ERROR;
    } else {
      search_setup(cmd, status); /* break apart words */
    
      u_enumCharNames(0, 
                      UCHAR_MAX_VALUE,
                      search_fn,
                      NULL,
                      U_UNICODE_CHAR_NAME,
                      &status);
      if(U_FAILURE(status)) {
        u_fprintf(ustderr, "Search fail: %s\n", u_errorName(status));
      }
      // match fn will append to workbuffer
    }
    break;

  case UCD_CMD_LEFTSQ: // [
    {
      clear_if_printed();
      USet *aSet = uset_openPattern(cmd-1, -1, &status);
      if(U_FAILURE(status)) {
        u_fprintf(ustderr, "Failed to open set: %s\n", u_errorName(status));
      } else {
        uset_freeze(aSet);
        for(UChar32 ch = 0;ch < UCHAR_MAX_VALUE;ch++) {
          if(uset_contains(aSet, ch)) {
            if((workmax-10)>worklen) {
              realloc_work(workmax*2);
            }
            U16_APPEND_UNSAFE(workbuffer, worklen, ch);
            if(worklen >= workmax) {
              u_fprintf(ustderr, "Overran internal workbuf of %d\n", workmax);
              clear_wb();
              status = U_BUFFER_OVERFLOW_ERROR;
              uset_close(aSet);
              return;
            }
          }
        }
        uset_close(aSet);
      }
    }
    break;

  case UCD_CMD_0: // 0
    clear_wb();
    if(VERBOSE) u_fprintf(ustderr, "Flushed workbuf.\n");
    break;

  case UCD_CMD_H:
    ucd_cmd_help(status);
    break;

  case UCD_CMD_P: 
    {
      int32_t n = u_file_write(workbuffer, worklen, ustdout);
      u_fflush(ustdout);
      u_fputc(UCD_CMD_CR, ustdout);
      u_fflush(ustdout);
      if(VERBOSE) u_fprintf(ustderr, "Wrote %d uchars + LF\n", n);
      set_printed();
    }
    break;

  case UCD_CMD_Q:
    DONE =1;
    return;

  case UCD_CMD_UU: // U
  case UCD_CMD_U:  // u
    {
      clear_if_printed();
      int hex=1;
      if(*cmd == UCD_CMD_PLUS) { cmd++; } // skip + in u+
      if(*cmd == UCD_CMD_HASH) { hex=0; cmd++; } // decimal mode
      UChar32 ch;
      int32_t res;
      if(hex) {
        res = u_sscanf(cmd, "%lX", &ch);
      } else {
        res = u_sscanf(cmd, "%ld", &ch);
      }

      if(ch>UCHAR_MAX_VALUE) {
        u_fprintf(ustderr, "Could not parse u+ sequence, exceeds max value U+%04X: '%S'\n", UCHAR_MAX_VALUE, cmd);
        status = U_PARSE_ERROR;        
      } else if(res!=1) {
        u_fprintf(ustderr, "Could not parse u+ sequence: '%S'\n", cmd);
        status = U_PARSE_ERROR;
      } else {
        if(VERBOSE) u_fprintf(ustderr, "Parsed: %04X\n", ch);
        U16_APPEND_UNSAFE(workbuffer, worklen, ch);
      }
    }
    break;

  case UCD_CMD_UI:
    {
      ucd_cmd_info_long(workbuffer, worklen, status);
      set_printed();
    }
    break;

  case UCD_CMD_I:
    {
      ucd_cmd_info(workbuffer, worklen, status); // show info
      set_printed();
    }
    break;

  default:
    u_fprintf(ustderr, "Unknown command (%C) (U+%04X)\n", cmdChar, cmdChar);
    status = U_PARSE_ERROR;
    break;
  }
  if(VERBOSE) u_fprintf(ustderr, "#-\n");

  commands_processed++;
}

/**
 * Startup
 */
void parseRc(UErrorCode &status)
{
  if(U_FAILURE(status)) return;
  if(PARSERC) {
    const char *home = getenv("HOME");
    if(home&&*home) {
      char buf[2048];
      strcpy(buf, home);
      strcat(buf, U_FILE_SEP_STRING);
      strcat(buf, UCD_RCFILE);
      ucd_parseFile(buf, status);
      if(VERBOSE) u_fprintf(ustderr, "# parse %s, status %s\n", buf, u_errorName(status));
    } else {
      if(VERBOSE) u_fprintf(ustderr, "# No home, so no .rcfile\n");
    }
    status = U_ZERO_ERROR;
  }
}

/**
 * Load in/out/err files
 */
void initFiles(UErrorCode &status)
{
  u_init(&status);
  if(U_FAILURE(status)) {
    u_fprintf(ustderr, "Couldn't init ICU. %s\n", u_errorName(status));
    return;
  }
  def = ucnv_open(NULL, &status);
  if(def==NULL||U_FAILURE(status)) {
    u_fprintf(ustderr, "Couldn't open default converter (%s): %s\n", ucnv_getDefaultName(), u_errorName(status));
    return;
  }

  ustdout = u_finit(stdout, NULL, NULL);
  ustdin = u_finit(stdin, NULL, NULL);
  ustderr = u_finit(stderr, NULL, NULL);

  if(ustderr==NULL||ustdout==NULL||ustdin==NULL) {
    fprintf(stderr, "Error: couldn't open stdin/stdout/stderr\n");
    status = U_FILE_ACCESS_ERROR;
  }
}

void closeFiles()
{
  u_fflush(ustderr);
  u_fflush(ustdout);
  u_fflush(ustdin);
  u_fclose(ustdin);
  u_fclose(ustdout);
  u_fclose(ustderr);
  ucnv_close(def);
  def = NULL;
}

void printFileInfo(UFILE *targ, UFILE *f, const char *name)
{
  u_fprintf(targ, "%s: locale %s, codepage %s\n", name, u_fgetlocale(f), u_fgetcodepage(f));
}

void printVersion() 
{
    UVersionInfo iver;
    char iverstr[50];
    UVersionInfo uver;
    char uverstr[50];
    u_fprintf(ustdout, "Unicode Character Database tool\n");
    u_fprintf(ustdout, "%s\n", U_COPYRIGHT_STRING);
    u_fprintf(ustdout, "  built against ICU %s and Unicode %s\n",
            U_ICU_VERSION, U_UNICODE_VERSION);
    u_getVersion(iver);
    u_getUnicodeVersion(uver);
    u_versionToString(iver, iverstr);
    u_versionToString(uver, uverstr);
    u_fprintf(ustdout, "running against ICU %s and Unicode %s\n", iverstr, uverstr);
    printFileInfo(ustdout, ustdin, "<in ");
    printFileInfo(ustdout, ustdout, ">out");
    printFileInfo(ustdout, ustderr, "!err");
}

/**
 * Print usage of the program
 * @param argv0 argv[0]
 */
void printUsage(const char *argv0)
{
	u_fprintf(ustdout, "Usage: %s "
#ifdef U_WINDOWS
		" [ -W ] "
#endif
		" [ options.. ] [ commands... ] \n", argv0);

#ifdef U_WINDOWS
	u_fprintf(ustdout, "  On Windows, the -W option must be first, and requests a UTF-8 console.\n");
#endif

	u_fprintf(ustdout, "Options:\n"
					   " -h   print this help text\n"
					   " -v   set verbose (debugging) mode\n"
					   " -n   don't read the file $HOME/" UCD_RCFILE " on startup\n"
					   " -V   print version and configuration information\n"
					   " -f filename             Run commands from filename\n"
					   " -i   stay in interactive mode even if there are commands on the commandline.\n");

	u_fprintf(ustdout, "\n\n commands can be any valid command, one per argument. So for example to run the \" (doublequote) command you might need to type:\n"
		"\t%s '\"abc' \n", argv0);


	u_fprintf(ustdout, "Try '%s h' to list valid commands.\n", argv0);
}

int main(int argc, const char** argv)
{

  UErrorCode status = U_ZERO_ERROR;

  realloc_work(65535);

  workbuffer[0]=0;
  worklen=0;
  printed=0;

#ifdef U_WINDOWS
  for(int i=1;U_SUCCESS(status)&&i<argc;i++) {
	  if(!strcmp(argv[i],"-W")) {
		ucnv_setDefaultName("utf-8");
		SetConsoleCP(CP_UTF8);
		fprintf(stderr, "Windows mode enabled\n");
	  }
  }
#endif

  initFiles(status);
  if(U_FAILURE(status)) return 1;

  for(int i=1;U_SUCCESS(status)&&(i<argc);i++) {
    if(argv[i][0]=='-') {
      switch(argv[i][1]) {
	  case 'h':
		printUsage(argv[0]);
		DONE=1;
		break;

      case 'v':
        VERBOSE=1;
        break;
	  
	  case 'W':
		  break; // ignored here
      
	  case 'V':
        {
			printVersion();
	        DONE=1;
        }
        return 0;
      case 'n':
        PARSERC=0;
        break;
      case 'i':
        INTERACTIVE=2;
        break;
      case 'f':
        i++;
        if(i>=argc) {
          u_fprintf(ustderr,"Error: -f option takes argument of command to run.\n");
          return 1;
        }
        // don't parse here, just arg checking
        break;
      }
    }
  }

  /* parse RC */
  parseRc(status);
  if(U_FAILURE(status)) return 1;

  /* parse commands from command line */
  int oldcommands_processed = commands_processed; /* save this off */
  int nodoublehyphen=1; /* 0 = in '--' mode where everything's a command  */
  for(int i=1;!DONE&&U_SUCCESS(status)&&(i<argc);i++) {
    if((argv[i][0]=='-') && nodoublehyphen) {
      switch(argv[i][1]) {
	  case 'W':
		  break; // ignored
      case 'f':
        i++;
        ucd_parseFile(argv[i],status);
        break;
      case '-':
        nodoublehyphen=0;
        break;
      }
    } else {
      UChar buf[2048];
      int ln = ucnv_toUChars(def, buf, 2048, argv[i], strlen(argv[i]), &status); // todo: better err msg
      buf[ln]=0;
      ucd_parseCommand(buf, status);
    }
  }

  if(U_SUCCESS(status) && !DONE && ((oldcommands_processed == commands_processed)||INTERACTIVE)) {
    /* read from stdin */
    ucd_parseFile(ustdin, "(stdin)", status);
  }
  
  /* Process a FLUSH (implicit 'i'nfo command) */
  if(U_SUCCESS(status) && !DONE) {
    ucd_parseCommand(NULL, status);
  }
  
  closeFiles();
  
  return (U_FAILURE(status));
}
