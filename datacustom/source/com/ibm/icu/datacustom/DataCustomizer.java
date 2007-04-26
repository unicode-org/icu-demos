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

import javax.xml.XMLConstants;
import javax.xml.parsers.*;
import javax.xml.transform.Source;
import javax.xml.transform.stream.StreamSource;
import javax.xml.validation.*;

/**
 * The main servlet class of the Data Custimization tool
 */
public class DataCustomizer extends HttpServlet {

    // TODO: Add comment to manifest and .dat file about source of customizations. Add .res file with meta package information.
    // TODO: Update res_index.* files. Put into servlet code instead of icupkg tool for now.
    // TODO: Clean up res_index files after generation.
    // TODO: Log a summary of results. Store full xml files in a cache directory.
    // TODO: Cache big-endian results for ICU4J.
    // TODO: Make the path to the ICU tools a property.
    // TODO: Look into more efficient buffering of data files sent to user.
    // TODO: Use exceptions instead of if statements to detect if execution should continue.
    // TODO: Check for file uniqueness when generating files for res_index or icupkg.
    
    // Logging
    public static Logger logger = Logger.getLogger("com.ibm.icu.DataCustomizer");
    
    public static final String NEWLINE = System.getProperty("line.separator");
    public static final String ENDIAN_STR = "l"; // TODO: Fix this endianness

    /** status * */
    public static String toolHome;
    public static String toolHomeSrcDirStr;
    public static String toolHomeRequestDirStr;
    private static boolean DEBUG_FILES = false;
    
    private enum ICUDataFormat {
        ICU4C, ICU4J
    }

    ServletConfig config;
    DocumentBuilderFactory docBuilderFactory;

