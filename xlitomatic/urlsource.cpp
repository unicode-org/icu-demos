/**********************************************************************
*   Copyright (C) 2000, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

#include "urlsource.h"
# include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <unistd.h>
#include <string.h>
#include <time.h>
#include<stdlib.h>
# include <netinet/tcp.h>
# include <sys/types.h>
#include <fcntl.h>


#define LINEBUFSIZ 10

/* Error Handling */
const char *alarmError = NULL;

void alrmHandler(int signnum)
{
  fprintf(stderr, "Caught signal %d, msg = %s\n", signnum, alarmError);
  fprintf(stdout, "Content-type: text/html\r\n\r\n<B>Error: [%s]", alarmError);
  // Can't throw from here.. :( 
  exit(0);
}

int reAlarm(int secs)
{
  alarm(secs);
}

void setAlarm(const char *err = NULL, int secs=0)
{
  signal(SIGALRM, alrmHandler);
  if(err == NULL)
    {
      secs = 0;
    }
  else if(secs == 0)
    {
      err = NULL;
    }

  alarmError = err;
  
  // reset the alarm
  alarm(secs);
}

struct URLSourceImp
{
public:
  bool encodingTry;
  struct sockaddr_in skin;
  struct hostent  *hp;
  int     fSd;
  bool_t eof;
  
  char      buf[LINEBUFSIZ];
  int32_t   rdPtr, wrPtr;

  int32_t readInternal(char *out, int32_t len);
  void readInternal(); // throws a char* error. tries to fill the buffer.
  int32_t readNextLine(char *out, int32_t len); // throws a char* error
  inline bool_t isFull() { return ((wrPtr+1)%LINEBUFSIZ)==rdPtr; }
  inline bool_t isEmpty() { return (wrPtr == rdPtr); }
  void write(const char *s);

  void connect(const char *host, int32_t port);

  int32_t copyTo(char *out, int32_t last);

};

int32_t URLSourceImp::copyTo(char *out, int32_t last)
{
  // last = our 'wrptr' for this.
  int32_t total;

  if(isEmpty())
    return 0;

  if(rdPtr > last)
    {
      memcpy(out, buf+last, LINEBUFSIZ-rdPtr);
      total += (LINEBUFSIZ-rdPtr);

      memcpy(out+total, buf, last);
    }
  else
    {
      memcpy(out, buf+rdPtr, wrPtr-rdPtr);
      total = wrPtr - rdPtr;
    }
  rdPtr = last; // advance ptr..
  return total;
}

void URLSourceImp::write(const char *s)
{
  if(fSd == -1)
    return;

  ::write(fSd, s, strlen(s));
}

int32_t URLSourceImp::readInternal(char *out, int32_t len)
{
  int32_t n;

  if(eof) return 0;

  if(fSd == -1)
    {
      throw "Read on unopened socket.";
    }

  setAlarm("reading data", 4);
  n = recv(fSd, out, len, 0);
  setAlarm(0);
  if(n == -1)
    {
      fprintf(stderr, "Got err on socket, -> EOF\n");
      eof = true;
      return 0;
    }
  if(n == 0)
    {
      eof = true;
    }
  return n;
}

void URLSourceImp::readInternal()
{
  if(eof)
    return;

  // #1 is there any space in the buffer fill it
  if(isFull())
    {
      fprintf(stderr, "BUffer is full.\n");
      return;
    }

  fprintf(stderr, "R%d, W%d:\n ", rdPtr, wrPtr);
  if(wrPtr == rdPtr)
    {
      int32_t toRead,actRead;
      toRead = LINEBUFSIZ-wrPtr-1;
      if(toRead == 0)
        {
          wrPtr = 0;
          rdPtr = 0;
        }
      else
        {
          actRead = readInternal(buf+wrPtr, toRead);
          fprintf(stderr, " .. read %d of %d\n", actRead, toRead);
          if(actRead > 0)
            {
              wrPtr += actRead;
              return;
            }
        }
    }

  if(wrPtr > rdPtr)
    {
      int32_t toRead,actRead;
      toRead = LINEBUFSIZ-wrPtr-1;
      if(toRead == 0)
        {
          wrPtr = 0;
        }
      else
        {
          actRead = readInternal(buf+wrPtr, toRead);
          fprintf(stderr, " .. read %d of %d\n", actRead, toRead);
          if(actRead > 0)
            {
              wrPtr += actRead;
              return;
            }
        }
    }
  else
    {
      int32_t toRead,actRead;
      toRead = rdPtr - wrPtr - 1;
      if(toRead <= 0)
        {
          return;
        }
      
      actRead = readInternal(buf + wrPtr, toRead);
      fprintf(stderr, " ..+ read %d of %d\n", actRead, toRead);

      if(actRead > 0)
        {
          wrPtr += actRead;
        }
    }
}

