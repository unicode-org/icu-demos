#error update this file before use - see cgimain.c

/* Copyright (c) 2003 IBM, Inc. and Others */

/* Run locexp as a fastcgi..    http://www.fastcgi.com/

I put a script like this into the path:
 /usr/local/bin/cgi-fcgi -connect /tmp/locexp.sock.`whoami` /home/srl/II/bin/flx 2

Where flx is the file built against libfcgi
*/

#include <stdio.h>


#include <sys/resource.h>
#include "locexp.h"
#include "fcgi_stdio.h"
#include <unistd.h>

/*FILE *q; */

int main(const char *argv[], int argc);


int main(const char *argv[], int argc)
{
  int n;
  struct rusage rr;
  int life = 0;

/*  q = fopen("/tmp/qqq", "w"); */

/*  fprintf(q,"Got openance\n"); 
  fflush(q); */

  initLX();

  while((life < 50) && ((n=FCGI_Accept()) >= 0))
    {
      char * buf;
      size_t size;
      LXContext  localContext;

      life++;

      if(strstr(getenv("QUERY_STRING"),"_stats_"))
      {
        printf("content-type: text/plain\r\n\r\nUp %d tries\n", life);
        continue;
      }

/*    fprintf(q,"Got acceptance %d\n", n);x
      fflush(q); */

      size = 1;
      buf = malloc(size);
      

      initContext(&localContext);
      localContext.fOUT = open_memstream (&buf, &size);

/*    localContext.fOUT = FCGI_ToFILE(FCGI_stdout); 
      localContext.fOUT = stdout; */
      
      runLocaleExplorer(&localContext);

      closeLX(); /* ACK! */
      
/*    fflush(localContext.fOUT);
      fclose(localContext.fOUT);
*/

      {
        int left=size;
        int next;
        char *b = buf;
        
        while(left > 0)
          {
            next = 500;
            if(next > left)
              next = left;

            fwrite(b,1,next,stdout);

/*          fprintf(stdout, "||||");
*/
            left -= next;
            b += next;
          }
      }
/*    fprintf(q, "Size:%d, @%p\n",size,buf);
*/
      free(buf);
      
      if(size > 90000)
        {
          sleep(1);
        }

/*    sleep(1);
*/
      
/*     getrusage(RUSAGE_SELF, &r);
*/
      
      
    }

/* fprintf(q, " Got closance %d\n", n);
   fclose(q);
*/

  return 0;
}








