<html>
<!-- Copyright (c) 2008 IBM. All Rights Reserved -->

<%@ page contentType="text/html; charset=utf-8" import="com.ibm.icu.text.Bidi" %>
<%@ page import="com.ibm.icu.util.*" %>
<%@ page import="com.ibm.icu.text.*" %>
<%@ page import="java.util.Random" %>


<head>
<%@ include file="demohead.jsf" %>
<title>numero</title></head>
<body>
<%@ include file="demolist.jsf"  %>


<%
	ULocale from = new ULocale("en_US_POSIX");
	ULocale to = new ULocale("es_SV");
	try {
		to = new ULocale(escapeString(request.getParameter("to")));
	} catch(Throwable t) {
		// ;
	}
	
	Random r = new Random();
	double n = ((r.nextInt())%1000000)/100.0;
	
	try {
		n = Double.parseDouble(request.getParameter("n"));
	} catch(Throwable t) {
		// ;
	}
	
%>

<div style='font-size: xx-large;'>

<div style='font-size: small;'>From: <%= from.getDisplayName() %><br></div>
<form action="<%= request.getContextPath() + request.getServletPath() %>" method="POST">
<label>Number: <input style='font-size: xx-large;' name='n' value='<%= n %>'></label>
<label>To: <input style='font-size: large;' name='to' value='<%= to.getName() %>'></label>
<input style='font-size: xx-large;' type=submit>
</form>
<hr>

<%
	RuleBasedNumberFormat rbnf = new RuleBasedNumberFormat(to, RuleBasedNumberFormat.SPELLOUT);
 %>


<b><%= rbnf.format(n) %></b><br>
To: <%= to.getDisplayName() %>

</div>
<hr>
<i>based on <a href="http://icu-project.org">ICU4J <%= com.ibm.icu.util.VersionInfo.ICU_VERSION %></i>
</body>
</html>
