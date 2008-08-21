<%@ page import="java.io.*" %>
<%@ page import="java.util.*" %>
  <!--  "Copyright (c) 2008-2008 IBM Corporation and others. All Rights Reserved." name="COPYRIGHT"  -->

<%!
	String getSize(File f) {
				double size = f.length();
				double kb = size / 1024.0;
				double mb = size / 1048576; // 1000000 ; // 1048576 // kb / 1000.0;
				double mb1 = ((int)java.lang.Math.round(mb*10.0))/10.0;
				double kb1 = ((int)java.lang.Math.round(kb*10.0))/10.0;
				double b1 = ((int)java.lang.Math.round(size*10.0))/10.0;
				if(kb1  < 1.0) {
					return ""+(int)java.lang.Math.round(b1) + " B";
				} else if(mb1 < 1.0) {
					return ""+(int)java.lang.Math.round(kb) + " KB";
				} else {
					return ""+mb1 + " MB";
				}
	}
	
	String getDesc(File f) {
		String s = f.getName();
				String desc = "_____";
				
				if(s.endsWith(".md5")) {
					desc = "MD5 hash";
				} else if(s.endsWith("-src.zip")) {
					desc = "ZIP file for Windows platforms";
				} else if(s.endsWith("-src.tgz")) {
					desc = "gzipped tar archive for Unix and other platforms";
				} else if(s.endsWith("-docs.zip")) {
					desc = "ZIP file with the API documentation";
				} else if(s.endsWith("-userguide.zip")) {
					desc = "ZIP file with the User Guide documentation";
				} else if(0 < s.indexOf("AIX6_1")) {
					desc = "AIX 5.3 (PowerPC 64-bit) VisualAge 8";
				} else if(0 < s.indexOf("HPUX")) {
					desc = "HP-UX 11i v2 (B.11.23) (Itanium 64-bit) aCC A.06.10";
				} else if(0 < s.indexOf("RHEL")) {
					desc = "Red Hat Enterprise Linux 5 (x86 32-bit) gcc 4.1.1";
				} else if(0 < s.indexOf("Solaris")) {
					desc = "Solaris 10 (Sparc 64-bit) Forte 11";
				} else if(0 < s.indexOf("Win32")) {
					desc = "Windows (win32 x86 32-bit) Visual Studio 2005 (MSVC 8.0)";
				} else if(s.endsWith("-src.jar")) {
					desc = "Jar file containing the sources.";
				} else if(s.endsWith("-docs.jar")) {
					desc = "Jar file containing the documentation.";
				} else if(s.startsWith("icu4j-charsets")) {
					desc = "Jar file containing binaries for charsets.";
				} else if(s.startsWith("icu4j-localespi")) {
					desc = "Jar file containing binaries for Locale SPI.";
				} else if(s.endsWith(".jar")) {
					desc = "Jar file containing binaries.";
				}
		return desc;				
	}

%>

<%
	String subbase = request.getParameter("base");
	 if(subbase == null) subbase = "c";
	String 	which = "c";
	if(subbase.startsWith("j")) {
		which = "j";
	}
   String base = "/xsrl/40d2/" + subbase;
	File fbase = new File(base);
	Set<String> files = new TreeSet<String>();
	
	File[] subfiles = fbase.listFiles();
	
	for(File f : subfiles) {
		if(f.getName().equals(".DS_Store")) { continue; }
		files.add(f.getName());
	}
%>

<h1><%= subbase %></h1>


		<table summary="Available files to download" border="0">
			<tr>
				<th class="release-line">File</th>
				<th class="release-line">Size</th>
				<th class="release-line">Description</th>
			</tr><% for ( String s : files ) { %>
			<tr><% File f = new File(fbase, s);
					String mb1 = getSize(f);
					String desc = getDesc(f);
					String schemehost = "http://download";
					String icon="";
					if(s.endsWith(".md5")) {
					 	schemehost = "https://ssl";
						icon="<img width=16 height=16 src='https://ssl.icu-project.org/images/SSL_Symbol.png' border=0>";
					}
			%>
				<td class="release-line"><a href="<%= schemehost %>.icu-project.org/files/icu4<%= which %>/4.0/<%= s %>"><%= s %><%= icon %></a>
				</td>
				<td class="release-line"><%= mb1 %></td>
				<td class="release-line"><%= desc %></td>
			</tr> <%  } %>
		</table>



