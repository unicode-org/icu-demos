<%@ page contentType="text/html; charset=utf-8" import="com.ibm.icu.text.Bidi" %>

<%
request.setCharacterEncoding("utf-8");
%>

<html>
	<head>
		<title>Bidi Page</title>
  <meta content="Copyright (c) 2008-2008 IBM Corporation and others. All Rights Reserved." name="COPYRIGHT" />
	</head>
	<body>
<%@ include file="demolist.jsf"  %>
	   <h1>Bidi</h1>
	    <hr>
	    
		<form action="<%= request.getContextPath() + request.getServletPath() %>" method="POST">
		    <textarea name='text'><%= 
		          request.getParameter("text")!=null?request.getParameter("text"):"" 
		    %></textarea>
			<input type="submit" value="Click!" />
		</form>
<hr />
<% 
    String stuff = request.getParameter("text")!=null?request.getParameter("text"):"";
    
    Bidi b = new Bidi(stuff, Bidi.OPTION_DEFAULT);
%>
    <h4>BiDi: <%= b.countRuns() %> runs</h4>
    
    <ol>
        <%
            for(int i=0;i<b.countRuns();i++) {
        %>
                <li style='margin: 1em;'>Level:<%= b.getRunLevel(i) %>, text:
                    |<span style='border: 1px solid red; margin: 1px; padding: 2px;' dir=ltr><%=
                        stuff.substring(b.getRunStart(i),b.getRunLimit(i))
                    %></span>|
        <%
            }
        %>
    </ol>
		
<hr>
	    ICU4J <%= com.ibm.icu.util.VersionInfo.ICU_VERSION %>  

	</body>
</html>
