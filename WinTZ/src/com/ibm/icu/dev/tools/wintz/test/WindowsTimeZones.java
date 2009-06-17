/*
 *******************************************************************************
 * Copyright (C) 2009, International Business Machines Corporation and         *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
package com.ibm.icu.dev.tools.wintz.test;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.TreeMap;

import com.ibm.icu.dev.tools.wintz.TimeZoneRegistry;
import com.ibm.icu.util.BasicTimeZone;
import com.ibm.icu.util.Calendar;
import com.ibm.icu.util.GregorianCalendar;
import com.ibm.icu.util.TimeZone;
import com.ibm.icu.util.TimeZoneTransition;

public class WindowsTimeZones {

    private static List<BasicTimeZone> OLSON_ZONES = null;
    private static List<BasicTimeZone> WINDOWS_ZONES = null;

    static final int STARTYEAR = 2008;
    static final int ENDYEAR = 2009;
    static final int APPROX_DELTA = 2 * 60 * 60 * 1000; // 2 hours

    public static void main(String[] args) {
        long startTime = getYearStart(STARTYEAR);
        long endTime = getYearStart(ENDYEAR + 1);

        StringBuilder buf = new StringBuilder();

        List<BasicTimeZone> winTZs = getWindowsZones();
        for (BasicTimeZone winTZ : winTZs) {
            String winID = winTZ.getID();
            buf.setLength(0);
            buf.append(winID);
            buf.append(" - ");
            buf.append(getWindowsDisplayName(winID));
            buf.append("\n{");
            List<String> equivalents = findOlsonEquivalents(winTZ, startTime, endTime, false);
            if (equivalents != null) {
                for (String equivalentID : equivalents) {
                    buf.append("\n  ");
                    buf.append(equivalentID);
                }
            } else {
                equivalents = findOlsonEquivalents(winTZ, startTime, endTime, true);
                if (equivalents != null) {
                    buf.append("\n  <approximate matches only>");
                    for (String equivalentID : equivalents) {
                        buf.append("\n  ");
                        buf.append(equivalentID);
                    }
                } else {
                    buf.append("\n  <no matches>");
                }
            }
            buf.append("\n}");
            System.out.println(buf);
        }
    }

    static List<String> findOlsonEquivalents(BasicTimeZone winTZ, long start, long end, boolean approxMatch) {
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

        List<String> equivalentOlsonIDs = new ArrayList<String>();
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
                    if (approxMatch) {
                        if (!winTransitions.get(i).approximateMatch(olsonTransitions.get(i), APPROX_DELTA)) {
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
        if (equivalentOlsonIDs.size() == 0) {
            return null;
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
}
