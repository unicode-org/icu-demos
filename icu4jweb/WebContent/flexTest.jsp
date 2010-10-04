<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<%@ page language="java" contentType="text/html; charset=utf-8"
 pageEncoding="utf-8"%>
<%@ page import="java.io.*" %>
<%@ page import="java.util.Set" %>
<%@ page import="java.util.TreeSet" %>
<%@ page import="com.ibm.icu.util.*" %>
<%@ page import="java.util.Date" %>
<%@ page import="com.ibm.icu.text.*" %>
<%@ page import="com.ibm.icu.dev.demo.icu4jweb.ListPatternFormatter" %>
<%@ page import="java.text.ParsePosition" %>
<%@ page import="javax.xml.parsers.*" %>
<%@ page import="org.w3c.dom.*" %>
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta content="Copyright (c) 2008-2010 IBM Corporation and others. All Rights Reserved." name="COPYRIGHT" />
  <meta content="text/html; charset=utf-8" http-equiv="Content-Type" />
<title>Flex Test</title>
<%@ include file="demohead.jsf" %>
</head>
<body>

<%@ include file="demolist.jsf"  %>

<%@ include file="locale_chooser.jsf" %>

<%
final String PAT="pat";
String thePattern = request.getParameter(PAT);
if(thePattern==null) thePattern="";
DateTimePatternGenerator dtpg = DateTimePatternGenerator.getInstance(locale);
String formatName = "format";
%>

<form action="<%= request.getContextPath() + request.getServletPath() %>" method="get">
<label><%= formatName %>: <input name='<%= PAT %>' value='<%= thePattern %>' /></label>
<%= localeHidden %>
<input type="submit" />
</form>

<%

String newPat = thePattern;

try {
	newPat = dtpg.getBestPattern(thePattern);
	%>
		Best Pattern: <tt class='pat'><%= newPat  %></tt><br/>
		Example:  <span class='ex' style='border: 1px solid black; font-size: larger;'><%= new SimpleDateFormat(newPat, locale).format(new Date()) %></span><br/>
	<%
} catch(IllegalArgumentException iae) {
	out.println("<b>Error: " + iae.toString()+"</b><br/>");
}

%>


<hr/>
<form action="<%= request.getContextPath() + request.getServletPath() %>" method="get">
<%= localeHidden %>
<input type="submit"  value="clear" />
</form>

Click an item to add:<br/>
<table style='padding: 1em;'>
<tr>
<%

// copied from DTPG source :(
String[] CANONICAL_ITEMS = {
        "G", "y", "Q", "M", "w", "W", "e",
        "d", "D", "F",     /* I had to add this: >>> */ "a",
        "H", "m", "s", "S", "v"
	};
        
	SimpleDateFormat sdf = new SimpleDateFormat("y", locale);
	Date dnow = new Date();

	for(int i=0;i<DateTimePatternGenerator.TYPE_LIMIT;i++) {
		
		String name = dtpg.getAppendItemName(i);
		String field = dtpg.getAppendItemFormat(i);
		
		// Append the character, in order.
		StringBuffer pat2Buf = new StringBuffer();
		char ch = CANONICAL_ITEMS[i].charAt(0);
		char toAppend = ch;
		char patBytes[] = thePattern.toCharArray();
		StringBuffer pat2Test = new StringBuffer(CANONICAL_ITEMS[i]); // test array
		for(int j=0;j<patBytes.length;j++) {
			pat2Buf.append(patBytes[j]);
			if(patBytes[j] == ch) {
				pat2Test.append(ch);
				if(toAppend != 0) {
					pat2Buf.append(toAppend); // append our ch
					toAppend = 0; // zero our ch
				}
			}
		}
		if(toAppend!=0) {
			pat2Buf.append(toAppend);
			toAppend = 0;
		}
		sdf.applyPattern(pat2Test.toString());
		
		String pat2 = pat2Buf.toString();
		%>
		<td>
<form action="<%= request.getContextPath() + request.getServletPath() %>" method="get">
<%= localeHidden %><tt class='pat'><%= pat2Test.toString()+"</tt>: " %>
<input type="hidden" name="<%= PAT %>" value="<%= pat2 %>" />
<input type="submit"  value="<%= name + " ("+sdf.format(dnow)+")" %>" />
</form>
		</td>

		<%
		if(i%3 == 2) {
			%></tr><tr><%
		}
	}
%>
	</tr></table>
	
	<hr/>
	
	<h3>Instructions </h3>
	
	<ol>	
		<li>First, type in the locale you want to work with at the very top form, and click Submit. Use the locale ID such as en or el or ja</li>
		<li>Second, you can type in a format skeleton, or click the buttons to build up your pattern. Click clear to start over.</li>
		<li>To use one of the existing skeletons, click on the Available Skeletons.</li>
	</ol>
	
	
	
	<h3>Available Skeletons:</h3>
	<ul>
	<%
		Set<String> result = new TreeSet<String>();
		dtpg.getBaseSkeletons(result);
		
		for(String base : result) {
			%>
				<li> 
<form action="<%= request.getContextPath() + request.getServletPath() %>" method="get">
<%= localeHidden %>
<input type="hidden" name="<%= PAT %>" value="<%= base %>" />
<input type="submit"  value="<%= base %>" />
</form>
				</li>
			<%
		}
	%>
	</ul>

</body>
</html>