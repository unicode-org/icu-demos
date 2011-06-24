//
//  ufcgi.cpp
//  locexp2
//
//  Created by Steven R. Loomis on 5/16/11.
//  Copyright 2011 IBM Corporation and Others. All rights reserved.
//

#include "ufcgi.h"
#include <fcgiapp.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int running=1;

typedef struct {
    FCGX_Request req;
    char *content_type;
} Stuff;


static void copyOut(FILE *f, UFCGI *u) {
    char buf[2048];
    size_t n;
    Stuff *s = (Stuff*)u->stuff;
    
    while((n = fread(buf, sizeof(buf[0]), sizeof(buf)/sizeof(buf[0]), f))>0) {        
        FCGX_PutStr(buf, (int)n, s->req.out);
    }
}
static void writeHeader(UFCGI *u) {
    Stuff *s = (Stuff*)u->stuff;
    FCGX_FPrintF(s->req.out, "Content-type: %s\r\n\r\n", s->content_type);
}

static void init_stuff(UFCGI *u) {
    u->queryString = cgi_getParam(u,"QUERY_STRING");
    u->postData=NULL;
    u->cookies=NULL;
}

static void acceptLoop()
{
    int rc;
    
    UFCGI u;
    
    Stuff s;
    
    char fname[500];
    
//    FILE *xlog= NULL;
    
    u.stuff = &s;
  
//    xlog = fopen("/tmp/lxlog","w");
    
    tmpnam(fname);
//    
//    fprintf(xlog,"FN: %s\n",fname);
//    fflush(xlog);
    
    FCGX_InitRequest(&(s.req), 0, 0);
    
    while(running) {
        FILE *tmpf;
        
        s.content_type=strdup("text/html;charset=utf-8");
        u.user=NULL; // reset
        rc = FCGX_Accept_r(&(s.req));
        

        if(rc<0) {
            break;
        }
        
        
        tmpf = fopen(fname, "w+");
        
        if(!tmpf) {
            fprintf(stderr, "ERR: bad fname %s\n", fname);
            fflush(stderr);
            break;
        }
        
        u.out = u_finit(tmpf,NULL, "utf-8");
        
        init_stuff(&u);
        
        process_cgi(&u);
        
        u.user=NULL; // reset
        
        u_fclose(u.out);
        
        fseek(tmpf,0,SEEK_SET);
        
        writeHeader(&u);
        copyOut(tmpf,&u);
        
        fclose(tmpf);

        unlink(fname);


        FCGX_Finish_r(&s.req);
    }
    
}

U_CAPI void cgi_setContentType(UFCGI *u, const char *type) {
    Stuff *s = (Stuff*)u->stuff;
    free(s->content_type);
    s->content_type=strdup(type);
}

U_CAPI int32_t cgi_printf(UFCGI *u, const char *fmt, ...) {
    va_list ap;
    int32_t written;
    
//    if(ustdout==NULL) {
//        ustdout= u_finit(stdout, NULL, NULL);
//    }
    
    va_start(ap, fmt);
    written = u_vfprintf(u->out, fmt, ap);
    va_end(ap);
    u_fflush(u->out);
    return written;
}

U_CAPI void cgi_dumpEnv(UFCGI *u) {
    Stuff *s = (Stuff*)u->stuff;
    for(char**e = s->req.envp;
        *e;
        e++) {
        cgi_printf(u, "%s\r\n", *e);
    }
}

int main(int argc, const char *argv[])
{
    FCGX_Init();
    init_cgi();
    acceptLoop();
}

U_CAPI const char *cgi_getParam(UFCGI *u, const char *param) {
    Stuff *s = (Stuff*)u->stuff;
    return FCGX_GetParam(param, s->req.envp);
}