int32_t URLSourceImp::readNextLine(char *out, int32_t len)
{
  if(len < LINEBUFSIZ)
    {
      throw "give me a bigger buffer next time.";
    }

  // for now - copy all data.
  while(isEmpty() && !eof)
    {
      readInternal();
    }
  
  return copyTo(out,wrPtr);
}

void URLSourceImp::connect(const char *host, int32_t port)
{
  int32_t rv;
  setAlarm("looking up host..", 10);
  hp = gethostbyname(host);
  if(!hp)
    {
      char junk[200];
      sprintf(junk,"Cannot resolve host: %s",host);
      throw junk;
    }
  memset((char *)&skin,0,sizeof(skin));
  memcpy((char *)&(skin.sin_addr),hp->h_addr,hp->h_length);
  skin.sin_port = htons(port);
  skin.sin_family = AF_INET; 
  fSd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
  if(fSd < 0)
    {
      perror("socket");
      fSd = -1;
      throw "Error in socket()";
    }

  fprintf(stderr, "Connected to %s:%d\n", host, port);

  if (rv = ::connect(fSd,(sockaddr*)&skin,sizeof(skin)))
    {
      fSd = -1;
      perror("connect");
      throw "Error in connect";
    }
  setAlarm(0);
  eof = false;
}


URLSource::URLSource(const char *url)
  : u(url), encoding(NULL), imp(new URLSourceImp)
{
  imp->encodingTry = false;
  imp->hp = NULL;
  imp->fSd = -1;
  imp->rdPtr = 0;
  imp->wrPtr = 0;
  imp->eof = true;
}


URLSource::URLSource(const char *url, bool_t testEnc)
  : u(url), encoding(NULL), imp(new URLSourceImp)
{
  imp->encodingTry = testEnc;
  imp->hp = NULL;
  imp->fSd = -1;
  imp->rdPtr = 0;
  imp->wrPtr = 0;
  imp->eof = true;
}

void URLSource::rewind()
{
  if(!imp) return;

  if(imp->fSd != -1)
    {
      close(imp->fSd);
      imp->fSd = -1;
      imp->eof = true;
    }
}

int32_t URLSource::read(const uint8_t* &start, const uint8_t* &end)
{
  char junk[999];
  int len;
  if(fEOF)
    return 0;

  imp->connect("daphne",80);
  imp->write("GET / HTTP/1.0\r\n\r\n");
  while(!imp->eof)
    {
      len = imp->readNextLine((char*)buffer, sizeof(buffer));
      fprintf(stderr, "[%d]::: ", len);
      strncpy(junk,buffer,len);
      fprintf(stderr, "%s<<\n", junk);
    }
  fEOF = true;

  return 0;
}

URLSource::~URLSource()
{
  rewind();
  delete imp;
}

const char *URLSource::getEncoding()
{
  if(encoding != NULL)
    {
      return encoding;
    }

  if(imp->encodingTry == false)
    {
      // use a sub object
      URLSource sub(u, true);
      return sub.getEncoding();
    }

  while(encoding == NULL)
    {
      const uint8_t *start,*end;
      read(start,end);
    }

  return encoding;
}

#if 0

