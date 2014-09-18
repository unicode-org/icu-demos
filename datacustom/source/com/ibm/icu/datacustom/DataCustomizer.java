/*
 ******************************************************************************
 * Copyright (C) 2007-2014, International Business Machines Corporation and   *
 * others. All Rights Reserved.                                               *
 ******************************************************************************
 */

package com.ibm.icu.datacustom;

import java.io.*;
//import java.nio.channels.FileChannel;
import java.util.*;
import java.util.logging.*;

import javax.servlet.*;
import javax.servlet.http.*;
import javax.xml.XMLConstants;
import javax.xml.parsers.*;
import javax.xml.transform.Source;
import javax.xml.transform.stream.StreamSource;
import javax.xml.validation.*;

// DOM imports
import org.w3c.dom.*;
import org.xml.sax.*;

/**
 * The main servlet class of the Data Customization tool
 */
public class DataCustomizer extends HttpServlet {

    // TODO: Add comment to manifest and .dat file about source of customizations. Add .res file with meta package information.
    // TODO: Delete unused packages after a timeout.
    // TODO: Cache big-endian results for ICU4J.
    // TODO: Make the path to the ICU tools a property.
    // TODO: Use exceptions instead of if statements to detect if execution should continue.
    // TODO: Don't include an empty res_index
    
	/**
	 * For serialization, which is unused.
	 */
	private static final long serialVersionUID = 1430423129503208114L;

	// Standard logging for information and errors
    public static Logger logger = Logger.getLogger("com.ibm.icu.DataCustomizer");

    // Summary log of each request. For example, what was requested, size and time of request.
    public static Logger requestLogger;
    private static FileHandler requestFileHandler;
    
    public static final String NEWLINE = System.getProperty("line.separator");
    public static final String ENDIAN_STR = "l"; // We expect the little endian format to be the default.
    private static final int DEFAULT_FILE_BUFFER_SIZE = 1048576; // 2^20 or 1MB

    /** Location for all file manipulation, reading, logging and so forth. */
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
            requestLogger.info("Starting request for " + sessionID);
            String filesToPackage = "";
            Vector<String> filesToPackageVect = new Vector<String>();
            Vector<String> indexesToGenerateVect = new Vector<String>();
            Vector<String> localesToIndexVect = new Vector<String>();
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
                String targetTypeStr = xmlDoc.getElementsByTagName("target").item(0).getFirstChild().getNodeValue();
                if (targetTypeStr.equals("ICU4C")) {
                    icuDataType = ICUDataFormat.ICU4C;
                }
                else {
                    icuDataType = ICUDataFormat.ICU4J;
                }
                String baseDataName = "icudt" + icuDataVersion + ENDIAN_STR;
                int itemsLen = pkgItems.getLength();
                for (int itemIdx = 0; itemIdx < itemsLen; itemIdx++) {
                    Node pkgItemNode = pkgItems.item(itemIdx);
                    /*
                     * The name attribute key must be present and unique according the schema.
                     * So we can safely assume that a non-null NamedNodeMap will be returned.
                     */
                    NamedNodeMap itemAttNodeMap = pkgItemNode.getAttributes();
                    String pkgItemName = itemAttNodeMap.getNamedItem("name").getNodeValue();
                    // The schema should have already taken care of the file pattern authentication. 
                    File fileToRead = new File(toolHomeSrcDirStr + baseDataName + File.separator + pkgItemName);
                    if (!fileToRead.exists() || !fileToRead.canRead()) {
                        reportError(response, HttpServletResponse.SC_NOT_FOUND, pkgItemName + " was not found.");
                        return;
                    }
                    filesToPackage += pkgItemName + NEWLINE;
                    if (icuDataType == ICUDataFormat.ICU4J) {
                        filesToPackageVect.add(pkgItemName);
                    }
                    if (pkgItemName.endsWith(".res")) {
                        if (pkgItemName.endsWith("res_index.res")) {
                            indexesToGenerateVect.add(pkgItemName);
                        }
                        else {
                            String hiddenVal = "0";
                            Node hiddenNode = itemAttNodeMap.getNamedItem("hidden");
                            if (hiddenNode != null) {
                                String newHiddenVal = hiddenNode.getNodeValue();
                                if (newHiddenVal != null) {
                                    hiddenVal = newHiddenVal;
                                }
                            }
                            if (hiddenVal.equals("0")) {
                                localesToIndexVect.add(pkgItemName);
                            }
                        }
                    }
                }
                requestLogger.info("Requested " + itemsLen + " items in " + targetTypeStr + " format for session " + sessionID + "." + NEWLINE
                        + filesToPackage);
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
            if (!regenerateResIndexFiles(response, indexesToGenerateVect, localesToIndexVect, sessionDir, icuDataVersion)) {
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
            requestLogger.info("Finished request for " + sessionID);
        }
        else {
            PrintWriter writer = response.getWriter();
            dumpRequest(request, writer);
        }

