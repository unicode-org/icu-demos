<html>
<%@ page import="java.io.*" %>
<%@ page import="java.util.*" %>
<%@ page import="java.text.*" %>
<%@ page import="javax.xml.parsers.*" %>
<%@ page import="org.w3c.dom.*" %>

<head>
  <meta content="Copyright (c) 2008-2008 IBM Corporation and others. All Rights Reserved." name="COPYRIGHT" />
<title>Tuesdays and Thursdays</title>
</head>
<body>
<%@ include file="demolist.jsf"  %>

<%
Calendar c = Calendar.getInstance();
c.setMinimalDaysInFirstWeek(1);

String year = request.getParameter("year");

int yearNum = -1;
if(year != null && year.length()>0) {
	yearNum = Integer.parseInt(year);
	c.set(Calendar.YEAR, yearNum);
} else {
	yearNum = c.get(Calendar.YEAR);
}

%>

<form action="<%= request.getContextPath() + request.getServletPath() %>" method="POST">
<label>Year: <input name='year' value='<%= c.get(Calendar.YEAR) %>'></label>
<input type=submit>
</form>

<hr>

<center><h1><%= c.get(Calendar.YEAR) %> Tuesdays and Thursdays</h1></center>

<%
int minmo = c.getActualMinimum(Calendar.MONTH);
int maxmo = c.getActualMaximum(Calendar.MONTH);
for(int m=minmo;m<=maxmo;m++) {
  c.clear();
  c.set(Calendar.YEAR, yearNum);
  c.set(Calendar.MONTH, m);
%>
  <h2><%= new SimpleDateFormat("MMMM").format(c.getTime()) %></h2>

	<% 
		/* find first tue/thu and roll */
		int minw = c.getActualMinimum(Calendar.WEEK_OF_MONTH);
		int maxw = c.getActualMaximum(Calendar.WEEK_OF_MONTH);
		for(int w=minw;w<=maxw;w++) {
			
		 /* t1: have we a Tuesday? */
			c.clear();
			c.set(Calendar.YEAR, yearNum);
			c.set(Calendar.MONTH, m);
			c.set(Calendar.WEEK_OF_MONTH, w);
			c.set(Calendar.DAY_OF_WEEK, Calendar.TUESDAY);
			if(c.get(Calendar.MONTH)==m) {
				out.println(new SimpleDateFormat("EEEE d ").format(c.getTime()) + "<br>");
			}
		 /* t1: have we a Thursday? */
			c.clear();
			c.set(Calendar.YEAR, yearNum);
			c.set(Calendar.MONTH, m);
			c.set(Calendar.WEEK_OF_MONTH, w);
			c.set(Calendar.DAY_OF_WEEK, Calendar.THURSDAY);
			if(c.get(Calendar.MONTH)==m) {
				out.println(new SimpleDateFormat("EEEE d ").format(c.getTime()) + "<br>");
			}
		}
	%>
<%
  }
%>

</body>
</html>
