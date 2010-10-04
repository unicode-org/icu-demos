<!-- Copyright (c) 2008 IBM. All Rights Reserved -->
<html>
<%@ page contentType="text/html; charset=utf-8" import="com.ibm.icu.text.Bidi" %>
<%@ page import="com.ibm.icu.util.*" %>
<%@ page import="java.util.Date" %>
<%@ page import="com.ibm.icu.text.*" %>
<%@ page import="java.util.Random" %>

<head>
<title>ICU Interval Demo</title>

<%@ include file="demohead.jsf" %>
 </head>

<body >



<%@ include file="demolist.jsf"  %>




<%
/*

*/

	//ULocale from = new ULocale("en_US_POSIX");
	ULocale to = new ULocale("es_SV");
	try {
		to = new ULocale(request.getParameter("to"));
	} catch(Throwable t) {
		// ;
	}
	
	String skel = request.getParameter("skel");
	if(skel==null) skel="yMMMdd";
	
	Date ta = new Date();
	Date tb = new Date();
	
    try {
        ta = new Date(request.getParameter("ta"));
    } catch(Throwable t) {
        out.println(t.toString());
        ta = new Date();
    }
    try {
        tb = new Date(request.getParameter("tb"));
    } catch(Throwable t) {
        out.println(t.toString());
        tb = new Date();
    }

    
    DateInterval dtitv = new DateInterval(ta.getTime(), 
            tb.getTime());

String oneSkeleton = skel;
// Skipping some test case in the non-exhaustive mode to reduce the test time
//ticket#6503


DateIntervalFormat dtitvfmt = DateIntervalFormat.getInstance(
        oneSkeleton, to);
String formatted = dtitvfmt.format(dtitv);


    
%>

<div style='font-size: xx-large;'>

<form action="<%= request.getContextPath() + request.getServletPath() %>" method="POST"><br>
<label>skel: <input style='font-size: large;' name='skel' value='<%= skel %>'></label><br>
<label>A: <input size=50 style='font-size: large;' name='ta' value='<%= ta %>'></label><br>
<label>B: <input size=50 style='font-size: large;' name='tb' value='<%= tb %>'></label><br>
<label>To: <input style='font-size: large;' name='to' value='<%= to.getName() %>'></label><br>
<input style='font-size: xx-large;' type=submit>
</form>

<hr>

<hr>


<b><%= formatted %></b><br>
locale: <%= to.getDisplayName() %><br>
ta: <%= ta.toString() %><br>
tb: <%= tb.toString() %><br>

</div>
<hr>
<i>based on <a href="http://icu-project.org">ICU4J <%= com.ibm.icu.util.VersionInfo.ICU_VERSION %></i>
</body>
</html>