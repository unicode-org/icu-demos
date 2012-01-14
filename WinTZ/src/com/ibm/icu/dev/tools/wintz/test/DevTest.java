/*
 *******************************************************************************
 * Copyright (C) 2009-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
package com.ibm.icu.dev.tools.wintz.test;

import java.util.Date;
import java.util.Set;

import com.ibm.icu.dev.tools.wintz.RegUtil;
import com.ibm.icu.dev.tools.wintz.TimeZoneRegistry;
import com.ibm.icu.text.SimpleDateFormat;
import com.ibm.icu.util.BasicTimeZone;
import com.ibm.icu.util.Calendar;
import com.ibm.icu.util.GregorianCalendar;
import com.ibm.icu.util.TimeZone;
import com.ibm.icu.util.TimeZoneRule;
import com.ibm.icu.util.TimeZoneTransition;

public class DevTest {
    public static void main(String[] args) {
//        regUtilKeyValue();
//        regUtilHexValue();
//        regUtilDwordValue();
//        timezoneRegistry();
        dumpWindowsTimeZones();
    }

    static void regUtilKeyValue() {
        System.out.println("-- regUtilKeyValue --");
        String line = "\"key1\"=\"value1\"";
        String key = RegUtil.getKey(line);
        String val = RegUtil.getStringValue(line);

        System.out.println(key + " = " + val);
    }

    static void regUtilHexValue() {
        System.out.println("-- regUtilHexValue --");
        String line = "\"key1\"=hex:01,02,03,04";
        byte[] bin = RegUtil.getBinaryValue(line);
        for (int i = 0; i < bin.length; i++) {
            System.out.println(bin[i]);
        }
    }

    static void regUtilDwordValue() {
        System.out.println("-- regUtilIntValue --");
        String line = "\"key1\"=dword:80009010";
        int intval = RegUtil.getDwordValue(line);
        System.out.println(intval);
    }

    static void timezoneRegistry() {
        long t = System.currentTimeMillis();
        TimeZoneRegistry reg = TimeZoneRegistry.get();
        Set<String>IDs = reg.getAvailableTZIDs();
        int[] offsets = new int[2];
        for (String s : IDs) {
            BasicTimeZone tz = reg.getTimeZone(s);
            tz.getOffset(t, false, offsets);
            TimeZoneTransition tzt = tz.getNextTransition(t, false);
            if (tzt == null) {
                System.out.println("tzid: " + s + ", current offset: " + offsets[0] + " [NODST]");
            } else {
                System.out.println("tzid: " + s + ", current offset: " + offsets[0] + ", next transition: " + tzt.getTime());
            }
        }
    }

    static void dumpWindowsTimeZones() {
        TimeZoneRegistry reg = TimeZoneRegistry.get();
        Set<String>winIDs = reg.getAvailableTZIDs();
        int[] offsets = new int[2];

        GregorianCalendar greg = new GregorianCalendar(TimeZone.getTimeZone("Etc/GMT"));
        int currentYear = greg.get(Calendar.YEAR);

        greg.clear();
        greg.set(2006, Calendar.JANUARY, 1, 0, 0, 0);   // Windows historic rules are available back to 2006
        final long start = greg.getTimeInMillis();

        greg.clear();
        greg.set(currentYear + 1, Calendar.JANUARY, 1, 0, 0, 0);
        final long end = greg.getTimeInMillis();

        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");

        for (String tzid : winIDs) {
            BasicTimeZone tz = reg.getTimeZone(tzid);
            sdf.setTimeZone(tz);

            System.out.println("\n\nTZID: " + tzid + " -----------------------------------------");

            long t = start;
            TimeZoneTransition tzt;
            int transCount = 0;

            while (t < end) {
                tzt = tz.getNextTransition(t, false);
                if (tzt == null) {
                    break;
                }
                transCount++;
                t = tzt.getTime();
                Date d = new Date(t);
                tz.getOffset(t, false, offsets);
                System.out.println("[TRANSITION " + transCount + "] " + sdf.format(d) + " offset: " + offsetToTime(offsets[0] + offsets[1]) + (offsets[1] == 0 ? "(STD)" : "(DST)"));
//                TimeZoneRule from = tzt.getFrom();
                TimeZoneRule to = tzt.getTo();
//                System.out.println("    from rule: " + from.toString());
                System.out.println("    to rule  : " + to.toString());
            }

            if (transCount == 0) {
                tz.getOffset(t, false, offsets);
                System.out.println("[NO TRANSITION] offset: " + offsetToTime(offsets[0] + offsets[1]) + (offsets[1] == 0 ? "(STD)" : "(DST)"));
            }
        }
    }

    private static String offsetToTime(int t) {
        int sign = t < 0 ? -1 : 1;
        int hour, min, sec, millis;

        t = t * sign;
        millis = t % 1000;
        t /= 1000;
        sec = t % 60;
        t /= 60;
        min = t % 60;
        hour = t / 60;

        StringBuilder buf = new StringBuilder();
        if (sign < 0) {
            buf.append("-");
        } else {
            buf.append("+");
        }
        if (hour < 10) {
            buf.append("0");
        }
        buf.append(hour);
        buf.append(":");
        if (min < 10) {
            buf.append("0");
        }
        buf.append(min);

        if (sec > 0 || millis > 0) {
            buf.append(":");
            if (min < 10) {
                buf.append("0");
            }
            buf.append(sec);
            if (millis > 0) {
                buf.append(":");
                if (millis < 10) {
                    buf.append("00");
                } else if (millis < 100) {
                    buf.append("0");
                }
                buf.append(millis);
            }
        }

        return buf.toString();
    }
}
