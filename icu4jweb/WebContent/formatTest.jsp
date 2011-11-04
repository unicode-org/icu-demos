<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<%@ page language="java"
 import="com.ibm.icu.util.*,com.ibm.icu.text.MessagePattern,com.ibm.icu.text.*,java.text.ParsePosition"
 contentType="text/html; charset=utf-8"
 pageEncoding="utf-8"%>
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
<head>
  <meta content="text/html; charset=utf-8" http-equiv="Content-Type" />
  <meta content="George Rhoten" name="AUTHOR" />
  <meta content="Copyright (c) 2008-2011 IBM Corporation and others. All Rights Reserved." name="COPYRIGHT" />
  <title>Format Tester</title>
  				<script type="text/javascript" src="dojo.js"></script>
 
<%@ include file="demohead.jsf" %>
<%!
static final String STR = "str";
static final String NUM = "num";
static final String DATE = "date";
static final String CHAR = "char";
static final String NUM_FMT = "0.#";
static final DecimalFormat decFmt = new DecimalFormat(NUM_FMT);
static final String DATE_FMT = "yyyy-MM-dd HH:mm:ss";
static final SimpleDateFormat dateFmt = new SimpleDateFormat(DATE_FMT);

static final Object convertToObject(String argType, String arg) {
    try {
        Object result = null;
        if (argType.equals(STR)) {
            return arg;
        }
        else if (argType.equals(NUM)) {
            result = decFmt.parse(arg, new ParsePosition(0));
        }
        else if (argType.equals(DATE)) {
            result = dateFmt.parse(arg, new ParsePosition(0));
        }
        else if (argType.equals(CHAR)) {
            result = new Character(arg.charAt(0));
        }
        else {
            return "Invalid Type " + argType;
        }
        if (result == null) {
            return "<span class='null'>null</span>";
        }
        else {
            return result;
        }
    }
    catch (Exception e) {
        return "[Can not parse \"" + arg + "\"]";
    }
}
static final Object[] convertToObjects(String[] argTypes, String[] args) {
    int argsLength = (args.length < argTypes.length ? args.length : argTypes.length);
    Object[] objs = new Object[argsLength];
    for (int argIdx = 0; argIdx < argsLength; argIdx++) {
        objs[argIdx] = convertToObject(argTypes[argIdx],args[argIdx]);
    }
    return objs;
}
static final String escapeString(String arg) {
    return arg.replaceAll("&", "&amp;").replaceAll("\"", "&quot;").replaceAll("<", "&lt;").replaceAll(">", "&gt;");
}
static final String[] escapeStrings(String[] args) {
    for (int argIdx = 0; argIdx < args.length; argIdx++) {
        args[argIdx] = escapeString(args[argIdx]);
    }
    return args;
}

/**
 * If we get something like 3.8.1.0_11, it becomes just "3.8.1".
 * If we get something like 4.0.0.0, it becomes just "4.0".
 */
static final String trimVersion(String ver) {
    int endIdx = ver.lastIndexOf('_') - 1;
    if (endIdx < 0) {
        endIdx = ver.length() - 1;
    }
    for (; endIdx >= 0; endIdx--) {
        char currChar = ver.charAt(endIdx);
        if (currChar != '0' && currChar != '.') {
            endIdx++;
            break;
        }
    }
    if (endIdx == 1) {
        int minorEndIdx = ver.indexOf('.', endIdx + 1);
        if (minorEndIdx > endIdx) {
            endIdx = minorEndIdx;
        }
    }
    return ver.substring(0, endIdx);
}

%><%
request.setCharacterEncoding("UTF-8");
response.setContentType("text/html;charset=utf-8");

String selectedLocale = request.getParameter("locale");
if (selectedLocale == null) {
    selectedLocale = request.getLocale().toString();
}
String msgType = request.getParameter("msgType");
if (msgType== null) {
    msgType="icu4j";
}
String msgFmtStr = request.getParameter("msg");
if (msgFmtStr == null) {
    msgFmtStr = "Hello {0}!";
}
//msgFmtStr = escapeString(msgFmtStr);
String args[] = request.getParameterValues("arg");
if (args == null) {
    args = new String[]{"world","",""};
}
args = escapeStrings(args);
String argTypes[] = request.getParameterValues("argType");
if (argTypes == null) {
    argTypes = new String[]{"str","str","str"};
}
Object argObjs[] = convertToObjects(argTypes, args);


