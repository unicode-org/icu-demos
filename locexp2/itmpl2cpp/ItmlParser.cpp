//
//  ItmlParser.cpp
//  locexp2
//
//  Copyright 2011 IBM Corporation and Others. All rights reserved.
//

#include "ItmlParser.h"
#include <stdio.h>
#include "unicode/regex.h"
#include "unicode/ustdio.h"
#include "unicode/ustring.h"

#define ASSERT_STATUS if(U_FAILURE(status)) { fprintf(stderr,"%s:%d: error: %s\n", __FILE__, __LINE__, u_errorName(status)); return 8; }

#define ASSERT_STATUS0 if(U_FAILURE(status)) { fprintf(stderr,"%s:%d: error: %s, pe offset %d\n", __FILE__, __LINE__, u_errorName(status), pe.offset); return; }

ItmlParser::ItmlParser(UErrorCode &status)
: xstr(0)
{
    pattern[0]=0;
    pattern[1]=0;
    {
        UParseError pe;
        pattern[0] = RegexPattern::compile(UnicodeString("<%([01/!@=e])?",""),pe,status);
        ASSERT_STATUS0
    }
    {
        UParseError pe;
        ASSERT_STATUS0    
        {
            RegexPattern *m = RegexPattern::compile("%>",pe,status);
            pattern[1] = m;
            ASSERT_STATUS0    
            if(U_FAILURE(status) || m!=NULL) {
                ASSERT_STATUS0    
            }
        }
        ASSERT_STATUS0    
    }
    
    {
        UParseError pe;
        ASSERT_STATUS0    
        stringPattern.reset( RegexPattern::compile("%[^ ]*",pe,status));
        ASSERT_STATUS0    
    }
    {
        UParseError pe;
        ASSERT_STATUS0    
        pagePattern.reset( RegexPattern::compile("\\s*([\\w-]+)\\s+([\\w-]+)\\s*=\\s*\"([^\"]*)\"",pe,status));
        ASSERT_STATUS0    
    }
}

ItmlParser::~ItmlParser()
{
    delete pattern[0];
    delete pattern[1];
}

void ItmlParser::addFile(const char *subname, UErrorCode &status) {
    string aclass(subname);
    filelist.push_back(aclass);
}

int ItmlParser::prologue(UFILE *out) {
    
    u_fprintf(out, "#include \"webhandler.h\"\n");
    u_fprintf(out, "#include \"itmplhandler.h\"\n");
    
    
    u_fprintf(out, "#include <list>\n"
              "using namespace std;\n");
    
    u_fprintf(out, "static list<WebHandler*> handlers;\n"
              "static CompoundWebHandler *cwh=NULL;");
    for(vector<string>::iterator iter = filelist.begin();iter!=filelist.end();iter++) {
        const char *sub = (*iter).c_str();
        u_fprintf(out, "static ItmplHandler *h_%s = NULL;\n", sub,sub);
    }
    return 0;
}

struct MatchEntry {
    MatchEntry(int32_t start,int32_t end) : start(start),end(end),flag(0x0020){}
    int32_t start;
    int32_t end;
    UChar flag;
};

