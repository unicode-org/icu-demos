<!-- Copyright (c) 2008 IBM. All Rights Reserved -->
<html>
<%@ page contentType="text/html; charset=utf-8" import="com.ibm.icu.text.Bidi" %>
<%@ page import="com.ibm.icu.util.*" %>
<%@ page import="java.util.Date" %>
<%@ page import="com.ibm.icu.text.*" %>
<%@ page import="com.ibm.icu.text.DateFormat" %>
<%@ page import="java.text.AttributedCharacterIterator"  %>
<%@ page import="com.ibm.icu.text.DateTimePatternGenerator"  %>
<%@ page import="java.text.CharacterIterator"  %>
<%@ page import="java.util.Random" %>

<head>
<title>ICU Patina Demo</title>


 </head>

<body>
    <style type="text/css">
            p { color: "white"; background-color: "navy"; padding: 2px; }
    </style>


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
	if(skel==null) skel= "";
	
	Date ta = new Date();
	//Date tb = new Date();
	
    try {
        ta = new Date(request.getParameter("ta"));
    } catch(Throwable t) {
        out.println(t.toString());
        ta = new Date();
    }
    /*
    try {
        tb = new Date(request.getParameter("tb"));
    } catch(Throwable t) {
        out.println(t.toString());
        tb = new Date();
    }
    */

    
    SimpleDateFormat  dtf =  (SimpleDateFormat)DateFormat.getInstance(Calendar.getInstance(to),to);

    if(skel.length()>0) {
        dtf.applyPattern(skel);
    } else {
        skel = dtf.toPattern();
    }

    String formatted = dtf.format(ta);

    AttributedCharacterIterator aci = dtf.formatToCharacterIterator(ta);

    
%>
<h2>Patina: Pattern manipulation</h2>
<a href="http://en.wikipedia.org/wiki/Japanese_mon_(currency)"><img width="100" src="patina.jpg" alt="1 Mon coin, Japan, 1859" title="1 Mon coin, Japan, 1859" border=0 align="right"></a>

<div style='font-size: xx-large;'>

<form action="<%= request.getContextPath() + request.getServletPath() %>" method="POST"><br>
<label>pattern: <input style='font-size: large;' name='skel' value='<%= skel %>'></label><br>
<label>A: <input size=50 style='font-size: large;' name='ta' value='<%= ta %>'></label><br>
<label>To: <input style='font-size: large;' name='to' value='<%= to.getName() %>'></label><br>
<input style='font-size: xx-large;' type=submit>
</form>

<hr>

<hr>


<b><%= formatted %></b><br>
locale: <%= to.getDisplayName() %><br>
ta: <%= ta.toString() %><br>

<hr>
<% 
DateTimePatternGenerator dtpg = DateTimePatternGenerator.getInstance(to);

for(char c = aci.first();c!=CharacterIterator.DONE;c=aci.next()) { 
    int af = 5;

    java.util.Map s = aci.getAttributes();  %>
    <!--  %= c % : -->
    <% if(s==null || s.isEmpty()) {
        %><%= c %><% } else {
       for ( Object k :  s.keySet() ) {
        DateFormat.Field f = (DateFormat.Field)k;
        int cf = f.getCalendarField();
        String str = com.ibm.icu.dev.test.util.DebugUtilities.enumString(
                com.ibm.icu.dev.test.util.DebugUtilitiesData.UCalendarDateFields, cf);
       %><span class='field'><%= str %></span><% } 
       int end = aci.getRunLimit();
       aci.setIndex(end-1);
     }
    %>
    
<% } %>
<hr>

</div>
<hr>
<i>based on <a href="http://icu-project.org">ICU4J <%= com.ibm.icu.util.VersionInfo.ICU_VERSION %></i>
</body>
</html>