%>
<script type="text/javascript">
//<![CDATA[
var argNum = <%= argTypes.length %>;
function addArgument() {
    // FireFox can't handle cell resizing. So we do it this way.
    var msgTypeNode = document.getElementById('msgType');
    var argumentsNode = document.getElementById('argumentsCell');
    var tableNode = argumentsNode.parentNode.parentNode;
    var rowNode = document.createElement('tr');
    rowNode.appendChild(document.createElement('th'));
    var cellNode = document.createElement('td');
    rowNode.appendChild(cellNode);
    var divNode = document.createElement('div');
    cellNode.appendChild(divNode);
    var rowLabel = (msgTypeNode.selectedIndex == 1) ? argNum+1 : argNum;
    divNode.innerHTML = '<label id="arg' + argNum + '" for="argVal' + argNum + '">' + rowLabel + '<\/label> <select id="argType' + argNum + '" name="argType" onchange="addExample(this,\'argVal' + argNum + '\');">'
        + '<option value="<%= STR %>">String<\/option>'
        + '<option value="<%= NUM %>">Number (<%= NUM_FMT %>)<\/option>'
        + '<option value="<%= DATE %>">Date (<%= DATE_FMT %>)<\/option>'
        + '<option value="<%= CHAR %>">Character<\/option>'
        + '<\/select>'
        + ' <input id="argVal' + argNum + '" type="text" size="24" name="arg" value="">';
    tableNode.appendChild(rowNode);
    argNum++;
}
function addExample(selectionBox,idToModify) {
    if (selectionBox.selectedIndex == 1) {//number
        document.getElementById(idToModify).value = '1234.56';
    }
    else if (selectionBox.selectedIndex == 2) {//date
        document.getElementById(idToModify).value = '1970-12-31 23:59:59';
    }
    else if (selectionBox.selectedIndex == 3) {//character
        document.getElementById(idToModify).value = 'A';
    }
    // else leave as string
}
function setArgumentBase(selectNode) {
	var msgAnalysis = dojo.byId("msgAnalysisSection");
    var baseNum = (selectNode.selectedIndex == 2) ? 1 : 0; // printf
    for (var argIdx = 0; argIdx < argNum; argIdx++) {
        var argNode = document.getElementById('arg' + argIdx);
        argNode.innerHTML = argIdx + baseNum;
    }
    if(selectNode.selectedIndex==0) {
    	msgAnalysis.style.display='';
    } else {
    	msgAnalysis.style.display='none';
    }
    
}

var myInterval  = -1;


function msgBlur( /* event */ e) {
	var msg = dojo.byId("msg");
	var msgAnalysis = dojo.byId("msgAnalysis");
	
	if(msgAnalysis != null) {
		//msgAnalysis.innerHTML = "<i>Updating...</i><hr>" + msgAnalysis.innerHTML;
		dojo.xhrGet( 
				{ 
					url:	'formatAnalyze.jsp',
					handleAs: 'text',
					content:  {
						"msg": msg.value
					},
					
					load: function(d) {
							msgAnalysis.innerHTML = d;
						},
					error:  function(e) {
								msgAnalysis.innerHTML = '<i class="null">'+e.toString()+'</i>';
								window.clearInterval(myInterval);
						}
			   });
	}
}

function msgFocus( /* event */ e) {
	
}


dojo.addOnLoad(function(){
	var ticks=0; // 1000;
	if(ticks>0) {
		myInterval = self.setInterval("msgBlur()",ticks);
	}
	//document.innerHTML("<h1>UPDATING EVERY "+ticks+" ms</h1>");
});

//]]>
</script>
</head>
<body style="margin: 0.5em">
<%@ include file="demolist.jsf" %>

<h2>Format Tester</h2>

<form action='<%= request.getRequestURI() %>' method="post">
<table class='formatTable'>
<tr><th><label for="msg">Message</label></th>
    <td><textarea  onchange="msgBlur()"  id="msg" name="msg" rows="2" cols="80"><%= escapeString(msgFmtStr) %></textarea></td></tr>