int ItmlParser::parse(const char *subname, UnicodeString &tmpl, UFILE *out, UErrorCode &status) {
    const int parseCount = sizeof(pattern)/sizeof(pattern[0]);
    LocalPointer<RegexMatcher> matchers[parseCount];
    auto_ptr<RegexMatcher> stringMatcher(stringPattern->matcher(tmpl,status));
    auto_ptr<RegexMatcher> pageMatcher(pagePattern->matcher(tmpl,status));
    
    
    vector<MatchEntry> matches[parseCount];
    
    for(int i=0;i<parseCount;i++) {
        RegexMatcher *m;
        matchers[i].adoptInstead(m=pattern[i]->matcher(tmpl, status));
        ASSERT_STATUS
        
        while(m->find()) {
            MatchEntry thisMatch(m->start(status),m->end(status));
            ASSERT_STATUS
            if(m->groupCount()>0) {
                UnicodeString g = m->group(1,status);
                ASSERT_STATUS
                if(g.length()>0) {
                    thisMatch.flag = g.charAt(0);
                }
            } else {
                thisMatch.flag=0;
            }
            u_fprintf(out, "// match+[%d]: %d-%d/%C\n", i, thisMatch.start,thisMatch.end,thisMatch.flag);
            matches[i].push_back(thisMatch);
        }
        //u_fprintf(out, "// matches[%d]: %d\n", i, matches[i].size());
    }
    
    // Begin generation. 
    
    // look for <%0 ...
    for(int enn=0;enn<matches[0].size();enn++) {
        const MatchEntry &theOpen = matches[0][enn];
        const MatchEntry &theClose = matches[1][enn];
        if(theOpen.flag == '0') { // postlog
            /*
             postlog.append("\n // ----user junk from ");
             postlog.append(subname);
             postlog.append("\n");
             postlog.append(tmpl, theOpen.end, theClose.start-theOpen.end);
             postlog.append("\n // - \n");
             */
            UnicodeString my0(tmpl,theOpen.end, theClose.start-theOpen.end);
            
            u_fprintf(out, " // --- user junk from %s\n", subname);
            u_fprintf(out, "  %S\n", my0.getTerminatedBuffer());
        }
    }
    
    u_fprintf(out, "class Handler_%s : public ItmplHandler {\n", subname);
    u_fprintf(out, "public:\n ");
    
    u_fprintf(out, "  virtual void handleRequest(const UnicodeString &url, UFCGI *u, const RegexMatcher *match) {\n");
    u_fprintf(out, "    const char *scriptName=\"/%s.itml\";\n", subname);
    u_fprintf(out, "   VARS_SETUP(u)\n");
    u_fprintf(out, "    // generated code goes here\n");
    
    int32_t len = tmpl.length();
    
    UnicodeString staticStuff,userCtor;
    
    UnicodeString defaultString = "%s";
    
    int32_t p; // pointer
    int32_t ent = 0; // entry in str table
    for(p=0;p<len&&U_SUCCESS(status);) {
        if(ent>=matches[0].size()) {
            writeLiteral(out, tmpl, p, len, status);
            p=len;
        } else {
            // first, write any preceding literal
            const MatchEntry &theOpen = matches[0][ent];
            const MatchEntry &theClose = matches[1][ent];
            
            writeLiteral(out, tmpl, p, theOpen.start, status);
            ASSERT_STATUS
            
            u_fprintf(out, " // Ent #[%d], flag [%C/%x]..\n", ent, theOpen.flag,theOpen.flag);
            if(theOpen.flag == ' ' || theOpen.flag==0) { // code
                for(p=theOpen.end+1;p<theClose.start;p++) {
                    u_fprintf(out, "%C", tmpl[p]);
                }
            } else if(theOpen.flag == '/') { // comment
                u_fprintf(out, "   /*** \n    *** ");
                    for(p=theOpen.end+1;p<theClose.start;p++) {
                        u_fprintf(out, "%C", tmpl[p]);
                    }
                u_fprintf(out, "\n    ***/ \n");
            } else if(theOpen.flag == '=') { // string
                UChar parm[100];
                
                if(stringMatcher->find(theOpen.end, status) && // found string params?
                   (p = (stringMatcher->end(status)+1))<theClose.start) { // and not found after start of close
                    // string flag
                    int32_t s = stringMatcher->start(status);
                    int32_t e = stringMatcher->end(status);
                    tmpl.extract(s,e-s,parm);
                    parm[e-s]=0; // null
                } else {
                    // no string flag - default
                    u_strcpy(parm,defaultString.getTerminatedBuffer());
                    p = theOpen.end;
                }
                ASSERT_STATUS
                u_fprintf(out, "\n  cgi_printf(u, \"%S\", ", parm);
                for(/*p*/;p<theClose.start;p++) {
                    u_fprintf(out, "%C", tmpl[p]);
                }
                u_fprintf(out, "  );\n");
                p = theClose.end+1;
            } else if(theOpen.flag == 'e') { // env
                u_fprintf(out, "\n  cgi_printf(u, \"<pre>\"); ");
                u_fprintf(out, "\n  cgi_dumpEnv(u); ");
                u_fprintf(out, "\n  cgi_printf(u, \"</pre>\"); ");
            } else if(theOpen.flag == '@') { // page
                if(pageMatcher->region(theOpen.end,theClose.start,status).lookingAt(status)) {
                    ASSERT_STATUS
                    UnicodeString directive = pageMatcher->group(1,status).toLower();
                    UnicodeString attr = pageMatcher->group(2,status).toLower();
                    UnicodeString aval = pageMatcher->group(3,status);
                    ASSERT_STATUS
                    u_fprintf(out, "  // @ '%S' '%S' = '%S'\n", directive.getTerminatedBuffer(), attr.getTerminatedBuffer(),aval.getTerminatedBuffer());
                    
                    if(directive == "set") {
                        if(attr == "fmt") {
                            defaultString = aval;
                        } else if(attr == "content-type") {
//                            char ctype[256];
//                            aval.extract(0, aval.length(), ctype, 256);
                            u_fprintf(out, "  cgi_setContentType(u, \"%S\");\n", aval.getTerminatedBuffer());
                        } else {
                            u_fprintf(out, "// Unknown set: %S\n", attr.getTerminatedBuffer());
                        }
                    } else if(directive == "dispatch") {
                        if(attr == "to") {
                            u_fprintf(out, "   h_%S->handleRequest(url,u,NULL);\n", aval.getTerminatedBuffer());
                        }
                    } else {
                        u_fprintf(out, "// Unknown directive: %S\n", directive.getTerminatedBuffer());
                    }
                    
                } else {
                    u_fprintf(out, "  cgi_printf(u, \"<!-- failing region match @ -->\");\n");
                }
                ASSERT_STATUS
            } else if(theOpen.flag == '!') { // static
                staticStuff.append("\n // ----user static ---- \n");
                staticStuff.append(tmpl, theOpen.end, theClose.start-theOpen.end);
                staticStuff.append("\n // - \n");
            } else if(theOpen.flag == '1') { // ctor
                userCtor.append("\n // ----user ctor ---- \n");
                userCtor.append(tmpl, theOpen.end, theClose.start-theOpen.end);
                userCtor.append("\n // - \n");
            } else if(theOpen.flag == '0') { // postlog
                /*
                postlog.append("\n // ----user junk from ");
                postlog.append(subname);
                postlog.append("\n");
                postlog.append(tmpl, theOpen.end, theClose.start-theOpen.end);
                postlog.append("\n // - \n");
                 */
            } else {
                u_fprintf(out, "    \n cgi_printf(u, \" (something clever here '%C') \");\n", theOpen.flag);
            }
            p = matches[1][ent].end;
            ent++;
        }
    }
    
    //u_fprintf(out, "    cgi_printf(u, \"%S\");\n", tmpl.getTerminatedBuffer());
    u_fprintf(out, "  }\n");
    
    u_fprintf(out, "%S\n", staticStuff.getTerminatedBuffer());

    u_fprintf(out, "public: \nHandler_%s(UErrorCode &status) : ItmplHandler(\"%s\",status) {\n\n%S\n\n}\n", subname, subname, userCtor.getTerminatedBuffer());
        
    u_fprintf(out, "};\n\n\n\n");

    ASSERT_STATUS

    return U_SUCCESS(status)?0:1;
}


