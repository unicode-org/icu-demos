<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<%@ page language="java" contentType="text/html; charset=utf-8"
 pageEncoding="utf-8"%>
<%@ page import="java.io.*" %>
<%@ page import="com.ibm.icu.util.*" %>
<%@ page import="com.ibm.icu.text.*" %>
<%@ page import="com.ibm.icu.dev.demo.icu4jweb.ListPatternFormatter" %>
<%@ page import="java.text.ParsePosition" %>
<%@ page import="javax.xml.parsers.*" %>
<%@ page import="org.w3c.dom.*" %>
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta content="Copyright (c) 2008-2010 IBM Corporation and others. All Rights Reserved." name="COPYRIGHT" />
  <meta content="text/html; charset=utf-8" http-equiv="Content-Type" />
<title>Tuesdays and Thursdays</title>
<%@ include file="demohead.jsf" %>
</head>
<body>

<%@ include file="demolist.jsf"  %>

<%@ include file="locale_chooser.jsf" %>

<%

int dayList[] = { Calendar.TUESDAY, Calendar.THURSDAY };
String dayNames[] = new String[dayList.length];

DateTimePatternGenerator gen = DateTimePatternGenerator.getInstance(locale);

DateFormat yearFormat = new SimpleDateFormat(gen.getBestPattern("y"), locale);
DateFormat yearMonthFormat = new SimpleDateFormat(gen.getBestPattern("yMMMM"), locale);
DateFormat monthFormat = new SimpleDateFormat(gen.getBestPattern("MMMM"), locale);
DateFormat dayMonthFormat = new SimpleDateFormat(gen.getBestPattern("dMMMM"), locale);
DateFormat longDayFormat = new SimpleDateFormat(gen.getBestPattern("EEEE"), locale);
DateFormat longDayDateFormat = new SimpleDateFormat(gen.getBestPattern("dEEEE"), locale);
String yearName = gen.getAppendItemName(DateTimePatternGenerator.YEAR);

Calendar c = Calendar.getInstance(locale);

c.setMinimalDaysInFirstWeek(1);


// calculate the list of day names
for(int i=0;i<dayList.length;i++) {
	c.set(Calendar.DAY_OF_WEEK,dayList[i]);
	dayNames[i] = longDayFormat.format(c);
}

// calculate the string with the list of names
ListPatternFormatter lpf = ListPatternFormatter.getInstance(locale);
String namesList = lpf.format(dayNames);


String year = request.getParameter("year");
int yearNum = -1;
if(year != null && year.length()>0) {
	ParsePosition pp = new ParsePosition(0);
	yearFormat.parse(year, c, pp);
	if(pp.getErrorIndex() != -1) {
		out.println("<b>parse err: "+pp.toString()+"</b> on ["+year+"]<br>");
	}
} else {
	yearNum = c.get(Calendar.YEAR);
}

%>

<form action="<%= request.getContextPath() + request.getServletPath() %>" method="get">
<label><%= yearName %>: <input name='year' value='<%= yearFormat.format(c) %>'></label>
<%= localeHidden %>
<input type=submit>
</form>

<hr>

<center><h1><%= yearFormat.format(c) %> <%= namesList %> </h1></center>

<%
int minmo = c.getActualMinimum(Calendar.MONTH);
int maxmo = c.getActualMaximum(Calendar.MONTH);

%>
<table style="padding: 1em;">
<tr><td style="padding: 1em;" valign="top">
<%
boolean split = false;
for(int m=minmo;m<=maxmo;m++) {
	if(!split && m > ((maxmo-minmo)/2)) { // split
		%></td><td style="padding: 1em;" valign="top"><%
		split=true;
	}

  c.clear();
  c.set(Calendar.YEAR, yearNum);
  c.set(Calendar.MONTH, m);
%>
  <h2><%= monthFormat.format(c.getTime()) %></h2>

	<% 
		/* find first tue/thu and roll */
		int minw = c.getActualMinimum(Calendar.WEEK_OF_MONTH);
		int maxw = c.getActualMaximum(Calendar.WEEK_OF_MONTH);
		for(int w=minw;w<=maxw;w++) {
			
			for(int i=0;i< dayList.length;i++) {
			 /* t1: have we a Tuesday? */
				c.clear();
				c.set(Calendar.YEAR, yearNum);
				c.set(Calendar.MONTH, m);
				c.set(Calendar.WEEK_OF_MONTH, w);
				c.set(Calendar.DAY_OF_WEEK, dayList[i]);
				if(c.get(Calendar.MONTH)==m) {
					out.println(longDayDateFormat.format(c.getTime()) + "<br>");
				}
			}
		}
	%>
<%
  }
%>
</td></tr>
</table>

</body>
</html>
