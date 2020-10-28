/******************************************************************************
*
*   Copyright (C) 1999-2009, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  dumpdat.c
*   encoding:   US-ASCII
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 1999nov25
*   created by: Markus W. Scherer  - gencmn
*   modified by: steven r loomis [srl]
*
*
*/

#include "unicode/utypes.h"
#include "unicode/putil.h"
#include "unicode/udata.h"
#include "unicode/uloc.h"
#include "unicode/ucnv.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_CONFIG_H
#include "demo_config.h"
#endif

#ifdef HAVE_READLINE
#include <readline/readline.h>
#endif

/* ---- Can we do dynamic loads?  #define CAN_DYNAMIC_LOAD 1 if so. ======== */

/* dlopen (macosx, etc) ---------------------------------------------------- */
#ifdef HAVE_DLOPEN
#define CAN_DYNAMIC_LOAD 1

#ifdef HAVE_DLFCN_H
#include <dlfcn.h>
#endif

void *open_dl(const char *name) {
    return dlopen(name, RTLD_NOW|RTLD_GLOBAL);
}
int close_dl(void *lib) {
    return dlclose(lib);
}
void *sym_dl(void *lib, const char* sym) {
    void *found;
    found= dlsym(lib, sym);
    if(!found) {
        fprintf(stderr, "dlsym: %s\n", dlerror());
    }
    return found;
}

#endif

/* nothing. ---------------------------------------------------- */

#ifndef CAN_DYNAMIC_LOAD
/* no way found. */
#define CAN_DYNAMIC_LOAD 0
#endif

/* prototypes --------------------------------------------------------------- */
void printVersion(const uint8_t  *v);
void printInfo(const UDataInfo *info);


/* --------------------------------------------------------------------------

 */
void printVersion(const uint8_t  *v)
{
  int i;
  for(i=0;i<4;i++)
    fprintf(stdout, "%3d%c", (int)v[i], (i==3?' ':'.'));

  for(i=0;i<4;i++)
    fprintf(stdout, "%c", isprint(v[i])?v[i]:'_');

  fprintf(stdout, "\n");
}

void printInfo(const UDataInfo *info)
{
    printf("Size: %d, Endianness: %c, Charset family: %c, ",
	   (int)info->size,
	   "lB?"[info->isBigEndian],
	   "AE?"[info->charsetFamily]);
 
    printf("UChar=%d bytes.\n", info->sizeofUChar);

    printf("dataFormat   =");
    printVersion(info->dataFormat);
    printf("formatVersion=");
    printVersion(info->formatVersion);
    printf("dataVersion  =");
    printVersion(info->dataVersion);
}

static UBool
isAcceptable(void *context,
             const char *type, const char *name,
             const UDataInfo *pInfo) {
  fprintf(stderr, "isAcceptable() called on:\n");
  printInfo(pInfo);
  fprintf(stderr, "\n");
    return true;
}

#if CAN_DYNAMIC_LOAD

void *theLib = NULL;

void cmd_C(const char *buf)
{
    if(theLib != NULL)
    {
        if(close_dl(theLib))
        {
            perror("dlclose");
            fprintf(stderr, "Closed, with err\n");
        } else {
            fprintf(stderr, "Closed.\n");
        }
        theLib = NULL;
    }
}

void cmd_O(const char *buf)
{
    void *p;

    if((buf[1]!=' ')||(buf[2]==0))
    {
        fprintf(stderr, "Usage:  O library...\n");
        return;
    }

    /* close the buffer if it is open. */
    cmd_C(buf);

    fprintf(stderr, "Opening: [%s]\n", buf+2);
    p = open_dl(buf+2);
    if(!p) {
        perror("dlopen");
        fprintf(stderr, "Didnt' open.\n");
    }
    else
    {
        fprintf(stderr, " -> %p\n", p);
    }

    theLib = p;
}

void cmd_S(char *buf)
{
    UErrorCode err = U_ZERO_ERROR;
    char *equals;
    void *foo;
    const char *lib;


    if(buf[0]=='S')
    {
        equals=strchr(buf, '=');
        
        if(!equals||strlen(buf)<5)
        {
            fprintf(stderr, "usage:  S myapp_dat=myapp\n");
            return;
        }
        *equals=0;
        equals++;
    }
    else if(buf[0]=='I')
    {
        if(strlen(buf)<5 || buf[1]!=' ')
        {
            fprintf(stderr, "Usage: I " U_ICUDATA_NAME "_dat\n");
            return;
        }
        equals="ICUDATA";
    }
    
    lib = (buf + 2);

    if(theLib == NULL)
    {
        fprintf(stderr, "loading global %s as package %s\n", lib, equals);
    }
    else
    {
        fprintf(stderr, "loading %p's %s as package %s\n", theLib, lib, equals);
    }

    foo = sym_dl(theLib, lib);
    if(!foo) {
        perror("dlsym");
        fprintf(stderr, " Couldn't dlsym(%p,%s)\n", theLib, lib);
        return;
    }
    if(buf[0]=='S') {
        udata_setAppData(equals, foo, &err);
        if(U_FAILURE(err))
        {
            fprintf(stderr, " couldn't setAppData(%s, %p, ...) - %s\n",
                    equals, foo, u_errorName(err));
            return;
        }
        fprintf(stderr, " Set app data - %s\n", u_errorName(err));

    } else if(buf[0]=='I') {
        udata_setCommonData(foo, &err);
        if(U_FAILURE(err))
        {
            fprintf(stderr, " couldn't setCommonData(%s, %p, ...) - %s\n",
                    equals, foo, u_errorName(err));
            return;
        }
        fprintf(stderr, " Set cmn data - %s\n", u_errorName(err));
        
    } else
    {
        fprintf(stderr, "Unknown cmd letter '%c'\n", buf[0]);
        return;
    }

}

