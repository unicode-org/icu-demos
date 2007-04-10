/*
 ******************************************************************************
 * Copyright (C) 2007-2007, International Business Machines Corporation and   *
 * others. All Rights Reserved.                                               *
 ******************************************************************************
 */
package com.ibm.icu.datacustom;

import java.io.*;
import java.util.*;

// logging
import java.util.logging.*;

// servlet imports
import javax.servlet.*;
import javax.servlet.http.*;

// DOM imports
import org.w3c.dom.*;
import org.xml.sax.SAXException;

import javax.xml.parsers.*;
import javax.xml.validation.Schema;
import javax.xml.validation.SchemaFactory;
import javax.xml.*;

/**
 * The main servlet class of the Data Custimization tool
 */
public class DataCustomizer extends HttpServlet {

    // Logging
    public static Logger logger = Logger.getLogger("com.ibm.icu.DataCustomizer");
    //private DocumentBuilderFactory docBuilder = DocumentBuilderFactory.newInstance();

    /** status * */
    public static String icuDataHome = null;

    ServletConfig config = null;
    DocumentBuilder xmlDocBuilder = null;

    public final void init(final ServletConfig config) throws ServletException {
        super.init(config);

        icuDataHome = config.getInitParameter("ICU_DATA.home");
        this.config = config;
        doStartup();
    }

    public DataCustomizer() {
        //docBuilder.setSchema();
    }

    /**
     * output MIME header, build context, and run code..
     */
    public void doPost(HttpServletRequest request, HttpServletResponse response)
            throws IOException, ServletException
    {
        response.setHeader("Cache-Control", "no-cache");
        response.setDateHeader("Expires", 0);
        response.setHeader("Pragma", "no-cache");
        response.setDateHeader("Max-Age", 0);

        // rest of these are XML
        response.setContentType("text/xml; charset=utf-8");

        // Parse the request.
//        SchemaFactory factory = SchemaFactory.newInstance(XMLConstants.W3C_XML_SCHEMA_NS_URI);
//        StreamSource ss = new StreamSource("mySchema.xsd");
//        Schema schema = factory.newSchema(ss);
//        SAXParserFactory sf = SAXParserFactory.newInstance();
//        sf.setNamespaceAware(true); 
//        sf.setValidating(true);            
//        SAXParser sp = sf.newSAXParser();
//        sp.setProperty(SCHEMA_LANGUAGE, XMLConstants.W3C_XML_SCHEMA_NS_URI);
//        sp.setProperty(SCHEMA_SOURCE, schema);
//        sp.parse(request.getInputStream(), dh);
        
        PrintWriter writer = response.getWriter();
        if (request.getContentType().startsWith("text/xml; ")) {
            try {
                Document xmlDoc = xmlDocBuilder.parse(request.getInputStream());
                NodeList pkgItems = xmlDoc.getElementsByTagName("item");
                int itemsLen = pkgItems.getLength();
                for (int itemIdx = 0; itemIdx < itemsLen; itemIdx++) {
                    String pkgItemName = pkgItems.item(itemIdx).getFirstChild().getNodeValue();
                    writer.println(pkgItemName + ",");
                }
            }
            catch (SAXException e) {
                logger.warning(e.getMessage());
                // TODO: Handle Error
            }
        }
        else {
            dumpRequest(request, writer);
        }
    }

//  public void doGet(HttpServletRequest request, HttpServletResponse response)
//          throws IOException, ServletException
//  {
//      doPost(request, response);
//  }
    
    private String convertBufferedReaderToString(BufferedReader reader)
        throws IOException
    {
        StringBuffer contentBuf = new StringBuffer();
        String line;
        while ((line = reader.readLine()) != null) {
            contentBuf.append(line);
        }
        return contentBuf.toString();
    }

    private void dumpRequest(HttpServletRequest request, PrintWriter writer) 
        throws IOException
    {
        writer.println("<html>");
        writer.println("<head>");
        writer.println("<title>Data Request Dump</title>");
        writer.println("</head>");
        writer.println("<body>");

        writer.println("<table border=\"0\">");
        writer.println("<tr>");
        writer.println("<td>");
        writer.println("<h1>Data Request Dump</h1>");
        writer.println("<p>");
        writer.println("The following is diagnostic information of your request.");
        writer.println("</p>");
        writer.println("</td>");
        writer.println("</tr>");
        writer.println("</table>");

        writer.println("<table border=\"1\">");
        writer.println("<caption>Header Information</caption>");
        Enumeration names = request.getHeaderNames();
        while (names.hasMoreElements()) {
            String name = (String) names.nextElement();
            writer.println("<tr>");
            writer.println("  <th align=\"right\">" + name + ":</th>");
            writer.println("  <td>" + request.getHeader(name) + "</td>");
            writer.println("</tr>");
        }
        writer.println("</table>");

        writer.println("<table border=\"1\">");
        writer.println("<caption>Request Information</caption>");
        HttpSession session = request.getSession();
        writer.println("<tr><td>Session ID</td><td>" + session.getId() + "</td></tr>");
        writer.println("</table>");

        writer.println("<table border=\"1\">");
        writer.println("<caption>Parameter Information</caption>");
        Enumeration paramNames = request.getParameterNames();
        while (paramNames.hasMoreElements()) {
            String name = (String) paramNames.nextElement();
            writer.println("<tr>");
            writer.println("  <th align=\"right\">" + name + ":</th>");
            writer.println("  <td>");
            String[] values = request.getParameterValues(name);
            for (int i = 0; i < values.length; i++) {
                writer.println(Integer.toString(i) + " " + values[i] + "<br />");
            }
            writer.println("</td>");
            writer.println("</tr>");
        }
        writer.println("</table>");
        writer.println("<samp><pre>");
        writer.println(convertBufferedReaderToString(request.getReader()));
        writer.println("</pre></samp>");
        writer.println("</body>");
        writer.println("</html>");
    }

    /**
     * Main setup
     */
    static boolean isSetup = false;

    public synchronized void doStartup() throws ServletException {
        if (isSetup == true) {
            return;
        }

        logger.info("Starting ICU Data Customization tool. Memory in use: "
                + usedKB() + "KB");
        isSetup = true;
        try {
            xmlDocBuilder = DocumentBuilderFactory.newInstance().newDocumentBuilder();
            // TODO: Set schema validator
        }
        catch (ParserConfigurationException e) {
            logger.warning(e.getMessage());
        }

        logger.info("ICU Data Customization tool ready for requests. Memory in use: "
                        + usedKB() + "KB");
    }

    public void destroy() {
        logger.warning("ICU Data Customization tool shutting down.");
        super.destroy();
    }

    // ====== Utility Functions
    public static int usedKB() {
        Runtime r = Runtime.getRuntime();
        double total = r.totalMemory();
        total = total / 1024;
        double free = r.freeMemory();
        free = free / 1024;
        return (int) (Math.floor(total - free));
    }
}
