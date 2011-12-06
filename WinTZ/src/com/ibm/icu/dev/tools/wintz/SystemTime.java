/*
 *******************************************************************************
 * Copyright (C) 2009-2011, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
package com.ibm.icu.dev.tools.wintz;

//typedef struct _SYSTEMTIME {
//    WORD wYear;
//    WORD wMonth;
//    WORD wDayOfWeek;
//    WORD wDay;
//    WORD wHour;
//    WORD wMinute;
//    WORD wSecond;
//    WORD wMilliseconds;
//} SYSTEMTIME, *PSYSTEMTIME;

class SystemTime {
    private int _year;
    private int _month;
    private int _dayOfWeek;
    private int _day;
    private int _hour;
    private int _minute;
    private int _second;
    private int _milliseconds;

    public SystemTime(int year, int month, int dayOfWeek, int day, int hour, int minute, int second, int milliseconds) {
        _year = year;
        _month = month;
        _dayOfWeek = dayOfWeek;
        _day = day;
        _hour = hour;
        _minute = minute;
        _second = second;
        _milliseconds = milliseconds;
    }

    public int getYear() {
        return _year;
    }

    public int getMonth() {
        return _month;
    }

    public int getDayOfWeek() {
        return _dayOfWeek;
    }

    public int getDay() {
        return _day;
    }

    public int getHour() {
        return _hour;
    }

    public int getMinute() {
        return _minute;
    }

    public int getSecond() {
        return _second;
    }

    public int getMilliseconds() {
        return _milliseconds;
    }
}
