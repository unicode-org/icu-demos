<%@ page language="java" contentType="text/html; charset=utf-8"
    pageEncoding="utf-8"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta content="Copyright (c) 2008-2008 IBM Corporation and others. All Rights Reserved." name="COPYRIGHT" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>ICU Release info</title>
 <STYLE type="text/css">
    .hilite {  background: yellow; }
 </style>
</head>
<body>

<%@ include file="demolist.jsf"  %>


<%@ page import="com.ibm.icu.dev.demo.icu4jweb.*" %>
<%@ page import="com.ibm.icu.text.SimpleDateFormat" %>
<%@ page import="com.ibm.icu.util.ULocale" %>
<%@ page import="com.ibm.icu.lang.UCharacter" %>
<%@ page import="com.ibm.icu.text.DurationFormat" %>

<%@ page import="java.io.*" %>
<%@ page import="java.util.*" %>
<%@ page import="java.text.*" %>
<%@ page import="javax.xml.parsers.*" %>
<%@ page import="javax.xml.xpath.*" %>
<%@ page import="org.w3c.dom.*" %>
<%@ page contentType="text/html; charset=utf-8" %>

<%!

   public static boolean showEOS = false;

	public static String FILE = "xml/icuinfo.xml";
	public static String kURL = "http://icu-project.org/"+FILE;
	public static String SRL_PATH = "/xsrl/IP/htdocs/"+FILE;




%>

<%
	 Document sdoc = null; // cached
	 Date swhen = null;
	 String swhere = "nothing";


// LOAD
//ServletContext sc = getServletConfig().getServletContext();
ServletContext sc = getServletConfig().getServletContext();
sdoc = (Document)sc.getAttribute("sdoc");
swhen = (Date)sc.getAttribute("swhen");
swhere = (String)sc.getAttribute("swhere");



Date now = new Date(System.currentTimeMillis());
String source = "none";

DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
 
Document doc = null;
Date when = null;

File xmlFile = new File(SRL_PATH);


if(sdoc != null) {
	if(xmlFile.exists()) {
		if(new Date(xmlFile.lastModified()).after(swhen)) {
			sdoc = null; // clear cache
		}
	} else {
		if((now.getTime()-swhen.getTime()) > 1800*1000) { /* 30 minutes */
			sdoc = null;
			// check URL date if more than a few mins old
		}
	}	
}

if(sdoc != null) {
	when = swhen;
	source = swhere;
	doc = sdoc; // load from cache
}

DocumentBuilder docBuilder = factory.newDocumentBuilder();
docBuilder.setEntityResolver(new CachingEntityResolver());

if(sdoc == null) {
	if(xmlFile.exists()) {
		doc = docBuilder.parse(xmlFile);
		source = "Local file on SRL's disk";
		when = new Date(xmlFile.lastModified());
	//	when = now;
	} else {
		doc = docBuilder.parse(kURL);
		source = kURL;
		when = now;
	}

	if(sdoc==null && doc!=null) { sdoc = doc; swhen = now; swhere = "Cache of " + source; } // should be idempotent
}
 
IcuInfo info = new IcuInfo(doc);

Element el = doc.getDocumentElement();

NodeList prods = doc.getElementsByTagName("icuProduct");

Calendar c = Calendar.getInstance();
java.text.SimpleDateFormat sdf = new java.text.SimpleDateFormat("yyyy-MM-dd");
DurationFormat df = DurationFormat.getInstance(new ULocale("en_US"));
String base=request.getContextPath() + request.getServletPath();
String uvers=request.getParameter("uvers");
String uproj=request.getParameter("uproj");
String uwhat=request.getParameter("uwhat");

//  Determine what to do, print top menu, etc.. 
%>



<h2><a href="<%= base %>">ICU Version and Platform Info</a></h2>


