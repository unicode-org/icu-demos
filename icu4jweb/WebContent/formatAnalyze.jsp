<%@ page language="java"
 import="java.util.*,java.text.*,com.ibm.icu.util.*,com.ibm.icu.text.MessagePattern"
 contentType="text/html; charset=utf-8"
 pageEncoding="utf-8"%>
 <%!
 static final String escapeString(String arg) {
	    return arg.replaceAll("&", "&amp;").replaceAll("\"", "&quot;").replaceAll("<", "&lt;").replaceAll(">", "&gt;");
	}
	static final String[] escapeStrings(String[] args) {
	    for (int argIdx = 0; argIdx < args.length; argIdx++) {
	        args[argIdx] = escapeString(args[argIdx]);
	    }
	    return args;
	}
 %>
 <meta content="Copyright (c) 2008-2011 IBM Corporation and others. All Rights Reserved." name="COPYRIGHT" />
 <%
 request.setCharacterEncoding("UTF-8");
response.setContentType("text/html;charset=utf-8");

/*
String selectedLocale = request.getParameter("locale");
if (selectedLocale == null) {
    selectedLocale = request.getLocale().toString();
}
String msgType = request.getParameter("msgType");
if (msgType== null) {
    msgType="icu4j";
}
*/
String msgFmtStr = request.getParameter("msg");
if (msgFmtStr == null) {
    msgFmtStr = "Hello {0}!";
}
//msgFmtStr = escapeString(msgFmtStr);
/* String args[] = request.getParameterValues("arg");
if (args == null) {
    args = new String[]{"world","",""};
}
args = escapeStrings(args);
String argTypes[] = request.getParameterValues("argType");
if (argTypes == null) {
    argTypes = new String[]{"str","str","str"};
}
Object argObjs[] = convertToObjects(argTypes, args);
*/
 %>
 <% request.setAttribute("msgFmtStr", msgFmtStr); %>
<%@ include file="msganalyze.jsf" %>
 
