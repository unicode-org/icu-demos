<html>
	<head><title>Candidate List Demo</title>
<%@ include file="demohead.jsf" %>
  <meta content="Copyright (c) 2008-2008 IBM Corporation and others. All Rights Reserved." name="COPYRIGHT" />
</head>

	<body>
<%@ include file="demolist.jsf"  %>
		<form method="post" action="testLocale.jsp">
			<b>Enter Locale:</b>
 			<input type="text" size="20" name="locale" /><br/>
 			<input type="submit" name="B1" value="Submit" />
            			
		</form>
		<%@page import="java.util.ArrayList"%>
		<%
			if(request.getParameter("locale") != null){
				String s[] = null;			
				String jdk[]=null;
				String baseLocale=null;
				String extension=null;
				if(request.getParameter("locale").equals(""))
					out.println("<html><font color=red>Please enter locale.</font></html>");
				else{
					String a = request.getParameter("locale");
					System.out.println(a);
					if(a.contains("@")){
						baseLocale = a.substring(0, a.indexOf("@")-1);
						extension = a.substring(a.indexOf("@"));
					}
					s = Test.testLocale.getCandidateList(a);
					jdk = Test.testLocale.getJDKLocales(a);
					ArrayList al = new ArrayList(jdk.length);
					for (int j=0;j<jdk.length;j++) {
    						al.add(jdk[j]);
					}
                                        boolean isExtension = false;
					for (int j=0;j<s.length;j++) {
    					if(s[j].equals("@")){
    						isExtension = true;
    					}	
    					
					} 
					out.println("<b>Base Locale is:</b> " + s[0]+"<br/>");
					if(extension!=null){
						out.println("<b>Base Extension is:</b> " + extension+"<br/>");
					}
					out.println("<b>Candidate Locale are:</b>");
					if(isExtension){
						for(int i=0;i<s.length-2;i++){
							out.println("<br/>");
							if(al.contains(s[i])){
								out.println(s[i]);
					    	}else {
					    		out.println("<font color="+"#00FFFF"+">"+s[i]+"</font>");
					   		 }
					   	}
					out.println("<br/><b>Extension is :<b/> "+"<font color="+"#00FFFF"+"> @"+s[s.length-1]+"</font>");
					}else{
						for(int i=0;i<s.length;i++){
							out.println("<br/>");
							if(al.contains(s[i])){
								out.println(s[i]);
					    	}else {
					    		out.println("<font color="+"#00FFFF"+"> "+s[i]+"</font>");
					    	}
					    
						}
						out.println("<br/>Extension is : "+"<font color="+"#00FFFF"+"> No Extensions</font>");
					}
					
				}
					
			}
		%>
	</body>
</html>