#endif

void
cmd_help()
{
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "\th                     - print the top 256 bytes of the item\n");
    fprintf(stderr, "\ti                     - print info on current item\n");
    fprintf(stderr, "\tl %cpath%cpkg|name.type - load item of path, package, 'name' and 'type'\n", U_FILE_SEP_CHAR, U_FILE_SEP_CHAR);
    fprintf(stderr, "\tl name.type           - load item of 'name' and 'type'\n");
    fprintf(stderr, "\tl pkg|name.type       - load item of package, 'name' and 'type'\n");
    fprintf(stderr, "\tp                     - print the path\n");
    fprintf(stderr, "\tp=                    - set the path to empty\n");
    fprintf(stderr, "\tp=%ca%cb%c%cstuff%cmypkg.dat - set the path\n", U_FILE_SEP_CHAR,U_FILE_SEP_CHAR,U_PATH_SEP_CHAR,U_FILE_SEP_CHAR, U_FILE_SEP_CHAR,U_PATH_SEP_CHAR, U_FILE_SEP_CHAR, U_FILE_SEP_CHAR);
    fprintf(stderr, "\tq                     - quit the program\n");
    fprintf(stderr, "\tu                     - unload data item\n");
    fprintf(stderr, "\tv                     - print version and info (Loads data!)\n");
    fprintf(stderr, "\t?                     - show this help\n");
#if CAN_DYNAMIC_LOAD
    fprintf(stderr, " Dynamic Load Functions:\n");
    fprintf(stderr, "\tO whatever.dll        - DLL load\n");
    fprintf(stderr, "\tC                     - close DLL\n");
    fprintf(stderr, "\tS myapp_dat=myapp     - load app data myapp from package myapp_dat\n");
    fprintf(stderr, "\tI icuwhatever_dat     - load ICU data\n");
    fprintf(stderr, "\tI " U_ICUDATA_NAME "      ( default for 'I')\n");
#endif
    fprintf(stderr, "\n");
}

const char *prettyDir(const char *d)
{
    if(d == NULL) return "<NULL>";
    if(*d == 0) return "<EMPTY>";
    return d;
}

void cmd_version(UBool noLoad)
{
    UVersionInfo icu;
    char str[200];
    u_getVersion(icu);
    u_versionToString(icu, str);
    fprintf(stderr, "\nCompiled against ICU " U_ICU_VERSION ", currently running ICU %s\n", str);
    fprintf(stderr, "Default locale is %s\n", uloc_getDefault());
    if(noLoad == false)
    {
        fprintf(stderr, "Default converter is %s.\n", ucnv_getDefaultName());
    }
    fprintf(stderr, "ICUDATA is %s\n", U_ICUDATA_NAME);
}

void cmd_path(const char *buf)
{
    if(buf[1]=='=')
    {
        fprintf(stderr, "ICU data path was %s\n", prettyDir(u_getDataDirectory()));
        if((buf[2]==0)||(buf[2]==' '))
        {
            u_setDataDirectory("");
            fprintf(stderr, "ICU data path set to EMPTY\n");
        }
        else
        {
            u_setDataDirectory(buf+2);
            fprintf(stderr, "ICU data path set to %s\n", buf+2);
        }
    }
    fprintf(stderr, "ICU data path is  %s\n", prettyDir(u_getDataDirectory()));
}

void cmd_unload(const char *buf, UDataMemory *old)
{
    if(old)
    {
        fprintf(stderr, "Unloading data at %p\n", old);
        udata_close(old);
    }
}

