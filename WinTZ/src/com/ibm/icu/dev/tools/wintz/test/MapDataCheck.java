/*
 *******************************************************************************
 * Copyright (C) 2011-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
package com.ibm.icu.dev.tools.wintz.test;

import java.io.PrintWriter;
import java.io.Writer;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Map.Entry;
import java.util.Set;
import java.util.SortedMap;
import java.util.TreeMap;
import java.util.TreeSet;

import com.ibm.icu.dev.tools.wintz.TimeZoneRegistry;
import com.ibm.icu.dev.tools.wintz.mapper.ExemplarLocationUtil;
import com.ibm.icu.dev.tools.wintz.mapper.MapDataUtil;
import com.ibm.icu.dev.tools.wintz.mapper.MapDataUtil.MapDataEntry;
import com.ibm.icu.util.BasicTimeZone;
import com.ibm.icu.util.Calendar;
import com.ibm.icu.util.GregorianCalendar;
import com.ibm.icu.util.TimeZone;
import com.ibm.icu.util.TimeZone.SystemTimeZoneType;

public class MapDataCheck {

    public static void main(String... args) {
        MapDataCheck checker = new MapDataCheck();
        checker.printInfo();
        checker.testVersion();
        checker.testWindowsIdSet();
        checker.testZoneIdSet();
        checker.testWindowsExemplarLocations();
        checker.testDuplicate();
        checker.testCanonical();
        checker.testRegion();
        checker.testNoBaseOffsetMatches();
        checker.testNoRuleMatches();
        checker.testZoneOffsets();
        checker.testDefaultMap();
        checker.testRegionalDefaultMap();
        checker.testUnmappableWindowsZones();
    }

    private PrintWriter _pw;
    private int _referenceYear;
    private int _maxmumIncompatibleDaysAllowed = 7;
    private static TimeZoneRegistry WINTZREG = TimeZoneRegistry.get();

    public MapDataCheck() {
        _pw = new PrintWriter(System.out, true);
        GregorianCalendar cal = new GregorianCalendar();
        _referenceYear = cal.get(Calendar.YEAR);
    }

    public MapDataCheck(Writer out, int referenceYear) {
        _pw = new PrintWriter(out, true);
        _referenceYear = referenceYear;
    }

    public void printInfo() {
        _pw.println("-------------------------------------------------");
        _pw.println("MapDataCheck");
        _pw.println("  Reference Year:                  " + _referenceYear);
        _pw.println("  Maximum incompable days allowed: " + _maxmumIncompatibleDaysAllowed);
        _pw.println("-------------------------------------------------");
    }

    public void testVersion() {
        _pw.println("### Checking zone data versions");
    
        boolean isOK = true;

        if (!TimeZone.getTZDataVersion().equals(MapDataUtil.getTZDataVersion())) {
            isOK = false;
            _pw.println("[ERROR] tzdata version mismatch - MapData: " + MapDataUtil.getTZDataVersion() + "/ICU: " + TimeZone.getTZDataVersion());
        }
        if (WINTZREG.getTzVersionString().equals(MapDataUtil.getWINTZDataVersion())) {
            isOK = false;
            _pw.println("[ERROR] Windows timezone version mismatch - MapData: " + MapDataUtil.getWINTZDataVersion() + "/Windows Registry Data: " + WINTZREG.getTzVersionString());
        }

        if (isOK) {
            _pw.println("[OK]");
        }
    }

    public void testWindowsIdSet() {
        _pw.println("### Checking available Windows IDs");
        Set<String> regWinIDs = WINTZREG.getAvailableTZIDs(true);
        Set<String> dataWinIDs = MapDataUtil.getAllKnownWindowsIDs();

        boolean isOK = true;

        if (!dataWinIDs.containsAll(regWinIDs)) {
            isOK = false;
            _pw.println("[ERROR] Missing Windows IDs in MapData");
            _pw.println("{");
            for (String winID : regWinIDs) {
                if (!dataWinIDs.contains(winID)) {
                    _pw.println(winID);
                }
            }
            _pw.println("}");
        }
        if (!regWinIDs.containsAll(dataWinIDs)) {
            isOK = false;
            _pw.println("[ERROR] Windows IDs no longer available in the registry");
            _pw.println("{");
            for (String winID : dataWinIDs) {
                if (!regWinIDs.contains(winID)) {
                    _pw.println(winID);
                }
            }
            _pw.println("}");
        }
        if (isOK) {
            _pw.println("[OK]");
        }
    }

    public void testZoneIdSet() {
        _pw.println("### Checking available Zone IDs");

        Set<String> dataIDs = MapDataUtil.getAllKnownIDs();
        Set<String> canonicalIDs = TimeZone.getAvailableIDs(SystemTimeZoneType.CANONICAL, null, null);

        boolean isOK = true;

        if (!dataIDs.containsAll(canonicalIDs)) {
            isOK = false;
            _pw.println("[ERROR] Missing zone IDs in MapData");
            _pw.println("{");
            for (String id : canonicalIDs) {
                if (!dataIDs.contains(id)) {
                    _pw.println(id);
                }
            }
            _pw.println("}");
        }
        if (!canonicalIDs.containsAll(dataIDs)) {
            isOK = false;
            _pw.println("[ERROR] IDs not available in the tz database, or not canonical");
            _pw.println("{");
            for (String dataID : dataIDs) {
                if (!canonicalIDs.contains(dataID)) {
                    _pw.println(dataID);
                }
            }
            _pw.println("}");
        }
        if (isOK) {
            _pw.println("[OK]");
        }
    }

    public void testWindowsExemplarLocations() {
        _pw.println("### Checking if ExemplarLocationData contains all Windows exemplar locations");
        Set<String> regWinIDs = WINTZREG.getAvailableTZIDs(true);

        Set<String> knownLocations = ExemplarLocationUtil.getExemplarLocations();
        Set<String> regLocations = new TreeSet<String>();

        for (String winID : regWinIDs) {
            Set<String> locations = ExemplarLocationUtil.extractLocationsFromDisplayName(WINTZREG.getDisplayName(winID));
            regLocations.addAll(locations);
        }

        boolean isOK = true;

        if (!knownLocations.containsAll(regLocations)) {
            isOK = false;
            _pw.println("[ERROR] Missing Windows exemplar locations in ExemplarLocationData");
            _pw.println("{");
            for (String regLoc : regLocations) {
                if (!knownLocations.contains(regLoc)) {
                    _pw.println(regLoc);
                }
            }
            _pw.println("}");
        }
        if (!regLocations.containsAll(knownLocations)) {
            isOK = false;
            _pw.println("[ERROR] Windows exemplar locations no longer available in the registry");
            _pw.println("{");
            for (String knownLoc : knownLocations) {
                if (!regLocations.contains(knownLoc)) {
                    _pw.println(knownLoc);
                }
            }
            _pw.println("}");
        }
        if (isOK) {
            _pw.println("[OK]");
        }
    }

    public void testDuplicate() {
        _pw.println("### Checking duplicated Olson ID mappings");

        Set<String> olsonIDs = new HashSet<String>();
        Set<String> dupIDs = new TreeSet<String>();

        for (MapDataEntry entry : MapDataUtil.getAllEntriesInMapData()) {
            String id = entry.olsonID();
            if (olsonIDs.contains(id)) {
                dupIDs.add(id);
            } else {
                olsonIDs.add(id);
            }
        }

        if (dupIDs.isEmpty()) {
            _pw.println("[OK]");
        } else {
            _pw.println("[ERROR] Duplicated Olson ID mappings");
            _pw.println("{");
            for (String dup : dupIDs) {
                _pw.println(dup);
            }
            _pw.println("}");
        }

    }

    public void testCanonical() {
        _pw.println("### Checking if Olson IDs in the mapping data are canonical");
        Set<String> olsonIDs = MapDataUtil.getAllKnownIDs();
        boolean isOK = true;
        for (String id : olsonIDs) {
            String canonical = TimeZone.getCanonicalID(id);
            if (!id.equals(canonical)) {
                if (isOK) {
                    _pw.println("[ERROR] Non-canonical Olson IDs");
                    _pw.println("{");
                    isOK = false;
                }
                _pw.println(id + "[" + canonical + "]");
            }
        }
        if (isOK) {
            _pw.println("[OK]");
        } else {
            _pw.println("}");
        }
    }

    public void testRegion() {
        _pw.println("### Checking if Olson ID's region in the mapping data is correct");

        boolean isOK = true;

        for (MapDataEntry entry : MapDataUtil.getAllEntriesInMapData()) {
            String dataRegion = entry.region();
            if (dataRegion.equals("ZZ")) {
                // MapData uses "ZZ" for non-location zones, while ICU uses "001" when unknown.
                dataRegion = "001";
            }
            String id = entry.olsonID();
            String region = TimeZone.getRegion(id);
            if (!dataRegion.equals(region)) {
                if (isOK) {
                    _pw.println("[ERROR] Incorrect Olson time zone's region");
                    _pw.println("{");
                    isOK = false;
                }
                _pw.println(id + ": " + dataRegion + " -> " + region);
            }
        }

        if (isOK) {
            _pw.println("[OK]");
        } else {
            _pw.println("}");
        }
    }

    public void testNoBaseOffsetMatches() {
        _pw.println("### Checking if no base offset match zones do not have any base offset match in Windows zones");
        Set<String> regWinIDs = WINTZREG.getAvailableTZIDs(true);
        List<TimeZone> winZones = new ArrayList<TimeZone>(regWinIDs.size());

        for (String winID : regWinIDs) {
            winZones.add(WINTZREG.getTimeZone(winID));
        }

        long date = System.currentTimeMillis();
        int[] offsets = new int[2];
        int[] winOffsets = new int[2];
        StringBuilder buf = new StringBuilder();

        Set<String> noBaseOffsetMatchIDs = MapDataUtil.getNoBaseOffestMatchZones();

        for (String noMatchZoneID : noBaseOffsetMatchIDs) {
            TimeZone noMatchZone = TimeZone.getTimeZone(noMatchZoneID);
            noMatchZone.getOffset(date, false, offsets);
            for (TimeZone winZone : winZones) {
                winZone.getOffset(date, false, winOffsets);
                if (offsets[0] == winOffsets[0]) {
                    buf.append(noMatchZoneID);
                    buf.append(" - Windows time zone: ");
                    buf.append(winZone.getID());
                    buf.append(" has the same base offset - ");
                    buf.append(ms2HourMin(offsets[0]));
                    buf.append("\n");
                    break;
                }
            }
        }

        if (buf.length() == 0) {
            _pw.println("[OK]");
        } else {
            _pw.println("[ERROR] Base offset matches");
            _pw.println("{");
            _pw.println(buf.append("}"));
        }
    }

    private void testNoRuleMatches() {
        _pw.println("### Checking if no rule match zones do not have any exact rule match in Windows zones");
        Set<String> regWinIDs = WINTZREG.getAvailableTZIDs(true);
        List<BasicTimeZone> winZones = new ArrayList<BasicTimeZone>(regWinIDs.size());

        for (String winID : regWinIDs) {
            winZones.add(WINTZREG.getTimeZone(winID));
        }

        Set<String> noDstRuleMatchIDs = MapDataUtil.getNoDstRuleMatchZones();
        StringBuilder buf = new StringBuilder();
        for (String noMatchZoneID : noDstRuleMatchIDs) {
            BasicTimeZone noMatchZone = (BasicTimeZone)TimeZone.getTimeZone(noMatchZoneID, TimeZone.TIMEZONE_ICU);

            long start, end;
            // Beginning of the reference year
            GregorianCalendar cal = new GregorianCalendar(noMatchZone);
            cal.clear();
            cal.set(_referenceYear, Calendar.JANUARY, 1, 0, 0, 0);
            start = cal.getTimeInMillis();
            // End of the reference year
            cal.add(Calendar.YEAR, 1);
            end = cal.getTimeInMillis() - 1;

            for (BasicTimeZone winZone : winZones) {
                if (noMatchZone.hasEquivalentTransitions(winZone, start, end)) {
                    buf.append(noMatchZoneID);
                    buf.append(" - Windows time zone: ");
                    buf.append(winZone.getID());
                    buf.append(" has the exact same zone rule in year " + _referenceYear);
                    buf.append("\n");
                }
            }
        }

        if (buf.length() == 0) {
            _pw.println("[OK]");
        } else {
            _pw.println("[ERROR] Exact zone rule matches");
            _pw.println("{");
            _pw.println(buf.append("}"));
        }
        
    }


    public void testZoneOffsets() {
        _pw.println("### Checking if a pair of Windows time zone and Olson time zone use same zone offset through " + _referenceYear);

        boolean isOK = true;

        GregorianCalendar winCal = new GregorianCalendar();
        GregorianCalendar olsonCal = new GregorianCalendar();
        int[] winOffsets = new int[2];
        int[] olsonOffsets = new int[2];

        for (MapDataEntry entry : MapDataUtil.getAllEntriesInMapData()) {
            BasicTimeZone winTZ = WINTZREG.getTimeZone(entry.windowsID());
            BasicTimeZone olsonTZ = (BasicTimeZone)TimeZone.getTimeZone(entry.olsonID(), TimeZone.TIMEZONE_ICU);

            winCal.setTimeZone(winTZ);
            olsonCal.setTimeZone(olsonTZ);

            // Count number of days in the reference year using different zone offsets
            int diffOffsetDays = 0;

            // Make sure two zones are using the same offset in the reference year
            winCal.clear();
            winCal.set(_referenceYear, Calendar.JANUARY, 1, 12, 0, 0);
            olsonCal.clear();
            olsonCal.set(_referenceYear, Calendar.JANUARY, 1, 12, 0, 0);

            while (winCal.get(Calendar.YEAR) == _referenceYear) {
                winOffsets[0] = winCal.get(Calendar.ZONE_OFFSET);
                winOffsets[1] = winCal.get(Calendar.DST_OFFSET);

                olsonOffsets[0] = olsonCal.get(Calendar.ZONE_OFFSET);
                olsonOffsets[1] = olsonCal.get(Calendar.DST_OFFSET);

                // Ignore daylight saving amount, just compare UTC offset (base+dst)
                if (winOffsets[0] + winOffsets[1] != olsonOffsets[0] + olsonOffsets[1]) {
                    diffOffsetDays++;
                }

                winCal.add(Calendar.DATE, 1);
                olsonCal.add(Calendar.DATE, 1);
            }

            if (diffOffsetDays > _maxmumIncompatibleDaysAllowed /* default - 7 days */) {
                // Check if this mapping is defined based on Windows exemplar location.
                // For example, we want to use Pacific/Fiji for (UTC+12:00) Fiji ("Fiji Standard Time")
                // even DST rules are different.
                boolean winLocationMap = false;
                Set<String> winLocations = ExemplarLocationUtil.extractLocationsFromDisplayName(WINTZREG.getDisplayName(entry.windowsID()));
                for (String winLocation : winLocations) {
                    String olsonIdForLocation = ExemplarLocationUtil.getOlsonIdByLocation(winLocation);
                    if (entry.olsonID().equals(olsonIdForLocation)) {
                        winLocationMap = true;
                        break;
                    }
                }

                if (!winLocationMap) {
                    if (isOK) {
                        _pw.println("[WARNING] Incompatible zones");
                        _pw.println("{");
                        isOK = false;
                    }
                    _pw.println(entry.windowsID() + " <-> " + entry.olsonID() + " (incompatible days = " + diffOffsetDays + ")");
                }
            }
        }
        if (isOK) {
            _pw.println("[OK]");
        } else {
            _pw.println("}");
        }
    }

    public void testDefaultMap() {
        _pw.println("### Checking if each Windows zone has a default Olson zone mapping");

        SortedMap<String, Integer> defMap = new TreeMap<String, Integer>(); 
        for (MapDataEntry entry : MapDataUtil.getAllEntriesInMapData()) {
            Integer numDefault = defMap.get(entry.windowsID());
            if (numDefault == null) {
                if (entry.isDefault()) {
                    defMap.put(entry.windowsID(), 1);
                } else {
                    defMap.put(entry.windowsID(), 0);
                }
            } else {
                defMap.put(entry.windowsID(), numDefault + (entry.isDefault() ? 1 : 0));
            }
        }

        boolean isOK = true;
        for (Entry<String, Integer> mapEntry : defMap.entrySet()) {
            if (mapEntry.getValue() != 1) {
                if (isOK) {
                    _pw.println("[ERROR] Multiple or no default mappings");
                    _pw.println("{");
                    isOK = false;
                }
                _pw.println(mapEntry.getKey() + " - " + mapEntry.getValue() + " mappings");
            }
        }
        if (isOK) {
            _pw.println("[OK]");
        } else {
            _pw.println("}");
        }
    }

    public void testRegionalDefaultMap() {
        _pw.println("### Checking if each Windows zone has a default Olson zone mapping for each region");

        StringBuilder buf = new StringBuilder();

        Set<String> winIDs = MapDataUtil.getAvailableWindowsIDsInMapData();
        for (String winID : winIDs) {
            Set<String> regions = MapDataUtil.getAvailableRegionsInMapData(winID);
            // Each regional map must have exactly one default (default or regional default)
            for (String region : regions) {
                Collection<MapDataEntry> regionEntries = MapDataUtil.getMapDataEntries(winID, region);
                int defCount = 0;
                for (MapDataEntry entry : regionEntries) {
                    if (entry.isDefault() || entry.isRegionDefault()) {
                        defCount++;
                    }
                }
                if (defCount == 0) {
                    buf.append("[ERROR] No regional default for " + winID + ":" + region + "\n");
                } else if (defCount > 1) {
                    buf.append("[ERROR] Multiple regional default for " + winID + ":" + region + "\n");
                }
            }
        }

        if (buf.length() > 0) {
            _pw.println("{");
            _pw.println(buf);
            _pw.println("}");
        } else {
            _pw.println("[OK]");
        }
    }

    public void testUnmappableWindowsZones() {
        _pw.println("### Checking if unmappable Windows zones in MapData has no exact rule match in Olson zones");

        Set<String> tzids = TimeZone.getAvailableIDs(SystemTimeZoneType.CANONICAL, null, null);
        List<BasicTimeZone> olsonZones = new ArrayList<BasicTimeZone>(tzids.size());
        for (String tzid : tzids) {
            olsonZones.add((BasicTimeZone)TimeZone.getTimeZone(tzid, TimeZone.TIMEZONE_ICU));
        }

        boolean isOK = true;

        for (String winID : MapDataUtil.getUnmappableWindowsZones()) {
            BasicTimeZone winZone = WINTZREG.getTimeZone(winID);

            long start, end;
            GregorianCalendar cal = new GregorianCalendar(winZone);
            cal.clear();
            cal.set(_referenceYear, Calendar.JANUARY, 1, 0, 0, 0);
            start = cal.getTimeInMillis();
            cal.add(Calendar.YEAR, 1);
            end = cal.getTimeInMillis() - 1;

            for (BasicTimeZone olsonZone : olsonZones) {
                if (winZone.hasEquivalentTransitions(olsonZone, start, end)) {
                    if (isOK) {
                        _pw.println("[ERROR] Exact zone rule matches");
                        _pw.println("{");
                        isOK = false;
                    }
                    _pw.println(winID + " <-> " + olsonZone.getID());
                }
            }
        }

        if (isOK) {
            _pw.println("[OK]");
        } else {
            _pw.println("}");
        }
    }

    private static String ms2HourMin(int ms) {
        boolean negative = ms < 0;
        if (negative) {
            ms *= -1;
        }
        int tmp = ms / (1000 * 60);
        return String.format("%02d:%02d", negative ? -tmp/60 : tmp/60, tmp%60);
    }
}
