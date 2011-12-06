/*
 *******************************************************************************
 * Copyright (C) 2009, International Business Machines Corporation and         *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
package com.ibm.icu.dev.tools.wintz;

//typedef struct _REG_TZI_FORMAT
//{
//    LONG Bias;
//    LONG StandardBias;
//    LONG DaylightBias;
//    SYSTEMTIME StandardDate;
//    SYSTEMTIME DaylightDate;
//}

class TZI {

    private int _bias;
    private int _standardBias;
    private int _daylightBias;
    private SystemTime _standardDate;
    private SystemTime _daylightDate;

    private static final int TZI_DATA_LEN = 44;
    
    public static TZI getInstance(byte[] data) {
        if (data.length != TZI_DATA_LEN) {
            throw new IllegalArgumentException("TZI data length must be " + TZI_DATA_LEN + " - actual:" + data.length);
        }
        TZI tzi = new TZI();

        tzi._bias = parseLong(data, 0);
        tzi._standardBias = parseLong(data, 4);
        tzi._daylightBias = parseLong(data, 8);
        tzi._standardDate = parseSystemTime(data, 12);
        tzi._daylightDate = parseSystemTime(data, 28);

        return tzi;
    }

    private TZI() {
    }

    public int getBias() {
        return _bias;
    }

    public int getStandardBias() {
        return _standardBias;
    }

    public int getDaylightBias() {
        return _daylightBias;
    }

    public SystemTime getStandardDate() {
        return _standardDate;
    }

    public SystemTime getDaylightDate() {
        return _daylightDate;
    }

    private static int parseLong(byte[] data, int idx) {
        // Windows LONG is 4-byte Little Endian
        return (int)data[idx] & 0xFF
                | ((int)data[idx + 1] & 0xFF) << 8
                | ((int)data[idx + 2] & 0xFF) << 16
                | ((int)data[idx + 3] & 0xFF) << 24;
    }

    private static int parseWord(byte[] data, int idx) {
        // Windows WORD is 2-byte Little Endian
        return (int)data[idx]
                | ((int)data[idx + 1] & 0xFF) << 4;
    }

    private static SystemTime parseSystemTime(byte[] data, int idx) {
        int year = parseWord(data, idx);
        int month = parseWord(data, idx + 2);
        int dayOfMonth = parseWord(data, idx + 4);
        int day = parseWord(data, idx + 6);
        int hour = parseWord(data, idx + 8);
        int minute = parseWord(data, idx + 10);
        int second = parseWord(data, idx + 12);
        int milliseconds = parseWord(data, idx + 14);

        return new SystemTime(year, month, dayOfMonth, day, hour, minute, second, milliseconds);
    }
}