        //response.setContentType("text/xml; charset=utf-8");
    }

    private void generateICU4CData(HttpServletRequest request, HttpServletResponse response, String filesToPackage, Vector<String> generatedIndexesVect, String icuDataVersion, String sessionID, File sessionDir)
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
        
        // Different versions of the data require different versions of the icupkg tool.
        String icupkgCommand;
        if (Integer.parseInt(icuDataVersion) == 40) {
            icupkgCommand = "icupkg44";
        } else {
            icupkgCommand = "icupkg"; // latest
        }
        String pkgCommand = icupkgCommand + " -tl -a " + packageList
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
            String pkgIndexesCommand = icupkgCommand + " -tl -a " + (String)generatedIndexesVect.elementAt(idx) + " -s . " + generatedDatFile;
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
            Vector<String> filesToPackage, Vector<String> generatedIndexesVect, String icuDataVersion, String sessionID, File sessionDir) throws IOException
    {
        String sessionDirStr = sessionDir.getAbsolutePath() + File.separator;
        String packagePath = sessionDirStr + "com/ibm/icu/impl/data/icudt" + icuDataVersion + "b/";
        
        // Hmm. Did the person submit the request twice? There is a conflict. Lock out this request.
        File packageICU4JDir = new File(packagePath);
        if (!packageICU4JDir.mkdirs()) {
            reportError(response, HttpServletResponse.SC_INTERNAL_SERVER_ERROR, "Could not create package directory structure in " + packageICU4JDir);
            return;
        }

        // Copy and convert each file from big endian to little endian.
        String srcDir = toolHomeSrcDirStr + "icudt" + icuDataVersion + ENDIAN_STR + "/";
        
        // Different versions of the data require different versions of the icupkg tool.
        String icupkgCommand;
        if (Integer.parseInt(icuDataVersion) < 46) {
            icupkgCommand = "icupkg44";
        } else if (Integer.parseInt(icuDataVersion) == 46) {
            icupkgCommand = "icupkg46";
        } else if (Integer.parseInt(icuDataVersion) == 48) {
            icupkgCommand = "icupkg48";
        } else if (Integer.parseInt(icuDataVersion) == 49) {
            icupkgCommand = "icupkg49";
        } else if (Integer.parseInt(icuDataVersion) == 50) {
            icupkgCommand = "icupkg50";
        } else if (Integer.parseInt(icuDataVersion) == 51) {
            icupkgCommand = "icupkg51";
        } else if (Integer.parseInt(icuDataVersion) == 52) {
            icupkgCommand = "icupkg52";
        } else if (Integer.parseInt(icuDataVersion) == 53) {
            icupkgCommand = "icupkg53";
        } else if (Integer.parseInt(icuDataVersion) == 54) {
            icupkgCommand = "icupkg54";
        }
        
        for (int idx = 0; idx < filesToPackage.size(); idx++) {
            String itemToRead = (String)filesToPackage.elementAt(idx);
            int treeIndex = itemToRead.lastIndexOf('/');
            if (treeIndex >= 0) {
                String treeDir = packagePath + itemToRead.substring(0, treeIndex);
                File treeDirFile = new File(treeDir);
                if (!treeDirFile.exists() && !treeDirFile.mkdirs()) {
                    reportError(response, HttpServletResponse.SC_INTERNAL_SERVER_ERROR, "Can not create directory " + treeDir);
                    return;
                }
            }
            /*try {
                copyFile(srcDir + itemToRead, packagePath + itemToRead);
            }
            catch (IOException e) {
                reportError(response, HttpServletResponse.SC_INTERNAL_SERVER_ERROR, e.getMessage());
                return;
            }*/
            /* Use pre-46 icupkg for pre-46 data.
             * This is needed for some data files (e.g. collation res files).
             */
            String pkgCommand = icupkgCommand + " -tb -s " + srcDir
                + " -d " + packagePath + " " + itemToRead;
            if (!runCommand(response, pkgCommand, sessionDir, "Packaging tool")) {
                return;
            }
        }
        for (int idx = 0; idx < generatedIndexesVect.size(); idx++) {
            String itemToRead = (String)generatedIndexesVect.elementAt(idx);
            String pkgCommand = icupkgCommand + " -tb -s . -d " + packagePath + " " + itemToRead;
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
                (new File(packagePath + File.separator + (String)filesToPackage.elementAt(idx))).delete();
            }
            File packagePathFile = new File(packagePath);
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
        
        if (fileToSend.endsWith(".jar")) {
            response.setContentType("application/java-archive");
        }
        else if (fileToSend.endsWith(".zip")) {
            response.setContentType("application/zip");
        }
        int generatedFileSize = (int)generatedFile.length();
        response.setContentLength(generatedFileSize);
        requestLogger.info(generatedFile + " is " + generatedFileSize + " bytes big.");
        
        // Read the file in, and write it to the client using DEFAULT_FILE_BUFFER_SIZE chunks.
        // It's buffered this way to save space in the JVM for each request.
        InputStream inZip = new FileInputStream(generatedFile);
        OutputStream outStream = response.getOutputStream();
        byte []zipBytes = new byte[DEFAULT_FILE_BUFFER_SIZE];
        while (inZip.available() > 0) {
            outStream.write(zipBytes, 0, inZip.read(zipBytes));
        }
        outStream.flush();
        inZip.close();
        if (!DEBUG_FILES) {
            generatedFile.delete();
            (new File(toolHomeRequestDirStr + pathToSend)).delete();
        }
        requestLogger.info("Finished sending " + generatedFile);

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
    	Process commandProcess;
    	try {
    		commandProcess = Runtime.getRuntime().exec(command, null, execDir);
    	}
        catch (IOException e) {
            System.err.println(e.toString() + " running " + command + " in " + execDir.getAbsolutePath());
            e.printStackTrace();
            reportError(response, HttpServletResponse.SC_INTERNAL_SERVER_ERROR, genericName + " could not be run." /* + e.getMessage()*/);
            return false;
        }
        try {
            commandProcess.waitFor();
        }
        catch (InterruptedException e) {
            reportError(response, HttpServletResponse.SC_INTERNAL_SERVER_ERROR, genericName + " was interrupted.");
            return false;
        }
        BufferedReader icupkgErrOut = new BufferedReader(new InputStreamReader(commandProcess.getErrorStream()));
        if (commandProcess.exitValue() != 0) {
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
    
    private boolean deleteResIndexFiles(Vector<String> listOfIndexes, File targetDirectory)
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

    private boolean regenerateResIndexFiles(HttpServletResponse response, Vector<String> listOfIndexes, Vector<String> listOfFiles, File targetDirectory, String icuDataVersion)
        throws IOException
    {
        for (int idx = 0; idx < listOfIndexes.size(); idx++) {
            String indexFile = (String)listOfIndexes.elementAt(idx);
            String prefixToMatch = getTreePrefix(indexFile);
            Vector<String> listOfLocales = new Vector<String>();
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
            // TODO: Remove res_index.res when it's empty? 
            // Are we really indexing anything?
            //if (listOfLocales.size() > 0) {
                if (!regenerateResIndex(response, listOfLocales, treeIndexDir, icuDataVersion)) {
                    reportError(response, HttpServletResponse.SC_INTERNAL_SERVER_ERROR, indexFile + " could not be regenerated.");
                    return false;
                }
            /*}
            else {
                (new File(treeIndexDir.getAbsolutePath() + File.separator + "res_index.res")).delete();
                // Remove from the res_index list
                listOfIndexes.remove(indexFile);
                idx--; 
            }*/
        }
        return true;
    }
    
    private boolean regenerateResIndex(HttpServletResponse response, Vector<String> listOfLocales, File targetDirectory, String icuDataVersion)
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
        
        // If building 3.8 or 4.0 data, use formatVersion 1 option
        String genrbFormatVersionOption;
        if (icuDataVersion.equals("40") || icuDataVersion.equals("38")) {
            genrbFormatVersionOption = "--formatVersion 1";
        } else {
            genrbFormatVersionOption = "";
        }
        // Compile res_index.txt
        String command = "genrb " + genrbFormatVersionOption + " " + resIndexFileStr;
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

    /*private static void copyFile(String source, String destination) throws IOException
    {
        FileChannel srcChannel = new FileInputStream(source).getChannel();
        FileChannel dstChannel = new FileOutputStream(destination).getChannel();
    
        // Copy file contents from source to destination
        long srcSize = srcChannel.size();
        if (dstChannel.transferFrom(srcChannel, 0, srcSize) < srcSize) {
            throw new IOException(source + " could not be completely copied to " + destination);
        }
        // Close the channels
        srcChannel.close();
        dstChannel.close();
    }*/
    /**
     * Main setup
     */
    private static boolean isSetup = false;

    private synchronized void doStartup() throws ServletException {
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
            return;
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
        
        try {
            requestLogger = Logger.getLogger("com.ibm.icu.DataCustomizerRequest");
            requestLogger.setUseParentHandlers(false); // Disable console output
    
            File logDir = new File(toolHome + "log");
            String logDirStr = logDir.getAbsolutePath() + File.separator;
            if (!logDir.exists()) {
                if (!logDir.mkdir()) {
                    logger.warning(logDirStr + " could not be created.");
                }
            }
            if (!logDir.isDirectory() || !logDir.canRead() || !logDir.canWrite()) {
                logger.warning(logDirStr + " is not a valid directory.");
            }
            requestFileHandler = new FileHandler(logDirStr + "requests-%u-%g.log", true);
            requestFileHandler.setFormatter(new SimpleFormatter());
            requestLogger.addHandler(requestFileHandler);
    
            requestLogger.setLevel(Level.ALL);
        }
        catch (IOException e) {
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
     * By default, we want the parse to fail on all funny requests.
     */
    private static final ErrorHandler DEFAULT_SAX_ERROR_HANDLER = new ErrorHandler() {
        public void warning(SAXParseException e) throws SAXException {
            throw e;
        }

        public void error(SAXParseException e) throws SAXException {
            throw e;
        }

        public void fatalError(SAXParseException e) throws SAXException {
            throw e;
        }
    };

    /**
     * DocumentBuilder and DocumentBuilderFactory are not thread safe.
     * Synchronized to ensure thread safety.
     */
    private synchronized DocumentBuilder getDocumentBuilder() throws ParserConfigurationException {
        DocumentBuilder docBuilder = docBuilderFactory.newDocumentBuilder();
        docBuilder.setErrorHandler(DEFAULT_SAX_ERROR_HANDLER);
        return docBuilder;
    }

    public void destroy() {
        logger.warning("ICU Data Customization tool shutting down.");
        super.destroy();
        requestLogger = null;
        requestFileHandler.close();
        logger.warning("ICU Data Customization tool shut down.");
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