    public final void init(final ServletConfig config) throws ServletException {
        super.init(config);

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
        String sessionID = request.getSession().getId();
        applyNoCache(response);

        // Parse the request.
        //PrintWriter writer = response.getWriter();
        // You can't use getInputStream and getReader
        //dumpRequest(request, writer);
        String contentType = request.getContentType();
        if (contentType != null && contentType.startsWith("text/xml; ")) {
            String filesToPackage = "";
            Vector filesToPackageVect = new Vector();
            Vector indexesToGenerateVect = new Vector();
            Vector localesToIndexVect = new Vector();
            String icuDataVersion;
            ICUDataFormat icuDataType;
            File sessionDir = new File(toolHomeRequestDirStr + sessionID + File.separator);
            if (sessionDir.exists()) {
                String msg = sessionID + " directory already exists.";
                logger.warning(msg);
                // TODO: What happens when a user submits a request twice and we haven't cleaned up?
                //response.sendError(HttpServletResponse.SC_INTERNAL_SERVER_ERROR, msg);
                //return;
            }
            if (!sessionDir.mkdir()) {
                String msg = sessionID + " directory could not be created.";
                logger.warning(msg);
                // TODO: What happens when we run out space or it already exists?
                //response.sendError(HttpServletResponse.SC_INTERNAL_SERVER_ERROR, msg);
                //return;
            }
            try {
                Document xmlDoc = getDocumentBuilder().parse(request.getInputStream());
                NodeList pkgItems = xmlDoc.getElementsByTagName("item");
                icuDataVersion = xmlDoc.getElementsByTagName("version").item(0).getFirstChild().getNodeValue();
                if (xmlDoc.getElementsByTagName("target").item(0).getFirstChild().getNodeValue().equals("ICU4C")) {
                    icuDataType = ICUDataFormat.ICU4C;
                }
                else {
                    icuDataType = ICUDataFormat.ICU4J;
                }
                String baseDataName = "icudt" + icuDataVersion + ENDIAN_STR;
                int itemsLen = pkgItems.getLength();
                for (int itemIdx = 0; itemIdx < itemsLen; itemIdx++) {
                    Node pkgItemNode = pkgItems.item(itemIdx);
                    String pkgItemName = pkgItemNode.getFirstChild().getNodeValue();
                    // The schema should have already taken care of the file pattern authentication. 
                    File fileToRead = new File(toolHomeSrcDirStr + baseDataName + File.separator + pkgItemName);
                    if (!fileToRead.exists() || !fileToRead.canRead()) {
                        reportError(response, HttpServletResponse.SC_NOT_FOUND, pkgItemName + " was not found.");
                        return;
                    }
                    if (icuDataType == ICUDataFormat.ICU4C) {
                        filesToPackage += pkgItemName + NEWLINE;
                    }
                    else {
                        filesToPackageVect.add(pkgItemName);
                    }
                    if (pkgItemName.endsWith(".res")) {
                        if (pkgItemName.endsWith("res_index.res")) {
                            indexesToGenerateVect.add(pkgItemName);
                        }
                        else {
                            String hiddenVal = "0";
                            NamedNodeMap attNodeMap = pkgItemNode.getAttributes();
                            if (attNodeMap != null) {
                                Node hiddenNode = attNodeMap.getNamedItem("hidden");
                                if (hiddenNode != null) {
                                    String newHiddenVal = hiddenNode.getNodeValue();
                                    if (newHiddenVal != null) {
                                        hiddenVal = newHiddenVal;
                                    }
                                }
                            }
                            if (hiddenVal.equals("0")) {
                                localesToIndexVect.add(pkgItemName);
                            }
                        }
                    }
                }
            }
            catch (ParserConfigurationException e) {
                reportError(response, HttpServletResponse.SC_INTERNAL_SERVER_ERROR, e.getMessage());
                return;
            }
            catch (SAXException e) {
                reportError(response, HttpServletResponse.SC_BAD_REQUEST, e.getMessage());
                return;
            }
            // TODO: Clean up after ourselves in case of a problem.
            //sessionDir.deleteOnExit();
            if (!regenerateResIndexFiles(response, indexesToGenerateVect, localesToIndexVect, sessionDir)) {
                return;  /* Something bad happened during the generation. */
            }
            if (icuDataType == ICUDataFormat.ICU4C) {
                generateICU4CData(request, response, filesToPackage, indexesToGenerateVect, icuDataVersion, sessionID, sessionDir);
                // Don't do anything after this. The request may or may not have failed.
            }
            else {
                generateICU4JData(request, response, filesToPackageVect, indexesToGenerateVect, icuDataVersion, sessionID, sessionDir);
                // Don't do anything after this. The request may or may not have failed.
            }
            deleteResIndexFiles(indexesToGenerateVect, sessionDir);
        }
        else {
            PrintWriter writer = response.getWriter();
            dumpRequest(request, writer);
        }

        //response.setContentType("text/xml; charset=utf-8");
    }

    private void generateICU4CData(HttpServletRequest request, HttpServletResponse response, String filesToPackage, Vector generatedIndexesVect, String icuDataVersion, String sessionID, File sessionDir)
        throws IOException
    {
        String sessionDirStr = sessionDir.getAbsolutePath() + File.separator;
        String packageList = null;
        try {
            // Hmm. Did the person submit the request twice? There is a conflict. Lock out this request.
            packageList = sessionDirStr + "package.lst";
            if ((new File(packageList)).exists()) {
                reportError(response, HttpServletResponse.SC_CONFLICT, "Please finish the download of your existing request before creating a new request.");
                return;
            }
            PrintWriter dataToPackageWriter = new PrintWriter(packageList);
            dataToPackageWriter.print(filesToPackage);
            dataToPackageWriter.close();
        }
        catch (IOException e) {
            reportError(response, HttpServletResponse.SC_INTERNAL_SERVER_ERROR, "Could not write package list in " + sessionDir.getAbsolutePath());
            return;
        }
        String baseDataName = "icudt" + icuDataVersion + ENDIAN_STR;
        String generatedDatFile = sessionDirStr + baseDataName + ".dat";
        String generatedZipFile = sessionDirStr + baseDataName + ".zip";
        File generatedZipFileObj = new File(generatedZipFile);
        if (generatedZipFileObj.exists()) {
            // Hmm. Someone submitted the request twice.
            generatedZipFileObj.delete();
            //reportError(response, HttpServletResponse.SC_CONFLICT, "Please finish the download of your existing request before creating a new request.");
            //return;
        }
        String pkgCommand = "icupkg -tl -a " + packageList
            + " -s " + toolHomeSrcDirStr + baseDataName
            + " new " + generatedDatFile;
        if (!runCommand(response, pkgCommand, sessionDir, "Packaging tool")) {
            return;
        }
        if (!DEBUG_FILES) {
            (new File(packageList)).delete();
        }
        packageList = "";
        for (int idx = 0; idx < generatedIndexesVect.size(); idx++) {
            String pkgIndexesCommand = "icupkg -tl -a " + (String)generatedIndexesVect.elementAt(idx) + " -s . " + generatedDatFile;
            if (!runCommand(response, pkgIndexesCommand, sessionDir, "Packaging indexes tool")) {
                return;
            }
        }
        /*
         * -1 Reduces CPU usage, and results in a zip file 3-5% larger than standard compression
         * -9 Increases CPU usage, and results in a zip file 0-1% smaller than standard compression
         */
        String compressCommand = "zip -1q " + generatedZipFile + " " + baseDataName + ".dat";
        if (!runCommand(response, compressCommand, sessionDir, "Compression tool")) {
            return;
        }
        
        // Deleting these files allows us to "unlock" the session for another request regardless if the file was downloaded.
        if (!DEBUG_FILES) {
            (new File(generatedDatFile)).delete();
        }

        // Redirect with the result.
        response.getWriter().print(request.getRequestURL() + "/" + sessionID + "/" + baseDataName + ".zip");
    }

