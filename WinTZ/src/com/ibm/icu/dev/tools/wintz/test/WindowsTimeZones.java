/*
 *******************************************************************************
 * Copyright (C) 2009-2011, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
package com.ibm.icu.dev.tools.wintz.test;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.TreeMap;
import java.util.TreeSet;

import com.ibm.icu.dev.tools.wintz.TimeZoneRegistry;
import com.ibm.icu.text.LocaleDisplayNames;
import com.ibm.icu.util.BasicTimeZone;
import com.ibm.icu.util.Calendar;
import com.ibm.icu.util.GregorianCalendar;
import com.ibm.icu.util.TimeZone;
import com.ibm.icu.util.TimeZoneTransition;
import com.ibm.icu.util.ULocale;

public class WindowsTimeZones {

    private static List<BasicTimeZone> OLSON_ZONES = null;
    private static List<BasicTimeZone> WINDOWS_ZONES = null;

    public static void main(String[] args) {
        // Default start year = current year - 1
        GregorianCalendar cal = new GregorianCalendar();
        int startYear = cal.get(Calendar.YEAR) - 1;
        // Default numberOfYears = 1
        int numYears = 1;
        // Default approximate match delta hour
        int deltaH = 2;

        boolean argError = false;
        int argidx = 0;
        while (argidx < args.length) {
            if (args[argidx].length() != 2 || args[argidx].charAt(0) != '-') {
                argError = true;
                break;
            }
            char optChar = args[argidx].charAt(1);
            argidx++;
            if (argidx >= args.length) {
                argError = true;
                break;
            }
            int optVal;
            try {
                optVal = Integer.parseInt(args[argidx]);
            } catch (NumberFormatException e) {
                argError = true;
                break;
            }
            switch (optChar) {
            case 'y':
                startYear = optVal;
                break;
            case 'n':
                if (optVal > 0) {
                    numYears = optVal;
                } else {
                    argError = true;
                }
                break;
            case 'd':
                if (optVal >= 0) {
                    deltaH = optVal;
                } else {
                    argError = true;
                }
                break;
            default:
                argError = true;
            }
            if (argError) {
                break;
            }
            argidx++;
        }

        if (argError) {
            System.out.println("Invalid command line arguments.");
            System.out.println("Available options:");
            System.out.println(" -y <start year>");
            System.out.println(" -n <number of years>");
            System.out.println(" -d <hour delta for approximate match>");
            return;
        }

        System.out.println("####################################");
        System.out.println("# Windows - Olson Time Zone mapping");
        System.out.println("#  start year:             " + startYear);
        System.out.println("#  number of year(s):      " + numYears);
        System.out.println("#  approx match hour(+/-): " + deltaH);
        System.out.println("####################################");

        long startTime = getYearStart(startYear);
        long endTime = getYearStart(startYear + numYears);

        StringBuilder buf = new StringBuilder();

        List<BasicTimeZone> winTZs = getWindowsZones();
        for (BasicTimeZone winTZ : winTZs) {
            String winID = winTZ.getID();
            buf.setLength(0);
            buf.append(winID);
            buf.append(" - ");
            buf.append(getWindowsDisplayName(winID));
            buf.append("\n{");
            Set<String> equivalents = findOlsonEquivalents(winTZ, startTime, endTime, 0);
            if (!equivalents.isEmpty()) {
                buf.append("\n  # Exact Matches");
                for (String equivalentID : equivalents) {
                    buf.append("\n    ");
                    appendZoneInformation(buf, equivalentID);
                }
            }
            Set<String> equivalentsApprox = findOlsonEquivalents(winTZ, startTime, endTime, deltaH * 60 * 60 * 1000);
            if (!equivalentsApprox.isEmpty()) {
                for (String exactMatch : equivalents) {
                    equivalentsApprox.remove(exactMatch);
                }
                if (!equivalentsApprox.isEmpty()) {
                    buf.append("\n  # Approximate Matches");
                    for (String equivalentID : equivalentsApprox) {
                        buf.append("\n    ");
                        appendZoneInformation(buf, equivalentID);
                    }
                }
            }
            if (equivalents.isEmpty() && equivalentsApprox.isEmpty()) {
                buf.append("\n  # No Matches");
            }
            buf.append("\n}");
            System.out.println(buf);
        }
    }

    static Set<String> findOlsonEquivalents(BasicTimeZone winTZ, long start, long end, int maxTransitionTimeDelta) {
        long t;

        int winTZOffset = winTZ.getOffset(start);
        ArrayList<TransitionInfo> winTransitions = new ArrayList<TransitionInfo>();
        t = start;
        while (t < end) {
            TimeZoneTransition tzt = winTZ.getNextTransition(t, false);
            if (tzt == null) {
                break;
            }
            TransitionInfo tri = new TransitionInfo(tzt.getTime(), tzt.getTo().getRawOffset() + tzt.getTo().getDSTSavings());
            winTransitions.add(tri);
            t = tzt.getTime();
        }
        int winTransitionCount = winTransitions.size();

        Set<String> equivalentOlsonIDs = new TreeSet<String>();
        for (BasicTimeZone olsonTZ : getOlsonZones()) {
            int olsonTZOffset = olsonTZ.getOffset(start);

            if (olsonTZOffset != winTZOffset) {
                continue;
            }

            ArrayList<TransitionInfo> olsonTransitions = new ArrayList<TransitionInfo>();
            t = start;
            while (t < end) {
                TimeZoneTransition tzt = olsonTZ.getNextTransition(t, false);
                if (tzt == null) {
                    break;
                }
                TransitionInfo tri = new TransitionInfo(tzt.getTime(), tzt.getTo().getRawOffset() + tzt.getTo().getDSTSavings());
                olsonTransitions.add(tri);
                t = tzt.getTime();
            }
            int olsonTransitionCount = olsonTransitions.size();

            if (winTransitionCount != olsonTransitionCount) {
                continue;
            }

            if (winTransitionCount == 0) {
                equivalentOlsonIDs.add(olsonTZ.getID());
            } else {
                // compare transitions
                boolean match = true;
                for (int i = 0; i < winTransitionCount; i++) {
                    if (maxTransitionTimeDelta > 0) {
                        if (!winTransitions.get(i).approximateMatch(olsonTransitions.get(i), maxTransitionTimeDelta)) {
                            match = false;
                            break;
                        }
                    } else {
                        if (!winTransitions.get(i).equals(olsonTransitions.get(i))) {
                            match = false;
                            break;
                        }
                    }
                }
                if (match) {
                    equivalentOlsonIDs.add(olsonTZ.getID());
                }
            }
        }
        return equivalentOlsonIDs;
    }

    static List<BasicTimeZone> getOlsonZones() {
        if (OLSON_ZONES == null) {
            TreeMap<String, BasicTimeZone> olsonMap = new TreeMap<String, BasicTimeZone>();
            String[] olsonIDs = TimeZone.getAvailableIDs();
            for (String olsonID : olsonIDs) {
                String canonical = TimeZone.getCanonicalID(olsonID);
                if (!canonical.equals(olsonID)) {
                    continue;
                }
                BasicTimeZone tz = (BasicTimeZone)TimeZone.getTimeZone(olsonID);
                olsonMap.put(olsonID, tz);
            }
            OLSON_ZONES = new ArrayList<BasicTimeZone>();
            Set<String> ids = olsonMap.keySet();
            for (String olsonID : ids) {
                OLSON_ZONES.add(olsonMap.get(olsonID));
            }
        }
        return OLSON_ZONES;
    }

    static List<BasicTimeZone> getWindowsZones() {
        if (WINDOWS_ZONES == null) {
            WINDOWS_ZONES = new ArrayList<BasicTimeZone>();
            TimeZoneRegistry reg = TimeZoneRegistry.get();
            Set<String> winIDs = reg.getAvailableTZIDs();
            for (String winID : winIDs) {
                BasicTimeZone tz = reg.getTimeZone(winID);
                WINDOWS_ZONES.add(tz);
            }
        }
        return WINDOWS_ZONES;
    }

    static String getWindowsDisplayName(String tzid) {
        TimeZoneRegistry reg = TimeZoneRegistry.get();
        return reg.getDisplayName(tzid);
    }

    static class TransitionInfo {
        long _time;
        int _offset;

        TransitionInfo(long time, int offset) {
            _time = time;
            _offset = offset;
        }

        boolean equals(TransitionInfo other) {
            return (_time == other._time) && (_offset == other._offset);
        }

        boolean approximateMatch(TransitionInfo other, int maxTimeDelta) {
            return (Math.abs(_time - other._time) < maxTimeDelta) && (_offset == other._offset);
        }
    }

    static long getYearStart(int year) {
        GregorianCalendar gcal = new GregorianCalendar(TimeZone.getTimeZone("Etc/GMT"));
        gcal.clear();
        gcal.set(year, Calendar.JANUARY, 1, 0, 0, 0);
        return gcal.getTimeInMillis();
    }

    static LocaleDisplayNames LOCDISPNAMS = LocaleDisplayNames.getInstance(ULocale.US);

    static void appendZoneInformation(StringBuilder buf, String id) {
        buf.append(id);
        buf.append(" [");
        String region = TimeZone.getRegion(id);
        if (!region.equals("001")) {
            buf.append(region).append(":").append(LOCDISPNAMS.regionDisplayName(region));
        } else {
            buf.append("--");
        }
        buf.append("]");        
    }
}
