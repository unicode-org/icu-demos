/*
 *******************************************************************************
 *   Copyright (C) 2009, International Business Machines
 *   Corporation and others.  All Rights Reserved.
 *******************************************************************************
 * 
 * Author Michael Ow
 * Created on 090206
 */
import com.ibm.icu.text.Collator;
import com.ibm.icu.text.RuleBasedCollator;
import com.ibm.icu.text.CollationKey;
import com.ibm.icu.util.ULocale;
import com.ibm.icu.util.VersionInfo;

import java.io.File;
import java.io.FileWriter;
import java.io.FileInputStream;
import java.io.IOException;

public class CollationRegTest {
    public static void main(String[] args) {
        int result = 0;
        String resultDir = null;
        String dataFile = null;
        
        /* parse the options */
        for (int i = 0; i < args.length; i++) {
            /* result directory */
            if (args[i].equals("-d") || args[i].equals("--resultDir")) {
                resultDir = args[++i];
            }
            /* data file */
            if (args[i].equals("-f") || args[i].equals("--file")) {
                dataFile = args[++i];
            }
        }
        
        if (resultDir == null || dataFile == null) {
            System.out.println("print usage.");
        }
        
        System.out.println("Starting regression test...");
        
        result = startTest(resultDir, dataFile);
        if (result != 0) {
            System.out.println("Error running collation regression test.");
            return;
        }
        
        System.out.println("Done.\n");
    }
    
    private static int startTest(String resultDirPath, String dataFile) {
        int result = 0;
        String versionICU;
        String versionUCA;
        ULocale[]locales;
        
        locales = Collator.getAvailableULocales();
        
        versionICU = VersionInfo.ICU_VERSION.toString();
        versionUCA = Collator.getInstance().getUCAVersion().toString();
        
        for (int i = 0; i < locales.length; i++) {            
            System.out.println("Running test on Locale: " + locales[i].getName() + " (" + (i+1) + " out of " + locales.length + ")");
            
            RuleBasedCollator coll = (RuleBasedCollator)Collator.getInstance(locales[i]);
            
            if (runTestLoop(coll, resultDirPath, locales[i], versionICU, versionUCA, dataFile) != 0) {
                System.out.println("Error iterating through collation attributes.\n");
                result = -1;
                break;
            }
        }
        return result;
    }
    