    private void generateICU4JData(HttpServletRequest request,
            HttpServletResponse response,
            Vector filesToPackage, Vector generatedIndexesVect, String icuDataVersion, String sessionID, File sessionDir) throws IOException
    {
        String sessionDirStr = sessionDir.getAbsolutePath() + File.separator;
        String baseDataName = "icudt" + icuDataVersion + ENDIAN_STR;
        String packagePath = "com/ibm/icu/impl/data/icudt" + icuDataVersion + "b";
        
        // Hmm. Did the person submit the request twice? There is a conflict. Lock out this request.
        File packageICU4JDir = new File(sessionDirStr + packagePath);
        if (!packageICU4JDir.mkdirs()) {
            reportError(response, HttpServletResponse.SC_INTERNAL_SERVER_ERROR, "Could not create package directory structure in " + packageICU4JDir);
            return;
        }

        // Copy and convert each file from big endian to little endian.
        for (int idx = 0; idx < filesToPackage.size(); idx++) {
            String itemToRead = (String)filesToPackage.elementAt(idx);
            String pkgCommand = "icupkg -tb -s " + toolHomeSrcDirStr + baseDataName
                + " -d " + sessionDirStr + packagePath + " " + itemToRead;
            if (!runCommand(response, pkgCommand, sessionDir, "Packaging tool")) {
                return;
            }
        }
        for (int idx = 0; idx < generatedIndexesVect.size(); idx++) {
            String itemToRead = (String)generatedIndexesVect.elementAt(idx);
            String pkgCommand = "icupkg -tb -s . -d " + sessionDirStr + packagePath + " " + itemToRead;
            if (!runCommand(response, pkgCommand, sessionDir, "Packaging tool for index regeneration")) {
                return;
            }
        }
        File generatedJarFile = new File(sessionDirStr + "icudata.jar");
        if (generatedJarFile.exists()) {
            // Hmm. Someone submitted the request twice.
            generatedJarFile.delete();
            //reportError(response, HttpServletResponse.SC_CONFLICT, "Please finish the download of your existing request before creating a new request.");
            //return;
        }
        String pkgCommand = "jar -cf icudata.jar com";
        if (!runCommand(response, pkgCommand, sessionDir, "jar tool")) {
            return;
        }
        // Deleting these files allows us to "unlock" the session for another request regardless if the file was downloaded.
        if (!DEBUG_FILES) {
            for (int idx = 0; idx < filesToPackage.size(); idx++) {
                (new File(sessionDirStr + packagePath  + File.separator + (String)filesToPackage.elementAt(idx))).delete();
            }
            File packagePathFile = new File(sessionDirStr + packagePath);
            File treeDirs[] = packagePathFile.listFiles();
            if (treeDirs != null) {
                for (int idx = 0; idx < treeDirs.length; idx++) {
                    treeDirs[idx].delete();
                }
            }
            // Basically finish doing an "rm -rf com"
            while (!sessionDir.equals(packagePathFile)) {
                packagePathFile.delete();
                packagePathFile = packagePathFile.getParentFile();
            }
        }

        // Redirect with the result.
        response.getWriter().print(
                request.getRequestURL() + "/" + sessionID + "/icudata.jar");
    }