<tr><th><label for="msgType">Message Type</label></th>
    <td><select id="msgType" name="msgType" onchange="setArgumentBase(this);">
        <option value="icu4j"<%= (msgType.equals("icu4j")?" selected=\"selected\"":"") %>>ICU4J MessageFormat</option>
        <option value="java"<%= (msgType.equals("java")?" selected=\"selected\"":"") %>>Java MessageFormat</option>
        <option value="printf"<%= (msgType.equals("printf")?" selected=\"selected\"":"") %>>Java Formatter (printf)</option>
        </select></td></tr>

<tr><th><label for="locale">Locale</label></th>
    <td><select id="locale" name="locale"><%
ULocale[] locales = com.ibm.icu.text.NumberFormat.getAvailableULocales();
for (int locIdx = 0; locIdx < locales.length; locIdx++) {
	ULocale currULoc = locales[locIdx];
    String currLocStr = currULoc.toString();
    out.println("<option"+(selectedLocale.equals(currLocStr)?" selected=\"selected\"":"")+" value=\""+currLocStr+"\">" +currLocStr+" ["+currULoc.getDisplayName()+"]</option>");
}
%></select></td></tr>

<tr><th>Arguments<br/>
<button type="button" onclick="addArgument()">Add</button></th>
    <td id="argumentsCell"><%
int argsLength = (args.length < argTypes.length ? args.length : argTypes.length);
for (int argIdx = 0; argIdx < argsLength; argIdx++) {
    out.println("<div class='argument' ><label id=\"arg" + argIdx + "\" for=\"argVal" + argIdx + "\">" + (msgType.equals("printf")?argIdx+1:argIdx) + "</label> <select id=\"argType" + argIdx + "\" name=\"argType\" onchange=\"addExample(this,'argVal" + argIdx + "');\">");
    out.println(" <option value=\""+STR+"\""+(argTypes[argIdx].equals(STR)?" selected=\"selected\"":"")+">String</option>");
    out.println(" <option value=\""+NUM+"\""+(argTypes[argIdx].equals(NUM)?" selected=\"selected\"":"")+">Number ("+NUM_FMT+")</option>");
    out.println(" <option value=\""+DATE+"\""+(argTypes[argIdx].equals(DATE)?" selected=\"selected\"":"")+">Date ("+DATE_FMT+")</option>");
    out.println(" <option value=\""+CHAR+"\""+(argTypes[argIdx].equals(CHAR)?" selected=\"selected\"":"")+">Character</option>");
    out.println("</select>");
    out.println("<input id=\"argVal" + argIdx + "\" type=\"text\" size=\"24\" name=\"arg\" value=\"" + args[argIdx] + "\" /></div>");
    out.println("");
}
%></td></tr>

</table>

<p><input type="submit" value="Format" /></p>

</form>

<h3>Result</h3>
<pre id='result' class='result'><%
try {
    ULocale currULoc = new ULocale(selectedLocale);
    if (msgType.equals("icu4j")) {
        out.print((new com.ibm.icu.text.MessageFormat(msgFmtStr,currULoc)).format(argObjs));
    }
    else if (msgType.equals("printf")) {
        out.print((new Formatter(currULoc.toLocale())).format(currULoc.toLocale(), msgFmtStr, argObjs));
    }
    else {
        out.print((new java.text.MessageFormat(msgFmtStr,currULoc.toLocale())).format(argObjs));
    }
}
catch (Exception e) {
    out.print("<span class='null'>"+e.getMessage()+"</span>");
}
%></pre>

<div id='msgAnalysisSection'>
<% if(msgType.equals("icu4j")) { %>
<h3>Pattern Analysis</h3>
<% request.setAttribute("msgFmtStr", msgFmtStr); %>
<%@ include file="msganalyze.jsf" %>
<%
} /* end Pattern Analysis */
%>
</div>

<p class='poweredByIcu'>Powered by
<a href="http://www.icu-project.org/">ICU</a> <%= trimVersion(com.ibm.icu.util.VersionInfo.ICU_VERSION.toString()) %> and
<%= System.getProperty("java.vendor") %> <a href="http://java.sun.com/">Java</a> <%= trimVersion(System.getProperty("java.version")) %></p>
<!--  jsp:include page="/ssi/footer.fragment" -->
</body>
</html>