<% if("match".equals(uwhat)) {
       String uattr = request.getParameter("uattr");
       String uval = request.getParameter("uval");
       
       out.write("<h2>Searching for " + uattr + " = "+uval + "</h2>\n");
       
       for(IcuInfo.IcuProduct product : info) {
           out.write("<h3>Searching "+product.fullName()+" ("+product.name()+")"+"...</h3>\n");
           
           for(IcuInfo.Release release : product ) {
               Set<IcuInfo.Platform> matching = release.supportsPlatform(uattr,uval,IcuInfo.Frequency.OFTEN);
               if(!matching.isEmpty()) {
                   out.write("<h2><a href=\""+ base + "?uproj=" + product.name() + "&uvers="+release.name() + "\">"+product.name() + " " + release.name() +"</a></h2>\n");
                   out.write("<blockquote>");
                    %>
<table border=1>
<thead>
    <tr>
        <th>Operating System</th>
        <th>Toolset</th>
        <th>Testing frequency</th>
    </tr>
</thead>
<tbody>
            <% for(IcuInfo.Platform plat : matching) { %>
<tr>
    <td><%= (plat.os + " " + plat.os_version + " ("+plat.arch+", "+
                plat.bits+"-bit) </td>" + "<td>" +  plat.tool + " " + plat.tool_version).replaceAll(uval,"<span class='hilite'>"+uval+"</span>")  %></td>
    <td><%= UCharacter.toTitleCase(plat.tested().toString(), null)  %></td>
</tr>
			<% } %>
</tbody>
</table>

<%
                   out.write("</blockquote>");
               }  // end if matching not empty
           } // end loop release
       } // end loop prods
 } else if("all".equals(uwhat)) { 
    Map<String,Set<String>> allAttributes = info.allAttributes(); 
 %>
 <h4>All Attribute Info</h4>

    <% for(String s : IcuInfo.sortedKeys(allAttributes)) { %>
            <%= s %><br/>
            <blockquote>
                <% for(String q : allAttributes.get(s)) {
                    if(s.equals("frequency")||s.equals("name")||s.equals("owner")) {
                        out.println(q + "<br>\n");
                    } else {
                    %>
                    <a href="<%= base + "?uwhat=match&uattr="+java.net.URLEncoder.encode(s, "UTF-8")+"&uval="+java.net.URLEncoder.encode(q, "UTF-8") %>"><%= q %></a><br/>
                <%  }
                 }  %>
            </blockquote>
    <% } %>

<% } else if(uproj == null ) {   // *** choose product %>
   <h3>Choose a project:</h3>
     <%
        for(String s : info.productList()) {
      %>
            <a href="<%= base %>?uproj=<%= s %>"><%= info.product(s).shortName() + " ("+s+")" %></a><br/>
      <%
            }
      %>
    <hr>
    <h3>Utilities</h3>
       <a href="<%= base %>?uwhat=all">Search by Attributes</a><br/>

<% } else if (uvers == null) { %>
<% IcuInfo.IcuProduct product = info.product(uproj);  %>
   <h3><%= product.fullName() + " ("+product.shortName()+")" %></h3>
   <h4>choose a version:</h4>

        <table border=1>
            <thead>
                <tr>
                    <th>version</th>
                    <th>GA</th>
                </tr>
            </thead>
            <tbody>

<%
    for(String ver : product.releaseList()) {
        IcuInfo.Release r = product.release(ver);
%>
                    <tr>
                        <th><a href="<%= base + "?uproj=" + uproj + "&uvers="+ver %>"><%= r.name() %></a></th>
                        <td><%= r.getDate() %> : <%= df.formatDurationFromNow(sdf.parse(r.getDate()).getTime()-now.getTime()) %>
                            <%= r.draft()!=null?"<br><b>("+r.draft()+")</b>":"" %>
                            </td>
                    </tr>
      
<%    } %>
            </tbody>
        </table>

		
		
<%
	
	} else if (uwhat == null) { 
%>
<% IcuInfo.IcuProduct product = info.product(uproj); 
   IcuInfo.Release    release = product.release(uvers);
%>
   <h3><a href="<%= base + "?uproj=" + uproj %>"><%= product.fullName() + " ("+product.shortName()+")" %></a></h3>
   <h4><%= release.name() %></h4>
    <b>Release GA Date</b> <%= release.getDate() %>
                            <%= release.draft()!=null?" <b>("+release.draft()+")</b>":"" %> -
        <%= df.formatDurationFromNow(sdf.parse(release.getDate()).getTime()-now.getTime()) %>

        <% if (release.platformList().isEmpty()) { %> <br> <i style='font-size: x-large;'>Note: Not all data has been entered into this new XML format. </i> <% } %>
        <br><br><b>Platform info:</b><br/>


         <table border=1>
            <thead>
                <tr>
                    <th>Operating System</th>
                    <th>Toolset</th>
                    <th>Testing frequency</th>
                </tr>
            </thead>
            <tbody>
        <% for(IcuInfo.Platform plat : release.platformsBySupport()) { %>
            <tr>
                <td><%= plat.os + " " + plat.os_version + " ("+plat.arch+", "+
                            plat.bits+"-bit)" %></td>
                <td><%= plat.tool + " " + plat.tool_version  %></td>
                <td><%= UCharacter.toTitleCase(plat.tested().toString(), null)  %></td>
            </tr>
        <% } %>
         </tbody>
        </table>
<%  }  // end (uwhat == null) %>

<hr />

	Source:
	<%= source %>, Dated: <%= when!=null?when.toString():"(null date)"  %>
<br/>
Generated: <%= sdf.format(now) %>
<br/>



<%
// SAVE
if(sdoc != null) 
	sc.setAttribute("sdoc", sdoc);
if(swhen != null) 
	sc.setAttribute("swhen", swhen);
if(swhere != null) 
	sc.setAttribute("swhere", swhere);

%>

</body>
</html>