void processLine(char *s)
{
  char *c;
  char tmp[999];

  if(passThrough == 1)
    {
      puts(s);
      return;
    }

  if(strncmp(s,"HTTP/",5) == 0)
    {
      /*      printf("Content-type: text/html\n\n\n"); */
      /* nothing to do */
    }
  else if(strstr(s,"Content-Length"))
    {
    }
  else if(strstr(s,"Content-Type"))
    {
      if(!strstr(s,"text"))
	{
	  /* uh oh.. bail out.. */
	  passThrough = 1;
	  puts(s);
	}
      else
	{
	  strcpy(tmp,s);
	  c = strchr(tmp,';');
	  if(c == 0)
	    {
	      strcat(tmp,";charset=");
	      if((!strncmp(dstEncoding, "mt-", 3)) || (strncmp(dstEncoding, "latin3", 6)) )
		strcat(tmp, "x-user-defined");
	      else
		strcat(tmp,dstEncoding);
	    }
	  else
	    {
	      strcpy(c,";charset=");
	      strcat(c,dstEncoding);
	      puts("x-thought-i-fixed-it:");
	    }
	  
	  puts(tmp);
	}
    }
  else
    printf("%s\n",s);
}

void writeData(char *b, int n)
{
  const char *source,*consumed;
  UChar * target;

  if(passThrough == 1)
    write(1,b,n); /* straight copy for now.. */
  else
    {
      /*      srcUC,dstUC */
      int nn,j;
      UErrorCode   status;
      status = U_ZERO_ERROR;
      
      source = b;
      consumed = 0;
      target = ucbuf;

      /*      b[n] = 0; */

      target[0] = '/';
      target[1] = '/';
      target[2] = '/';

      ucnv_toUnicode(srcUC,
		     &target,
		     target+UCSIZ,
		     &source,
		     source+n,
			NULL,
		     false,
		     &status);

      
      nn = target-ucbuf;
      /*      printf("c = %d\n",consumed-b);
	      printf("ds = %d\n",source-b); */

      /* printf("Got %d(%d)->%d... [%s]\n", 
	 n,strlen(b),nn,b); fflush(stdout);   */
     
      /*      ucbuf[nn] = 0; */
 
      if(U_FAILURE(status))
	{
	  char tmp[99];
	  sprintf(tmp,"Failed to do ucnv_toUchars - %d\n",(int)status);
	  err(tmp,"");
	  return;
	}

      /* printf("%x %x %x %x\n",ucbuf[0],ucbuf[1],ucbuf[2],ucbuf[3]); */

      status = U_ZERO_ERROR;
      
      strcpy(tmpbuf,"____________");
	ucbuf[nn] = 0;
      j = ucnv_fromUChars(dstUC, tmpbuf, 10000,ucbuf,&status);
      if(U_FAILURE(status))
	{
	  char tmp[99];
	  sprintf(tmp,"Failed to do ucnv_toUchars - %d\n",(int)status);
	  err(tmp,"");
	  return;
	}
      /* printf("-->%d (%d)\n",j,strlen(tmpbuf),tmpbuf[0],tmpbuf[1],tmpbuf[2]); fflush(stdout);   */
       /*       {
	 int q;
	 for( q=0;q<j;q++)
	   {
	     if(tmpbuf[q] != 0)
	       printf("%c",tmpbuf[q]);
	   }fflush(stdout);
	   }*/
      fflush(stdout);
            write(1,tmpbuf,j);  
      fflush(stdout);
    }
}