    private static int runTestLoop(RuleBasedCollator coll, String resultDirPath, ULocale locale, String versionICU, String versionUCA, String dataFile) {
        int result = 0;
        FileInputStream pDataFile = null;
        
        for (int a = 0; a < 2; a++) {
            if (a > 0) { /* UCOL_FRENCH_COLLATION */
                coll.setFrenchCollation(true);
            } else {
                coll.setFrenchCollation(false);
            }
            
            for (int b = 0; b < 2; b++) { /* UCOL_ALTERNATE_HANDLING */
                if (b > 0) {
                    coll.setAlternateHandlingShifted(false);
                } else {
                    coll.setAlternateHandlingShifted(true);
                }
                
                for (int c = 0; c < 3; c++) { /* UCOL_CASE_FIRST */
                    if (c == 0) {
                        coll.setLowerCaseFirst(false);
                        coll.setUpperCaseFirst(false);
                    } else if (c == 1) {
                        coll.setUpperCaseFirst(true);
                    } else {
                        coll.setLowerCaseFirst(true);
                    }
                    
                    for (int d = 0; d < 2; d++) { /* UCOL_CASE_LEVEL*/
                        if (d > 0) {
                            coll.setCaseLevel(true);
                        } else {
                            coll.setCaseLevel(false);
                        }
                        
                        for (int e = 0; e < 2; e++) { /* UCOL_DECOMPOSITION_MODE */
                            if (e > 0) {
                                coll.setDecomposition(Collator.CANONICAL_DECOMPOSITION);
                            } else {
                                coll.setDecomposition(Collator.NO_DECOMPOSITION);
                            }
                            
                            for (int f = 0; f < 5; f++) { /* UCOL_STRENGTH */
                                switch (f) {
                                case 0:
                                    coll.setStrength(Collator.PRIMARY);
                                    break;
                                case 1:
                                    coll.setStrength(Collator.SECONDARY);
                                    break;
                                case 2:
                                    coll.setStrength(Collator.TERTIARY);
                                    break;
                                case 3:
                                    coll.setStrength(Collator.QUATERNARY);
                                    break;
                                case 4:
                                    coll.setStrength(Collator.IDENTICAL);
                                    break;
                                default :
                                    /* Should never occur. */
                                    break;
                                }
                                
                                for (int g = 0; g < 2; g++) { /* UCOL_HIRAGANA_QUATERNARY_MODE */
                                    if (g > 0) {
                                        coll.setHiraganaQuaternary(true);
                                    } else {
                                        coll.setHiraganaQuaternary(false);
                                    }
                                    
                                    for (int h = 0; h < 2; h++) { /* UCOL_NUMERIC_COLLATION */
                                        if (h > 0) {
                                            coll.setNumericCollation(true);
                                        } else {
                                            coll.setNumericCollation(false);
                                        }
                                        
                                        try {
                                            pDataFile = new FileInputStream(new File(dataFile));
                                        } catch (IOException ex) {
                                            System.out.println("While trying to open " + dataFile + " got this error: " +ex.getMessage());
                                            return -1;
                                        }
                                        
                                        /* Now run the test on the collator. */
                                        if ((result = runTest(coll, resultDirPath, locale, versionICU, versionUCA, pDataFile)) != 0) {
                                            return result;
                                        }
                                        
                                        try {
                                            pDataFile.close();
                                        } catch (IOException ex) {
                                            System.out.println("While trying to close reader to " + dataFile + " got this error: " + ex.getMessage());
                                            result = -1;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        
        return result;
    }
    
    private static int runTest(RuleBasedCollator coll, String resultDirPath, ULocale locale, String versionICU, String versionUCA, FileInputStream pDataFile) {
        int result = 0;
        FileWriter pFile;
        int inputTest;
        boolean firstByte = true;
        char data = 0x0000;
        String inputString = "";
        String outputString = "";
        String testResultPath = "";
        String collatorName = getCollatorShortDefinitionString(coll, locale);
        CollationKey collKey;
        byte[] collKeyArray;
        int count = 0;
        
        testResultPath += resultDirPath + "/" + locale.getName() + "/" + collatorName; 
        
        File testDir = new File(testResultPath);
        if (!testDir.exists()) {
            if (!testDir.mkdirs()) {
                System.out.println("Unable to create directory: " + testResultPath);
                return -1;
            }
        }
        
        testResultPath += "/" + versionICU + "-" + versionUCA + "-ICU4J";
        
        try {
            pFile = new FileWriter(testResultPath);
        } catch (IOException ex) {
            System.out.println("Unable to create file: " + testResultPath);
            return -1;
        }
        
        try {
            while ((inputTest = pDataFile.read()) != -1) {
                if (firstByte) {
                    data = (char)((inputTest<<8)&0xFF00);
                    firstByte = false;
                } else {
                    data = (char)(data|(inputTest&0xFF));
                    firstByte = true;
                    
                    if (data == 0x000A) {
                        collKey = coll.getCollationKey(inputString);
                        
                        collKeyArray = collKey.toByteArray();
                        
                        count++;
                        outputString += "[" + count + "] ";
                        for (int i = 0; i < collKeyArray.length; i++) {
                            int byteValue = ((int)collKeyArray[i]) & 0x00FF;
                            outputString += "\\x";
                            if (byteValue < 0x10) {
                                outputString += 0;
                            }
                            outputString += Integer.toHexString(byteValue).toUpperCase();
                            
                        }
                        outputString += "\n";
                        
                        pFile.write(outputString);
                        
                        inputString = "";
                        outputString = "";
                    } else {
                        inputString += data;
                    }
                }
            }
            pFile.close();
        } catch (IOException ex) {
            System.out.println("Error while reading from: " + pDataFile + " or writing to: " + pFile);
            return -1;
        }
        
        return result;
    }
    
    private static String getCollatorShortDefinitionString(RuleBasedCollator coll, ULocale locale) {
        String collatorString = "";
        
        /* UCOL_ALTERNATE_HANDLING */
        collatorString += "A";
        if (coll.isAlternateHandlingShifted()) {
            collatorString += "S";
        } else {
            collatorString += "N";
        }
        /* UCOL_CASE_FIRST */
        collatorString += "_C";
        if (!coll.isLowerCaseFirst() && !coll.isUpperCaseFirst()) {
            collatorString += "X";
        } else if (coll.isLowerCaseFirst() && !coll.isUpperCaseFirst()) {
            collatorString += "L";
        } else if (!coll.isLowerCaseFirst() && coll.isUpperCaseFirst()) {
            collatorString += "U";
        }
        /* UCOL_NUMERIC_COLLATION */
        collatorString += "_D";
        if (coll.getNumericCollation()) {
            collatorString += "O";
        } else {
            collatorString += "X";
        }
        /* UCOL_CASE_LEVEL */
        collatorString += "_E";
        if (coll.isCaseLevel()) {
            collatorString += "O";
        } else {
            collatorString += "X";
        }
        /* UCOL_FRENCH_COLLATION */
        collatorString += "_F";
        if (coll.isFrenchCollation()) {
            collatorString += "O";
        } else {
            collatorString += "X";
        }
        /* UCOL_HIRAGANA_QUATERNARY_MODE */
        collatorString += "_H";
        if (coll.isHiraganaQuaternary()) {
            collatorString += "O";
        } else {
            collatorString += "X";
        }
        /* LOCALE */
        collatorString += "_L" + locale.getLanguage().toUpperCase();
        /* UCOL_DECOMPOSITION_MODE */
        collatorString += "_N";
        if (coll.getDecomposition() == Collator.NO_DECOMPOSITION) {
            collatorString += "X";
        } else {
            collatorString += "O";
        }
        /* UCOL_STRENGTH */
        collatorString += "_S";
        switch(coll.getStrength()) {
        case Collator.PRIMARY:
            collatorString += "1";
            break;
        case Collator.SECONDARY:
            collatorString += "2";
            break;
        case Collator.TERTIARY:
            collatorString += "3";
            break;
        case Collator.QUATERNARY:
            collatorString += "4";
            break;
        case Collator.IDENTICAL:
            collatorString += "I";
            break;
        default:
            /* Should not occur. */
            break;
        }
        
        return collatorString;
    }

}
