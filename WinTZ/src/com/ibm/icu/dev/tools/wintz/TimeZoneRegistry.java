/*
 *******************************************************************************
 * Copyright (C) 2009-2011, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
package com.ibm.icu.dev.tools.wintz;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.Map.Entry;
import java.util.Set;
import java.util.TreeMap;
import java.util.TreeSet;

import com.ibm.icu.util.AnnualTimeZoneRule;
import com.ibm.icu.util.BasicTimeZone;
import com.ibm.icu.util.DateTimeRule;
import com.ibm.icu.util.InitialTimeZoneRule;
import com.ibm.icu.util.RuleBasedTimeZone;
import com.ibm.icu.util.SimpleTimeZone;

public class TimeZoneRegistry {

    private static TimeZoneRegistry INSTANCE;

    private static final String TZREG_ROOT = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones";
    private static final String DYNDST_SUFFIX = "\\Dynamic DST";

    private static final int STARTYEAR = 1900;

    private int _tzVersion;
    private TreeMap<String, RegTimeZoneInformation> _timezones;
    private TreeMap<String, RegDynamicDST> _dynamicDsts;


    private void initialize(InputStreamReader reader) throws IOException {
        BufferedReader r = new BufferedReader(reader);

        _timezones = new TreeMap<String, RegTimeZoneInformation>();
        _dynamicDsts = new TreeMap<String, RegDynamicDST>();

        boolean next = true;
        boolean inTzRegRoot = false;
        RegTimeZoneInformation timezone = null;
        RegDynamicDST dynamicDST = null;
        StringBuilder lineBuf = new StringBuilder();

        while (next) {
            RegTimeZoneInformation nextTimezone = null;
            RegDynamicDST nextDynamicDST = null;

            String line = r.readLine();
            if (line == null) {
                next = false;
            } 
            else {
                line = line.trim();
                if (line.endsWith("\\")) {
                    // contiguous line
                    lineBuf.append(line.substring(0, line.length() - 1));
                    continue;
                } else if (line.startsWith("[") && line.endsWith("]")) {
                    // new key
                    String newKey = line.trim().substring(1, line.length() - 1);
                    if (newKey.equals(TZREG_ROOT)) {
                        inTzRegRoot = true;
                        lineBuf.setLength(0);
                    } else if (newKey.startsWith(TZREG_ROOT + "\\")) {
                        inTzRegRoot = false;
                        String subKey = newKey.substring(TZREG_ROOT.length() + 1);
                        if (subKey.endsWith(DYNDST_SUFFIX)) {
                            int idx = subKey.indexOf(DYNDST_SUFFIX);
                            String tzid = subKey.substring(0, idx);
                            nextDynamicDST = new RegDynamicDST(tzid);
                            _dynamicDsts.put(tzid, nextDynamicDST);
                        } else {
                            nextTimezone = new RegTimeZoneInformation(subKey);
                            _timezones.put(subKey, nextTimezone);
                        }
                    }
                } else {
                    lineBuf.append(line);
                }
            }

            if (lineBuf.length() > 0) {
                if (inTzRegRoot) {
                    String regDataLine = lineBuf.toString();
                    if ("TzVersion".equals(RegUtil.getKey(regDataLine))) {
                        _tzVersion = RegUtil.getDwordValue(regDataLine);
                    }
                    lineBuf.setLength(0);
                } else if (timezone != null) {
                    timezone.set(lineBuf.toString());
                    lineBuf.setLength(0);
                } else if (dynamicDST != null) {
                    dynamicDST.set(lineBuf.toString());
                    lineBuf.setLength(0);
                }
            }

            if (nextTimezone != null) {
                timezone = nextTimezone;
                dynamicDST = null;
                lineBuf.setLength(0);
            } else if (nextDynamicDST != null) {
                dynamicDST = nextDynamicDST;
                timezone = null;
                lineBuf.setLength(0);
            }
        }

        r.close();
    }

    public static TimeZoneRegistry get() {
        if (INSTANCE == null) {
            try {
                InputStream is = ClassLoader.getSystemResourceAsStream("com/ibm/icu/dev/tools/wintz/TimeZone.reg");
                if (is == null) {
                    throw new RuntimeException("Missing TimeZone.reg");
                }

                INSTANCE = new TimeZoneRegistry();
                INSTANCE.initialize(new InputStreamReader(is, "UTF-16LE"));
            } catch (IOException e) {
                INSTANCE = null;
            }
        }
        return INSTANCE;
    }

    private TimeZoneRegistry() {
    }

    public String getTzVersionString() {
        return String.format("%x", _tzVersion);
    }

    public int getTzVersion() {
        return _tzVersion;
    }

    public Set<String> getAvailableTZIDs() {
        return getAvailableTZIDs(false);
    }

    public Set<String> getAvailableTZIDs(boolean activeOnly) {
        if (activeOnly) {
            Set<String> activeIDs = new TreeSet<String>();
            for (Entry<String, RegTimeZoneInformation> entry : _timezones.entrySet()) {
                if (!entry.getValue().isObsolete()) {
                    activeIDs.add(entry.getKey());
                }
            }
            return activeIDs;
        }
        return _timezones.keySet();
    }

    public boolean isObsolete(String tzid) {
        return getTZInfo(tzid).isObsolete();
    }

    public String getDisplayName(String tzid) {
        return getTZInfo(tzid).getDisplay();
    }

    public BasicTimeZone getTimeZone(String tzid) {
        RegTimeZoneInformation tzInfo = getTZInfo(tzid);

        int offset, stdOffset, dstOffset, dstSavings;
        SystemTime stdDate, dstDate;
        TZI tzi;
        BasicTimeZone btz = null;

        // Check if dynamic DST is available for this zone
        RegDynamicDST dynDst = _dynamicDsts.get(tzid);

        if (dynDst != null) {

            // dynamic DST is available
            // Although MS does not provide any documentation, we assume date in or before
            // the year specified by "FirstEntry" uses the rule associated with the year (first).
            // Also, date in or after the year specified by "LastEntry" uses the rule associated
            // with the year (last).
            int firstYear = dynDst.getFirstYear();
            int lastYear = dynDst.getLastYear();

            final String stdName = tzInfo.getStd();
            String dstName;
            DateTimeRule dtRuleStd, dtRuleDst;

            // the first rule
            tzi = dynDst.getTZI(Integer.valueOf(firstYear));
            offset = biasToOffset(tzi.getBias());
            stdDate = tzi.getStandardDate();
            stdOffset = offset + biasToOffset(tzi.getStandardBias());

            if (stdDate.getMonth() == 0) {
                // month field = 0 indicates that the zone does not observe DST for the year
                // creating a pair of fake rules
                dstName = stdName;
                dstSavings = 0;

                dtRuleStd = new DateTimeRule(0, 1, 0, DateTimeRule.WALL_TIME);  // Jan 1, 00:00:00.000
                dtRuleDst = new DateTimeRule(0, 1, 1, DateTimeRule.WALL_TIME);  // Jan 1, 00:00:00.001
            } else {
                dstName = tzInfo.getDlt();

                dstDate = tzi.getDaylightDate();
                dstOffset = offset + biasToOffset(tzi.getDaylightBias());
                dstSavings = dstOffset - stdOffset;

                dtRuleStd = createDateTimeRule(stdDate);
                dtRuleDst = createDateTimeRule(dstDate);
            }

            // Initial rule
            InitialTimeZoneRule initialRule = new InitialTimeZoneRule(tzInfo.getStd(), stdOffset, 0);
            RuleBasedTimeZone rbtz = new RuleBasedTimeZone(tzid, initialRule);
 
            int year = firstYear + 1;
            AnnualTimeZoneRule atzRule;
            int startYear = STARTYEAR;

            while (true) {
                tzi = dynDst.getTZI(Integer.valueOf(year));
                if (tzi == null) {
                    continue;
                }
                // emit the pair of rule
                atzRule = new AnnualTimeZoneRule(stdName, stdOffset, 0, dtRuleStd, startYear, year - 1);
                rbtz.addTransitionRule(atzRule);
                atzRule = new AnnualTimeZoneRule(dstName, stdOffset, dstSavings, dtRuleDst, startYear, year - 1);
                rbtz.addTransitionRule(atzRule);

                // create a new pair of rule
                offset = biasToOffset(tzi.getBias());
                stdDate = tzi.getStandardDate();
                stdOffset = offset + biasToOffset(tzi.getStandardBias());

                if (stdDate.getMonth() == 0) {
                    // month field = 0 indicates that the zone does not observe DST for the year
                    // creating a pair of fake rules
                    dstName = stdName;
                    dstSavings = 0;

                    dtRuleStd = new DateTimeRule(0, 1, 0, DateTimeRule.WALL_TIME);  // Jan 1, 00:00:00.000
                    dtRuleDst = new DateTimeRule(0, 1, 1, DateTimeRule.WALL_TIME);  // Jan 1, 00:00:00.001
                } else {
                    dstName = tzInfo.getDlt();

                    dstDate = tzi.getDaylightDate();
                    dstOffset = offset + biasToOffset(tzi.getDaylightBias());
                    dstSavings = dstOffset - stdOffset;

                    dtRuleStd = createDateTimeRule(stdDate);
                    dtRuleDst = createDateTimeRule(dstDate);
                }

                startYear = year;

                if (year == lastYear) {
                    // emit the last rule and break
                    atzRule = new AnnualTimeZoneRule(stdName, stdOffset, 0, dtRuleStd, startYear, AnnualTimeZoneRule.MAX_YEAR);
                    rbtz.addTransitionRule(atzRule);
                    atzRule = new AnnualTimeZoneRule(dstName, stdOffset, dstSavings, dtRuleDst, startYear, AnnualTimeZoneRule.MAX_YEAR);
                    rbtz.addTransitionRule(atzRule);
                    break;
                }
                year++;
            }

            btz = rbtz;

        } else {
            // No dynamic DST is available
            tzi = tzInfo.getTZI();
            offset = biasToOffset(tzi.getBias());
            stdDate = tzi.getStandardDate();
            // month field = 0 indicates that the zone does not observe DST
            if (stdDate.getMonth() == 0) {
                btz = new SimpleTimeZone(offset, tzid);
            } else {
                dstDate = tzi.getDaylightDate();

                stdOffset = offset + biasToOffset(tzi.getStandardBias());
                dstOffset = offset + biasToOffset(tzi.getDaylightBias());
                dstSavings = dstOffset - stdOffset;

                // hack..  Namibia Standard Time has negative dstSavings (positive DaylightBias).
                // It looks this is a bug.  ICU SimpleTimeZone does not allow negative dstSavings,
                // we'll swap the standard time and daylight saving time
                if (dstSavings < 0) {
                    SystemTime tmpDate = stdDate;
                    stdDate = dstDate;
                    dstDate = tmpDate;

                    int tmpOffset = stdOffset;
                    stdOffset = dstOffset;
                    dstOffset = tmpOffset;
                    dstSavings = (-1) * dstSavings;
                }

/*
                int startDayOfWeek, endDayOfWeek, startDay, endDay;

                if (dstDate.getDayOfWeek() == 0) {
                    // day of month
                    startDayOfWeek = 0;
                    startDay = dstDate.getDay();
                } else {
                    // Nth day of week in a month
                    startDayOfWeek = dstDate.getDayOfWeek() + 1;                // ICU uses Sunday = 1, while MS uses Sunday = 0
                    startDay = (dstDate.getDay() == 5) ? -1 : dstDate.getDay(); // 5 means last week in a month
                }

                if (stdDate.getDayOfWeek() == 0) {
                    // day of month
                    endDayOfWeek = 0;
                    endDay = stdDate.getDay();
                } else {
                    // Nth day of week in a month
                    endDayOfWeek = stdDate.getDayOfWeek() + 1;
                    endDay = (stdDate.getDay() == 5) ? -1 : stdDate.getDay();
                }

                btz = new SimpleTimeZone(stdOffset, tzid,
                        dstDate.getMonth() - 1,                             // ICU uses 0-based month
                        startDay,
                        startDayOfWeek,
                        wallTimeToMillis(dstDate.getHour(), dstDate.getMinute(), dstDate.getSecond(), dstDate.getMilliseconds()),
                        SimpleTimeZone.WALL_TIME,
                        stdDate.getMonth() - 1,
                        endDay,
                        endDayOfWeek,
                        wallTimeToMillis(stdDate.getHour(), stdDate.getMinute(), stdDate.getSecond(), stdDate.getMilliseconds()),
                        SimpleTimeZone.WALL_TIME,
                        dstSavings);
*/
                btz = new SimpleTimeZone(stdOffset, tzid,
                        dstDate.getMonth() - 1,                             // ICU uses 0-based month
                        (dstDate.getDay() == 5) ? -1 : dstDate.getDay(),
                        dstDate.getDayOfWeek() + 1,
                        wallTimeToMillis(dstDate.getHour(), dstDate.getMinute(), dstDate.getSecond(), dstDate.getMilliseconds()),
                        SimpleTimeZone.WALL_TIME,
                        stdDate.getMonth() - 1,
                        (stdDate.getDay() == 5) ? -1 : stdDate.getDay(),
                        stdDate.getDayOfWeek() + 1,
                        wallTimeToMillis(stdDate.getHour(), stdDate.getMinute(), stdDate.getSecond(), stdDate.getMilliseconds()),
                        SimpleTimeZone.WALL_TIME,
                        dstSavings);
            }
        }

        return btz;
    }

    private RegTimeZoneInformation getTZInfo(String tzid) {
        RegTimeZoneInformation tzInfo = _timezones.get(tzid);
        if (tzInfo == null) {
            throw new IllegalArgumentException("Unknown Windows TZID: " + tzid);
        }
        return tzInfo;
    }

    private static int biasToOffset(int bias) {
        // Windows represents offset in minutes.  Also, sign is opposite.
        return (-1) * bias * 60 * 1000;
    }

    private static int wallTimeToMillis(int hour, int minute, int second, int milliseconds) {
        return (((hour * 60) + minute) * 60 + second) * 1000 + milliseconds;
    }

    private static DateTimeRule createDateTimeRule(SystemTime rule) {
        return new DateTimeRule(rule.getMonth() - 1,
                        (rule.getDay() == 5) ? -1 : rule.getDay(),
                        rule.getDayOfWeek() + 1,
                        wallTimeToMillis(rule.getHour(), rule.getMinute(), rule.getSecond(), rule.getMilliseconds()),
                        DateTimeRule.WALL_TIME);
    }
}
