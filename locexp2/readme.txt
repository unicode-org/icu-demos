# Copyright (C) 2012 IBM Corporation and others, All rights reserved

* ITML FILE FORMAT

.itml is HTML with some JSP/PHP inspired escapes to C++ code.

Briefly:

 - place .itml files in the ./itmpl directory

** % - normal code (per-request)

Example:
        <%   
                int count;
          %>

** %! - static area

Example:
 <%!
        XXX
  %>
Result:


** %1 - constructor
   Example:
        <%1 
                setupFunction(); 
           %>
  Result:
        setupFunction is called when object (handler) is initialized

** %0 - File level
   Example:
        <%0 
            #include "lxuser.h"
         %>

** %@ dispatch 

   Example:
        <%@ dispatch to="lxsetup" %>

   Result:
        The handler in lxsetup.itml  is used at this point in the file.


** %= - print

  Example:
        <% const char *aStr="This is a str";  %>
        <%= aStr %>

  Result:
        Default format %s  string is output

   Example:
        <%=%S  uchars %>
   Result:
        print null terminated uchars

    Example:
        <%=%d 42 %>
     Result:
        Print out %d formatted number 42

    See u_printf()  for format specification. Any text after the "=" and before a space is interpreted as a format specification. 


** %/  - comment
   Example:
     <%/  This is a comment %>

  The comment will show up in the cpp output.

** 
 