    public void doGet(HttpServletRequest request, HttpServletResponse response)
            throws IOException, ServletException
    {
        applyNoCache(response);

        //String sessionID = request.getSession(true).getId();
        /*if (sessionID == null) {
            reportError(response, HttpServletResponse.SC_FORBIDDEN, "Session is stale");
            return;
        }*/
        
        //dumpRequest(request, response.getWriter());
        
        /* Figure out which file is being requested */
        String pathInfo = request.getPathInfo();
        int fileIndex = pathInfo.lastIndexOf('/');
        if (fileIndex <= 1) {
            reportError(response, HttpServletResponse.SC_BAD_REQUEST, pathInfo + " is not a valid requested file");
            return;
        }
        int sessionIndex = pathInfo.lastIndexOf('/', fileIndex - 1);
        if (sessionIndex < 0) {
            reportError(response, HttpServletResponse.SC_BAD_REQUEST, pathInfo + " is not a valid requested file");
            return;
        }
        /* Really make sure they are not going outside the data sandbox. */
        String fileToSend = pathInfo.substring(fileIndex + 1);
        String pathToSend = pathInfo.substring(sessionIndex + 1, fileIndex);
        if (isPotentiallyPath(fileToSend) || isPotentiallyPath(pathToSend)) {
            reportError(response, HttpServletResponse.SC_BAD_REQUEST, pathInfo + " is not a valid requested file");
            return;
        }
        
        File generatedFile = new File(toolHomeRequestDirStr + pathToSend + File.separator + fileToSend);
        if (!generatedFile.exists()) {
            reportError(response, HttpServletResponse.SC_GONE, fileToSend + " has already been downloaded. Please submit a new request.");
            return;
        }
        
        response.setContentType("application/zip");
        InputStream inZip = new FileInputStream(generatedFile);
        OutputStream outStream = response.getOutputStream();
        byte []zipBytes = new byte[inZip.available()];
        inZip.read(zipBytes);
        inZip.close();
        outStream.write(zipBytes);
        outStream.flush();
        if (!DEBUG_FILES) {
            generatedFile.delete();
            (new File(toolHomeRequestDirStr + pathToSend)).delete();
        }

        //writer.println(fileToSend);
        //writer.println(generatedFile);
        //response.setContentType("text/html; charset=utf-8");
    }
    
    private static boolean isPotentiallyPath(String str) {
        return str == null || str.length() == 0 || str.indexOf('/') != -1 || str.indexOf('\\') != -1 || str.indexOf("..") != -1;
    }
    
    private void reportError(HttpServletResponse response, int httpStatus, String msg) throws IOException {
        logger.warning(msg);
        response.sendError(httpStatus, msg);
    }

    private boolean runCommand(HttpServletResponse response, String command, File execDir, String genericName)
        throws IOException
    {
        Process icupkg = Runtime.getRuntime().exec(command, null, execDir);
        try {
            icupkg.waitFor();
        }
        catch (InterruptedException e) {
            reportError(response, HttpServletResponse.SC_INTERNAL_SERVER_ERROR, genericName + " was interrupted.");
            return false;
        }
        BufferedReader icupkgErrOut = new BufferedReader(new InputStreamReader(icupkg.getErrorStream()));
        if (icupkg.exitValue() != 0) {
            //String msg = "\"" + command + "\" failed with the following message:";
            String msg = "";
            String outLine = "";
            while ((outLine = icupkgErrOut.readLine()) != null) {
                msg += outLine + "\n";
            }
            reportError(response, HttpServletResponse.SC_INTERNAL_SERVER_ERROR, msg);
            return false;
        }
        return true;
    }
    
