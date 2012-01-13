package com.ibm.icu.dev.tools.wintz.test;

import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.TreeSet;

import com.ibm.icu.dev.tools.wintz.TimeZoneRegistry;
import com.ibm.icu.dev.tools.wintz.mapper.ExemplarLocationUtil;
import com.ibm.icu.dev.tools.wintz.mapper.MapDataUtil;
import com.ibm.icu.dev.tools.wintz.mapper.MapDataUtil.MapDataEntry;
import com.ibm.icu.util.TimeZone;
import com.ibm.icu.util.TimeZone.SystemTimeZoneType;

public class MapDataCheck {

    public static void main(String... args) {
        MapDataCheck checker = new MapDataCheck();
        checker.testWindowsIdSet();
        checker.testZoneIdSet();
        checker.testWindowsExemplarLocations();
        checker.testDuplicate();
        checker.testCanonical();
        checker.testRegion();
        checker.testNoBaseOffsetMatches();
    }

    private PrintWriter pw;

    public MapDataCheck() {
        pw = new PrintWriter(System.out, true);
    }

    public void testWindowsIdSet() {
        pw.println("### Checking Available Windows IDs");
        TimeZoneRegistry reg = TimeZoneRegistry.get();
        Set<String> regWinIDs = reg.getAvailableTZIDs(true);
        Set<String> dataWinIDs = MapDataUtil.getAllKnownWindowsIDs();

        boolean isOK = true;

        if (!dataWinIDs.containsAll(regWinIDs)) {
            isOK = false;
            pw.println("# Missing Windows IDs in MapData");
            pw.println("{");
            for (String winID : regWinIDs) {
                if (!dataWinIDs.contains(winID)) {
                    pw.println(winID);
                }
            }
            pw.println("}");
        }
        if (!regWinIDs.containsAll(dataWinIDs)) {
            isOK = false;
            pw.println("# Windows IDs no longer available in the registry");
            pw.println("{");
            for (String winID : dataWinIDs) {
                if (!regWinIDs.contains(winID)) {
                    pw.println(winID);
                }
            }
            pw.println("}");
        }
        if (isOK) {
            pw.println("[OK]");
        }
    }

    public void testZoneIdSet() {
        pw.println("### Checking Available Zone IDs");

        Set<String> dataIDs = MapDataUtil.getAllKnownIDs();
        Set<String> canonicalIDs = TimeZone.getAvailableIDs(SystemTimeZoneType.CANONICAL, null, null);

        boolean isOK = true;

        if (!dataIDs.containsAll(canonicalIDs)) {
            isOK = false;
            pw.println("# Missing Zone IDs in MapData");
            pw.println("{");
            for (String id : canonicalIDs) {
                if (!dataIDs.contains(id)) {
                    pw.println(id);
                }
            }
            pw.println("}");
        }
        if (!canonicalIDs.containsAll(dataIDs)) {
            isOK = false;
            pw.println("# IDs not available in the tz database, or not canonical");
            pw.println("{");
            for (String dataID : dataIDs) {
                if (!canonicalIDs.contains(dataID)) {
                    pw.println(dataID);
                }
            }
            pw.println("}");
        }
        if (isOK) {
            pw.println("[OK]");
        }
    }

    public void testWindowsExemplarLocations() {
        pw.println("### Checking if ExemplarLocationData contains all Windows exemplar locations");
        TimeZoneRegistry reg = TimeZoneRegistry.get();
        Set<String> regWinIDs = reg.getAvailableTZIDs(true);

        Set<String> knownLocations = ExemplarLocationUtil.getExemplarLocations();
        Set<String> regLocations = new TreeSet<String>();

        for (String winID : regWinIDs) {
            Set<String> locations = ExemplarLocationUtil.extractLocationsFromDisplayName(reg.getDisplayName(winID));
            regLocations.addAll(locations);
        }

        boolean isOK = true;

        if (!knownLocations.containsAll(regLocations)) {
            isOK = false;
            pw.println("# Missing Windows exemplar locations in ExemplarLocationData");
            pw.println("{");
            for (String regLoc : regLocations) {
                if (!knownLocations.contains(regLoc)) {
                    pw.println(regLoc);
                }
            }
            pw.println("}");
        }
        if (!regLocations.containsAll(knownLocations)) {
            isOK = false;
            pw.println("# Windows exemplar locations no longer available in the registry");
            pw.println("{");
            for (String knownLoc : knownLocations) {
                if (!regLocations.contains(knownLoc)) {
                    pw.println(knownLoc);
                }
            }
            pw.println("}");
        }
        if (isOK) {
            pw.println("[OK]");
        }
    }

    public void testDuplicate() {
        pw.println("### Checking duplicated Olson ID mappings");

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
            pw.println("[OK]");
        } else {
            pw.println("# Duplicated Olson ID mappings");
            pw.println("{");
            for (String dup : dupIDs) {
                pw.println(dup);
            }
            pw.println("}");
        }

    }

    public void testCanonical() {
        pw.println("### Checking if Olson IDs in the mapping data are canonical");
        Set<String> olsonIDs = MapDataUtil.getAllKnownIDs();
        boolean isOK = true;
        for (String id : olsonIDs) {
            String canonical = TimeZone.getCanonicalID(id);
            if (!id.equals(canonical)) {
                if (isOK) {
                    pw.println("# Non-canonical Olson IDs");
                    pw.println("{");
                    isOK = false;
                }
                pw.println(id + "[" + canonical + "]");
            }
        }
        if (isOK) {
            pw.println("[OK]");
        } else {
            pw.println("}");
        }
    }

    public void testRegion() {
        pw.println("### Checking if Olson ID's region in the mapping data is correct");

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
                    pw.println("# Incorrect Olson time zone's region");
                    pw.println("{");
                    isOK = false;
                }
                pw.println(id + ": " + dataRegion + " -> " + region);
            }
        }

        if (isOK) {
            pw.println("[OK]");
        } else {
            pw.println("}");
        }
    }

    public void testNoBaseOffsetMatches() {
        pw.println("### Checking if the hardcoded list of zones without exact base offset match");
        TimeZoneRegistry reg = TimeZoneRegistry.get();
        Set<String> regWinIDs = reg.getAvailableTZIDs(true);
        List<TimeZone> winZones = new ArrayList<TimeZone>(regWinIDs.size());

        for (String winID : regWinIDs) {
            winZones.add(reg.getTimeZone(winID));
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
            pw.println("[OK]");
        } else {
            pw.println("# Base offset matches");
            pw.println("{");
            pw.println(buf.append("}"));
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
