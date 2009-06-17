/*
 *******************************************************************************
 * Copyright (C) 2009, International Business Machines Corporation and         *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
package com.ibm.icu.dev.tools.wintz;

public class RegUtil {
    public static String getKey(String line) {
        int eqIdx = line.indexOf('=');
        if (eqIdx < 2) {
            return null;
        }
        return line.substring(1, eqIdx - 1).trim();
    }

    private static String getValue(String line) {
        int eqIdx = line.indexOf('=');
        if (eqIdx < 2) {
            throw new IllegalArgumentException("No equal sign - " + line);
        }
        return line.substring(eqIdx + 1, line.length()).trim();
    }

    public static String getStringValue(String line) {
        String val =getValue(line);
        if (val.charAt(0) != '"' || val.charAt(val.length() - 1) != '"') {
            throw new IllegalArgumentException("No double quote characters - " + line);
        }
        return val.substring(1, val.length() - 1);
    }

    public static byte[] getBinaryValue(String line) {
        String val = getValue(line);
        if (!val.startsWith("hex:")) {
            throw new IllegalArgumentException("Regstry value does not start with hex - " + line);
        }

        String[] hex = val.substring(4).split(","); // 4 -> skip "hex:"
        byte[] bin = new byte[hex.length];
        for (int i = 0; i < hex.length; i++) {
            bin[i] = (byte)Integer.parseInt(hex[i], 16);
        }

        return bin;
    }

    public static int getDwordValue(String line) {
        String val = getValue(line);
        if (!val.startsWith("dword:")) {
            throw new IllegalArgumentException("Regstry value does not start with dword - " + line);
        }

        String digits = val.substring(6);
        if (digits.length() != 8) {
            throw new IllegalArgumentException("Regstry DWORD value is not length of 8 - " + line);
        }

        int dwordVal = Integer.parseInt(digits.substring(0, 2), 16);
        dwordVal = (dwordVal << 8) | Integer.parseInt(digits.substring(2, 4), 16);
        dwordVal = (dwordVal << 8) | Integer.parseInt(digits.substring(4, 6), 16);
        dwordVal = (dwordVal << 8) | Integer.parseInt(digits.substring(6, 8), 16);

        return dwordVal;
    }
}