void doload(const char *path)
{
  int rv;
  int count;

  char *cr;
	FILE *f;
	char *contentType = "text/html";
	
  chdir("/home/srl/public_html/malta/");  

  /*  err("no err- connected.",""); */
 f = fopen(path , "r");
 
  if(!f)
  {
     fprintf(stderr,"* could not load %s\n", path);
     err("Couldn't load %s", path);
     return;
  }
  
  if(strstr(path,".jpg"))
  {  contentType = "image/jpeg"; passThrough = 1; }
  else if (strstr(path,".gif"))
  { contentType = "image/gif"; passThrough = 1; }


  if(!passThrough)
  {
	  printf("Content-type: text/html; charset=%s\r\n", dstEncoding);
	  fprintf(stderr, "*Content-type: text/html; charset=%s\r\n", dstEncoding);
  }
  else
  	printf("Content-type: %s\r\n", contentType);
  
  printf("\r\n");
  fprintf(stderr, "*\r\n");

  if(!passThrough)
    {
      if(!strcmp(dstEncoding, "mt-times"))
	{
	  printf("<FONT FACE=\"MTimes\">");
	}
      else if(!strcmp(dstEncoding, "mt-tornado"))
	{
	  printf("<FONT FACE=\"tornado\">");
	}
      else if(!strcmp(dstEncoding, "latin3"))
	{
	}

      if(!strcmp(path, "TEST"))
	{
	  printf("<A TARGET=\"_top\" HREF=\"./%s\">", getenv("QUERY_STRING"));
	}
    }
    
  tmpbuf[1] = '_';
  tmpbuf[2] = '_';
  while((count = fread(tmpbuf, 1,READCHUNK, f)) >0 )
    {
	  writeData(tmpbuf,count);
    }

#if 0
      fprintf(stderr, "[read %d, %c[%02x] %c %c...]\n",
	     count,
	     tmpbuf[0],
	     (int)tmpbuf[0],
	     tmpbuf[1],
	     tmpbuf[2]);
	     fflush(stdout); 

	     

      if(foundAll == 1) /* we're just copying */
	{
	}
      else
	{
	  
	  char *startfrom; /* the beginning of the current input chunk under consideration*/
	  
	  tmpbuf[count] = 0; /* sanity! */
	  startfrom = tmpbuf; /* start from the beginning of the read buffer .. */

	  do
	    {
	      /* look for a cr. */
	      cr = strchr(startfrom,'\r');

	      if(cr >= (startfrom+count))
		cr == 0;

	      if(cr == 0) /* none this time */
		{
		  /* printf("Nocr,cpoying %d bytes \n", count);fflush(stdout); */
		  /* copy everything into the line buffer */
		  /* printf("Copying %d bytes into lb[%d]\n",count,linecnt); */
		  memcpy(&linebuf[linecnt],startfrom,count);
		  linecnt += count;
		  /* we'll pass through */
		  count = 0;
		}
	      else
		{
		  int thisline = cr - startfrom; /* # of chars on this line */

		  /* printf("Gotcr,lfrag=%d, [%x-%x] [%c]\n",thisline, startfrom, cr, startfrom[0]); fflush(stdout); */
		  
		    
		  
		  memcpy(&linebuf[linecnt],startfrom,thisline);
		  linecnt += thisline;
		  startfrom += thisline;
		  count -= thisline;

		  /* Before we go any further, eat up that CR/LF */
		  count --;
		  startfrom++;

		  if(count == 0)
		    {
		      /* printf("Need to do another read to eat up the 0x0a\n"); */
		      recv(fSd,tmpbuf,1,0);
		      /* printf("done.\n"); fflush(stdout); */
		    }
		  else
		    {
		      /* eat 0x0a this way */
		      count--;
		      startfrom++;
		    }

		  /* ** have a good line, ship it */
		  /* printf("putting in null term @ lb[%d]\n",linecnt); */
		  linebuf[linecnt] = 0;	      

		  if(linecnt == 0) /* found header stop line */
		    {
		      foundAll = 1; /* STOP */
		      /*		      printf("Content-Base: http://%s%s\n",host,path); */
		    }
		  /*		  printf("Line found=>>%s<<[%d]\n",linebuf,linecnt); */
		  processLine(linebuf);

		  /* Now, back to 0 */
		  linecnt = 0;

		  if((foundAll == 1) && (count > 0) ) /* excess data in the buffer, deal. */
		    {
		      /*		      printf("************8xxx*"); */
		      /* get the last one */
		      fflush(stdout);
		      writeData(startfrom,count);
		    }
		  
		  /*		  printf("sotw: sf=[%c%c%c...], cnt=%d\n",
			 startfrom[0],startfrom[1],startfrom[2],count);
			 fflush(stdout);*/
		}
	      fflush(stdout);

	      
	    } while (count > 0);
	}
      tmpbuf[1] = '_';
      tmpbuf[2] = '_';

    }
#endif

  if(count < 0 )
    {
      err("Error in read.","");
      perror("read");
   fclose(f);
      return;
    }
   fclose(f);
  if(!passThrough)
  {
   if(!strcmp(path, "TEST"))
     printf("</A> - %s", dstEncoding);
   else
     printf("<HR><A NAME=\"_chencoding\"><P><B>Current encoding: [click to change]<P><UL><A TARGET=_top HREF=\"./CHANGE?%s\">%s</A></UL><A HREF=\"encodings_help.html\">Help!</A></A>\n", path, dstEncoding);
   }
}





#endif