UDataMemory *cmd_load(char *buf, UDataMemory *old)
{
    const char *pkg;
    char *name;
    char *type;
    UDataMemory *data;

    UErrorCode errorCode=U_ZERO_ERROR;
    cmd_unload(buf, old);


    if((buf[1] != ' ')||(buf[2]==0))
    {
        fprintf(stderr, "Load: Load what?\n");
        fprintf(stderr, "Use ? for help.\n");
        return NULL;
    }

    name = strchr(buf+2, '|');
    if(name == NULL)  /* separator not found */
    {
        pkg = NULL;
        name = buf+2;
    }
    else
    {
        pkg = buf+2; /* starts with pkg name */
        *name = 0; /* zap | */
        name++;
    }
    
    type = strchr(name, '.');
    if(type == NULL)
    {
        fprintf(stderr, "Bad type. Use ? for help.\n");
        return NULL;
    }
    
    *type = 0;
    type++;

    if(*type == 0)
    {
        fprintf(stderr, "Bad type. Use ? for help.\n");
        return NULL;
    }

    if(*name == 0)
    {
        fprintf(stderr, "Bad name. use ? for help.\n");
        return NULL;
    }
    
    fprintf(stderr, "Attempting to load::    %s | %s . %s  ", 
           prettyDir(pkg),
           name,
           type);

    fflush(stdout);
    fflush(stderr);
    data = udata_open(pkg, type, name, &errorCode);
    fflush(stdout);
    fflush(stderr);
    fprintf(stderr, " ->  %s (%p)\n", u_errorName(errorCode), data);
    
    if(U_FAILURE(errorCode) && data)
    {
        fprintf(stderr, "Forcing data %p to close.\n", data);
        udata_close(data);
        data = NULL;
    }
    
    return data;
}

void cmd_info(const char *buf, UDataMemory *data)
{
    UDataInfo info;

    if(data == NULL) {
        fprintf(stderr, "Err, no data loaded\n");
        return;
    }

    info.size = sizeof(info);
    udata_getInfo(data, &info);
    
    printInfo(&info);
}

void
doInteractive()
{
    UDataMemory *data=NULL;

    char linebuf[1024];
    char loaded[1024];
#ifdef HAVE_READLINE
    char *rl;
#endif

    cmd_version(true);
    cmd_path("p");
    fprintf(stderr, "\nEntering interactive mode. Typing ? gets help.\n");
#if CAN_DYNAMIC_LOAD
    fprintf(stderr, "(Dynamic loading available.)");
#endif
#ifdef HAVE_READLINE
    fprintf(stderr, "(readline mode.)\n");
    while(rl=readline("==> "))
    {
        strcpy(linebuf, rl);
#else
    fprintf(stderr, "==> ");
    while(!feof(stdin) && fgets(linebuf, 1024, stdin))
    {
#endif
        if(linebuf[strlen(linebuf)-1]=='\n')
        {
            linebuf[strlen(linebuf)-1]=0;
        }

        switch(linebuf[0])
        {
#if CAN_DYNAMIC_LOAD
        case 'O':
            cmd_O(linebuf);
            break;
        case 'C':
            cmd_C(linebuf);
            break;
        case 'S':
            cmd_S(linebuf);
            break;
        case 'I':
            cmd_S(linebuf);
            break;
#endif

        case '!':
            system(linebuf+1);
            break;

        case '?':
            cmd_help();
            break;

        case 'i':
            cmd_info(linebuf, data);
            break;
            
        case 'v':
            cmd_version(false);
            break;

        case 'l':
            strcpy(loaded, linebuf);
            data = cmd_load(linebuf, data);
            break;
            
        case 'p':
            cmd_path(linebuf);
            break;

        case 'q':
            fprintf(stderr, "Bye.\n");
            cmd_unload(linebuf, data);
            return;
            break;
            
        default: 
            fprintf(stderr, "Unknown option '%c' (use ? for help).\n", linebuf[0]);
            break;

        case 0:
        case ' ':
            ;
        }

#ifndef HAVE_READLINE
        fprintf(stderr, "==> ");
#else
        free(rl);
#endif
    }
}

extern int
main(int argc, char *argv[]) {
    static uint8_t buffer[4096];
/*    char line[512];*/
/*    char *s;*/
    UErrorCode errorCode=U_ZERO_ERROR;
    UDataInfo  info;
/*    uint32_t i, fileOffset, basenameOffset, length; */
    UDataMemory *data;
    const uint16_t *table=NULL;

    if(argc == 1)
    {
        doInteractive();
    }
    else
    {

        if( (argc<3) || (argc >4) ) {
            fprintf(stderr, stderr,
                    "usage: %s <type> <name>\n" /* [ path ] */
                    "\tPrint the UDATA header for a data type. [try '%s icu unames']\n",
                    argv[0],argv[0]);
            return U_ILLEGAL_ARGUMENT_ERROR;
        }
        
        data=udata_openChoice(NULL, argv[1], argv[2], isAcceptable, NULL, &errorCode);
        if(U_FAILURE(errorCode)) {
            fprintf(stderr, stderr, "%s: error %s [%d]\n", argv[0],u_errorName(errorCode),
                    (int)errorCode);
            return errorCode;
        }
        
        table=(const uint16_t *)udata_getMemory(data);
        
        info.size = sizeof(info);
        udata_getInfo(data, &info);
        
        printInfo(&info);
        
        udata_close(data);
    }

    return 0;
}

