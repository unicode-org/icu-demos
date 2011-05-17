<html>
<head>
  <meta content="Copyright (c) 2008-2008 IBM Corporation and others. All Rights Reserved." name="COPYRIGHT" />
<title>Escaper</title>
<%@ page contentType="text/html; charset=utf-8" import="java.net.URI" %>
<%@ include file="demohead.jsf" %>

</head>
<body>

<%@ include file="demolist.jsf"  %>

<%

     request.setCharacterEncoding("utf-8");

     String s = request.getParameter("v");
if(s==null) s="";
try  {
    byte asBytes[] = new byte[s.length()];
    boolean wasHigh = false;
    int n;
    for(n=0;n<s.length();n++) {
	asBytes[n] = (byte)(s.charAt(n)&0x00FF);
	//println(" n : " + (int)asBytes[n] + " .. ");
	if(asBytes[n]<0) {
	    wasHigh = true;
	}
    }
    
    s = new String(asBytes, "UTF-8");
} catch(Throwable t) {
    //
}
					      
%>

<h1>Type some stuff.</h1>
<form method="GET">
<input value="<%= s %>" name="v">
<input type="submit">
</form>
<hr>
<hr>
<tt> const UChar chars[<%= s.length()+1 %>] = { <%
 for(int i=0;i<s.length();i++ ) {
%>0x<%= Integer.toHexString( ((int)s.charAt(i)) & 0xFFFF) %><%= 
	((i+1)==s.length())?"":", " %><%
 }
					      %> , 0}; /* <%= s %> */
</tt>



</body>
</html>
