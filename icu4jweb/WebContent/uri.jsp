
<%@ page contentType="text/html; charset=utf-8" import="java.net.URI" %>

<%
request.setCharacterEncoding("utf-8");
%>

<html>
	<head>
		<title>URI Page</title>
  <meta content="Copyright (c) 2008-2008 IBM Corporation and others. All Rights Reserved." name="COPYRIGHT" />
<%@ include file="demohead.jsf" %>

	</head>
	<body>
<%@ include file="demolist.jsf"  %>
	   <h1>URI</h1>
	    <hr>
	    
		<form action="<%= request.getContextPath() + request.getServletPath() %>" method="POST">
		    <textarea name='text'><%= 
		          request.getParameter("text")!=null?request.getParameter("text"):"" 
		    %></textarea>
		    <textarea name='text2'><%= 
		          request.getParameter("text2")!=null?request.getParameter("text2"):"" 
		    %></textarea>
			<input type="submit" value="Click!" />
		</form>
<hr />
<% 
    String stuff = request.getParameter("text")!=null?request.getParameter("text"):"";
    String stuff2 = request.getParameter("text2")!=null?request.getParameter("text2"):"";
    
    URI b = new URI(stuff);
//public URI(String scheme,
//           String authority,
//           String path,
//           String query,
//           String fragment)

%>
    <h4>URI: <%= b.toString() %></h4>
    
<br>
	URI: <%= b.toASCIIString() %>
	
	<br>
		
<hr>
	    ICU4J <%= com.ibm.icu.util.VersionInfo.ICU_VERSION %>  

	</body>
</html>
