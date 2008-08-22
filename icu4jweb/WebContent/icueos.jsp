<%@ page language="java" contentType="text/html; charset=utf-8"
    pageEncoding="utf-8"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta content="Copyright (c) 2008-2008 IBM Corporation and others. All Rights Reserved." name="COPYRIGHT" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>ICU Release info</title>
</head>
<body>

<%@ include file="demolist.jsf"  %>


<%@ page import="com.ibm.icu.dev.demo.icu4jweb.*" %>

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
SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd");
String base=request.getContextPath() + request.getServletPath();
String uvers=request.getParameter("uvers");
String uproj=request.getParameter("uproj");
String uwhat=request.getParameter("uwhat");

//  Determine what to do, print top menu, etc.. 
%>



<h2><a href="<%= base %>">ICU Version and Platform Info</a></h2>



<% if(uproj == null ) {   // *** choose product %>
   <h3>Choose a project:</h3>
     <%
        for(String s : info.productList()) {
      %>
            <a href="<%= base %>?uproj=<%= s %>"><%= s %></a><br/>
      <%
            }
      %>
<% } else if (uvers == null) { %>
   <h3><%= uproj %></h3>
   <h4>choose a version:</h4>
<% IcuInfo.IcuProduct product = info.product(uproj);  %>

<%
	for(int i=0;i<prods.getLength();i++) {
		Node prod = prods.item(i);
		String type = XMLUtil.getAttributeValue(prod, "type");
		if(!type.equals(uproj)) continue; 
		NodeList names = XMLUtil.findChild(prod, "names").getChildNodes();
		NodeList rels = XMLUtil.findChild(prod, "releases").getChildNodes();
%>
		<h2><%= type %></h2>
		
		<%
			for(int k=0;k<names.getLength();k++) {
				Node n = names.item(k);
				if(n.getNodeType()!=Node.ELEMENT_NODE) continue ;
		%>
				<%= XMLUtil.getAttributeValue(n, "type")+ ": "+XMLUtil.getNodeValue(n)+"\n" %><br/>
		<%
			}
			//  now, releases.
		%>
		
		<table border=1>
			<thead>
				<tr>
					<th>version</th>
					<th>GA</th>
	<% if(showEOS) { %>				<th>EOS</th> <% } %>
				</tr>
			</thead>
			<tbody>
		<%
			for(int k=0;k<rels.getLength();k++) {
				Node n = rels.item(k);
				if(n.getNodeType()!=Node.ELEMENT_NODE) continue ;
				String version = XMLUtil.getAttributeValue(n, "version");
				NodeList dates = XMLUtil.findChild(n, "dates").getChildNodes();
				Date ga = null;
				Date eos = null;
				for(int q=0;q<dates.getLength();q++) {
					Node nn = dates.item(q);
					if(nn.getNodeType()!=Node.ELEMENT_NODE) continue ;
					String dtype = XMLUtil.getAttributeValue(nn, "type");
					String ddate = XMLUtil.getAttributeValue(nn, "date");
					if(!"ga".equals(dtype)) {
						out.println("unknown type " + dtype+" on " + nn.getNodeName()+ "<br/>");
						continue;
					}
					// calculate 6 year out
					c.clear();
					ga = sdf.parse(ddate);
					c.setTime(ga);
					c.add(Calendar.YEAR, 6);
					eos = c.getTime();
				}
				
				if(ga!=null && eos!=null) {
					String pastdue =  "";
					if( now.after(eos)) pastdue = " <font color='#FF0000' size='-1'>(past"+/*sdf.format(now)+" "+*/")</font>";
				%>
					<tr>
						<th><a href="<%= base + "?uproj=" + uproj + "&uvers="+version %>"><%= version %></a></th>
						<td><%= sdf.format(ga) %></td>
<% if(showEOS) { %>						<td><%= sdf.format(eos) + pastdue %></td>  <% } %>
					</tr>
				<%
				} else {
				%>
					<tr>
						<th><%= version %></th>
						<td></td>
<% if(showEOS) { %>                     <td></td>  <% } %>
					</tr>
				<%
				}
			}
		%>
			</tbody>
		</table>
		
		
		
<%
	}  // end loop
	
	} else if (uwhat == null) { 
%>

<%
	for(int i=0;i<prods.getLength();i++) {
		Node prod = prods.item(i);
		String type = XMLUtil.getAttributeValue(prod, "type");
		if(!type.equals(uproj)) continue; 
		NodeList names = XMLUtil.findChild(prod, "names").getChildNodes();
		NodeList rels = XMLUtil.findChild(prod, "releases").getChildNodes();
%>
		<h2><a href="<%= base + "?uproj=" + type %>"><%= type %></a></h2>
		
		<%
			for(int k=0;k<names.getLength();k++) {
				Node n = names.item(k);
				if(n.getNodeType()!=Node.ELEMENT_NODE) continue ;
		%>
				<%= XMLUtil.getAttributeValue(n, "type")+ ": "+XMLUtil.getNodeValue(n)+"\n" %><br/>
		<%
			}
			//  now, releases.
		%>

		<%
			for(int k=0;k<rels.getLength();k++) {
				Node n = rels.item(k);
				if(n.getNodeType()!=Node.ELEMENT_NODE) continue ;
				String version = XMLUtil.getAttributeValue(n, "version");
				if(!version.equals(uvers)) continue;
		%>
				<h4><%= uvers %></h4>

		<%
				NodeList dates = XMLUtil.findChild(n, "dates").getChildNodes();
				for(int q=0;q<dates.getLength();q++) {
					Date ga = null;
					Date eos = null;
					Node nn = dates.item(q);
					if(nn.getNodeType()!=Node.ELEMENT_NODE) continue ;
					String dtype = XMLUtil.getAttributeValue(nn, "type");
					String ddate = XMLUtil.getAttributeValue(nn, "date");
					if(!"ga".equals(dtype)) {
						out.println("unknown type " + dtype+" on " + nn.getNodeName()+ "<br/>");
						continue;
					}
					// calculate 6 year out
					c.clear();
					ga = sdf.parse(ddate);
					c.setTime(ga);
					c.add(Calendar.YEAR, 6);
					eos = c.getTime();
				
				if(ga!=null && eos!=null) {
					String pastdue =  "";
					if( now.after(eos)) pastdue = " <font color='#FF0000' size='-1'>(past"+/*sdf.format(now)+" "+*/")</font>";
				%>
					<%= dtype %>: <%= sdf.format(ga) %>, 

                        <% if(showEOS) { %> eos: <%= sdf.format(eos) + pastdue %> <% } %>
				<%
				} }
				%>		
				
				<h4>Platforms Supported:</h4>

                    <i>under construction</i>
		<%
			} // vers loop
		%>
		
   <% }  // prods loop %>

<%  }  // end (uvers == null) %>

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