void ItmlParser::writeLiteral(UFILE *out, UnicodeString &tmpl, int32_t start, int32_t limit, UErrorCode &status) {
    if(start>=limit) {
        return;
    }
    u_fprintf(out, "  const UChar xstr_%d[%d] = { ", xstr, limit-start+1);
    for(int32_t p = start;p<limit;p++) {
        u_fprintf(out, "0x%X, ", tmpl.charAt(p));        
    }
    u_fprintf(out, "0};\n");
    u_fprintf(out, "  cgi_printf(u,\"%%S\", xstr_%d);\n", xstr);
    xstr++;
}

int ItmlParser::epilogue(UFILE *out) {
    u_fprintf(out, "\n\nextern CompoundWebHandler * init_tmpl(UErrorCode &status) {\n");
    u_fprintf(out, "  cwh = new CompoundWebHandler;\n");
    
    for(vector<string>::iterator iter = filelist.begin();iter!=filelist.end();iter++) {
        const char *sub = (*iter).c_str();
        u_fprintf(out, "  h_%s = new Handler_%s(status);\n", sub,sub);
        u_fprintf(out, "  cwh->addHandler(h_%s);\n", sub);
        u_fprintf(out, "  handlers.push_back(h_%s);\n", sub);
        if(!strcmp(sub,"index")) {
            u_fprintf(out, "  cwh->setDefaultHandler(h_%s);\n", sub);
        }
    }
    
    u_fprintf(out, "  return cwh;\n");
    u_fprintf(out, "}\n");
        
    return 0;
}