    private boolean deleteResIndexFiles(Vector listOfIndexes, File targetDirectory)
    {
        boolean allDeleted = true;
        if (!DEBUG_FILES) {
            for (int idx = 0; idx < listOfIndexes.size(); idx++) {
                String indexFile = (String) listOfIndexes.elementAt(idx);
                File treeIndexFile = new File(targetDirectory.getAbsolutePath()
                        + File.separator + indexFile);
                if (!treeIndexFile.exists() || !treeIndexFile.delete()) {
                    allDeleted = false;
                }
                String treePrefix = getTreePrefix(indexFile);
                if (treePrefix.length() > 0) {
                    // Delete only the temporary subdirectories
                    File treeIndexDir = new File(targetDirectory.getAbsolutePath()
                            + File.separator + treePrefix);
                    if (!treeIndexDir.exists() || !treeIndexDir.delete()) {
                        allDeleted = false;
                    }
                }
            }
        }
        return allDeleted;
    }

    private static String getTreePrefix(String dataItem) {
        int endOfTree = dataItem.lastIndexOf("/");
        if (endOfTree > 0) {
            return dataItem.substring(0, endOfTree + 1);
        }
        return "";
    }

    private boolean regenerateResIndexFiles(HttpServletResponse response, Vector listOfIndexes, Vector listOfFiles, File targetDirectory)
        throws IOException
    {
        for (int idx = 0; idx < listOfIndexes.size(); idx++) {
            String indexFile = (String)listOfIndexes.elementAt(idx);
            String prefixToMatch = getTreePrefix(indexFile);
            Vector listOfLocales = new Vector();
            boolean checkPrefix = prefixToMatch.length() != 0;
            for (int resIdx = 0; resIdx < listOfFiles.size(); resIdx++) {
                String file = (String)listOfFiles.elementAt(resIdx);
                if (checkPrefix && file.startsWith(prefixToMatch) || !checkPrefix && file.indexOf('/') < 0) {
                    listOfLocales.add(file);
                }
            }
            File treeIndexDir = new File(targetDirectory.getAbsolutePath() + File.separator + prefixToMatch);
            if (checkPrefix && !treeIndexDir.exists()) {
                treeIndexDir.mkdirs();
            }
            if (!regenerateResIndex(response, listOfLocales, treeIndexDir)) {
                reportError(response, HttpServletResponse.SC_INTERNAL_SERVER_ERROR, indexFile + " could not be regenerated.");
                return false;
            }
        }
        return true;
    }
    
    private boolean regenerateResIndex(HttpServletResponse response, Vector listOfLocales, File targetDirectory)
        throws IOException
    {
        String resIndexFileStr = targetDirectory.getAbsolutePath() + File.separator + "res_index.txt";
        File resIndexFile = new File(resIndexFileStr);
        PrintWriter dataToPackageWriter = new PrintWriter(resIndexFile);

        // Generate res_index.txt
        dataToPackageWriter.println("res_index:table(nofallback) {");
        dataToPackageWriter.println("InstalledLocales {");
        for (int idx = 0; idx < listOfLocales.size(); idx++) {
            String locale = (String)listOfLocales.elementAt(idx);
            int startIdx = locale.lastIndexOf('/');
            if (startIdx < 0) {
                startIdx = 0; // Start at the beginning
            }
            else {
                startIdx++; // skip the slash
            }
            locale = locale.substring(startIdx, locale.lastIndexOf('.'));
            dataToPackageWriter.println(locale + "{\"\"}");
        }
        dataToPackageWriter.println("}");
        dataToPackageWriter.println("}");
        dataToPackageWriter.close();
        
        // Compile res_index.txt
        String command = "genrb " + resIndexFileStr;
        if (!runCommand(response, command, targetDirectory, "Index regeneration")) {
            return false;
        }
        if (!DEBUG_FILES) {
            resIndexFile.delete();
        }
        return true;
    }
    
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
        writer.println("<p>");
        writer.println("The current home directory is " + System.getProperty("user.name"));
        writer.println("</p>");
        writer.println("<p>");
        writer.println("The current path info is " + request.getPathInfo());
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

