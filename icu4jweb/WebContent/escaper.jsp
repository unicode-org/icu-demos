<html>
<head>
  <meta content="Copyright (c) 2008-2008 IBM Corporation and others. All Rights Reserved." name="COPYRIGHT" />
<title>Escaper</title>
</head>
<body>

<%@ include file="demolist.jsf"  %>

<%
  String s = request.getParameter("v");
if(s==null) s="";
%>

<h1>Type some stuff.</h1>
<form method="GET">
<input value="<%= s %>" name="v">
<input type="submit">
</form>
<hr>
<hr>
<pre> const UChar chars[<%= s.length() %>] = { <%
 for(int i=0;i<s.length();i++ ) {
%>0x<%= Integer.toHexString( ((int)s.charAt(i)) & 0xFFFF) %><%= 
	((i+1)==s.length())?"":", " %><%
 }
%> }; /* <%= s %> */
</pre>



</body>
</html>