        writer.println("<table border=\"1\">");
        writer.println("<caption>Attribute Information</caption>");
        Enumeration attribNames = request.getAttributeNames();
        while (attribNames.hasMoreElements()) {
            String name = (String) attribNames.nextElement();
            writer.println("<tr>");
            writer.println("  <th align=\"right\">" + name + ":</th>");
            writer.println("  <td>" + request.getAttribute(name) + "</td>");
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

        toolHome = config.getInitParameter("DATA_CUSTOM.home");
        if (toolHome == null) {
            toolHome = System.getProperty("user.home");
            if (toolHome.lastIndexOf(File.separatorChar) != (toolHome.length() - 1)) {
                toolHome += File.separator;
            }
            toolHome += "datacustom" + File.separator;
        }

        logger.info(toolHome + " will be used for reading and writing data.");
        
        File toolHomeDir = new File(toolHome);
        if (!toolHomeDir.exists()) {
            if (toolHomeDir.mkdirs()) {
                logger.info(toolHomeDir.getAbsolutePath() + " was created.");
            }
            else {
                logger.warning(toolHomeDir.getAbsolutePath() + " could not be created.");
            }
        }
        if (!toolHomeDir.isDirectory() || !toolHomeDir.canRead()) {
            logger.warning(toolHomeDir.getAbsolutePath() + " is not a valid data directory.");
        }
        else {
            /* Check and create required subdirectories. */
            File toolHomeSrcDir = new File(toolHome + "source");
            toolHomeSrcDirStr = toolHomeSrcDir.getAbsolutePath() + File.separator;
            if (!toolHomeSrcDir.exists()) {
                logger.warning(toolHomeSrcDirStr + " does not exist.");
            }
            else if (!toolHomeSrcDir.isDirectory() || !toolHomeSrcDir.canRead()) {
                logger.warning(toolHomeSrcDirStr + " is not a valid directory.");
            }
            /*else if (toolHomeDir.canWrite()) {
                logger.warning(toolHomeSrcDirStr + " shouldn't be writable for security reasons.");
            }*/
            
            File toolHomeRequestDir = new File(toolHome + "request");
            toolHomeRequestDirStr = toolHomeRequestDir.getAbsolutePath() + File.separator;
            if (!toolHomeRequestDir.exists()) {
                if (!toolHomeRequestDir.mkdir()) {
                    logger.warning(toolHomeRequestDirStr + " could not be created.");
                }
            }
            if (!toolHomeRequestDir.isDirectory() || !toolHomeRequestDir.canRead() || !toolHomeRequestDir.canWrite()) {
                logger.warning(toolHomeRequestDirStr + " is not a valid directory.");
            }
        }
        
        String rootRealPath = config.getServletContext().getRealPath("/");
        try {
            docBuilderFactory = DocumentBuilderFactory.newInstance();
            //logger.info("rootRealPath=" + rootRealPath);
            File schemaFile = new File(rootRealPath + "datapackage.xsd");
            if (!schemaFile.exists()) {
                logger.warning(schemaFile.getAbsolutePath() + " does not exist.");
                return;
            }
            Source schemaFileStream = new StreamSource(schemaFile);
            /** A Schema is thread safe. A Validator is thread unsafe. */
            Schema dataRequestSchema = SchemaFactory.newInstance(XMLConstants.W3C_XML_SCHEMA_NS_URI).newSchema(schemaFileStream);
            docBuilderFactory.setSchema(dataRequestSchema);
        }
        catch (SAXException e) {
            logger.warning(e.getMessage());
        }
        
        logger.info("ICU Data Customization tool ready for requests. Memory in use: "
                        + usedKB() + "KB");
    }
    
    private void applyNoCache(HttpServletResponse response) {
        response.setHeader("Cache-Control", "no-cache");
        response.setDateHeader("Expires", 0);
        response.setHeader("Pragma", "no-cache");
        response.setDateHeader("Max-Age", 0);
    }
    
    /**
     * DocumentBuilder and DocumentBuilderFactory are not thread safe.
     * Synchronized to ensure thread safety.
     */
    private synchronized DocumentBuilder getDocumentBuilder() throws ParserConfigurationException {
        return docBuilderFactory.newDocumentBuilder();
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
