package com.ibm.icu.dev.tools.wintz.test;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;
import java.util.TreeSet;

import com.ibm.icu.dev.tools.wintz.TimeZoneRegistry;
import com.ibm.icu.dev.tools.wintz.mapper.ExemplarLocationUtil;
import com.ibm.icu.dev.tools.wintz.mapper.MapDataUtil;
import com.ibm.icu.util.TimeZone;
import com.ibm.icu.util.TimeZone.SystemTimeZoneType;

public class MapDataCheck {
    public static void main(String... args) {
        checkWindowsIdSet();
        checkWindowsExemplarLocations();
        checkDuplicate();
        checkCanonical();
        checkRegion();
        checkNoBaseOffsetMatches();
        printUnmappedOlsonIDs();
    }

    static void checkWindowsIdSet() {
        System.out.println("### Checking Available Windows IDs");
        TimeZoneRegistry reg = TimeZoneRegistry.get();
        Set<String> regWinIDs = reg.getAvailableTZIDs(true);
        Set<String> dataWinIDs = MapDataUtil.getAvailableWindowsIDs(true);

        boolean isOK = true;

        if (!dataWinIDs.containsAll(regWinIDs)) {
            isOK = false;
            System.out.println("# Missing Windows IDs in MapData");
            System.out.println("{");
            for (String winID : regWinIDs) {
                if (!dataWinIDs.contains(winID)) {
                    System.out.println(winID);
                }
            }
            System.out.println("}");
        }
        if (!regWinIDs.containsAll(dataWinIDs)) {
            isOK = false;
            System.out.println("# Windows IDs no longer available in the registry");
            System.out.println("{");
            for (String winID : dataWinIDs) {
                if (!regWinIDs.contains(winID)) {
                    System.out.println(winID);
                }
            }
            System.out.println("}");
        }
        if (isOK) {
            System.out.println("[OK]");
        }
    }

    static void checkWindowsExemplarLocations() {
        System.out.println("### Checking if ExemplarLocationData contains all Windows exemplar locations");
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
            System.out.println("# Missing Windows exemplar locations in ExemplarLocationData");
            System.out.println("{");
            for (String regLoc : regLocations) {
                if (!knownLocations.contains(regLoc)) {
                    System.out.println(regLoc);
                }
            }
            System.out.println("}");
        }
        if (!regLocations.containsAll(knownLocations)) {
            isOK = false;
            System.out.println("# Windows exemplar locations no longer available in the registry");
            System.out.println("{");
            for (String knownLoc : knownLocations) {
                if (!regLocations.contains(knownLoc)) {
                    System.out.println(knownLoc);
                }
            }
            System.out.println("}");
        }
        if (isOK) {
            System.out.println("[OK]");
        }
    }

    static void checkDuplicate() {
        System.out.println("### Checking duplicated Olson ID mappings");

        Set<String> olsonIDs = new HashSet<String>();
        Set<String> dupIDs = new TreeSet<String>();
        Set<String> dataWinIDs = MapDataUtil.getAvailableWindowsIDs(true);

        for (String winID : dataWinIDs) {
            Map<String, List<String>> regionMap = MapDataUtil.getMapData(winID, true);
            for (Entry<String, List<String>> data : regionMap.entrySet()) {
                List<String> zoneList = data.getValue();
                for (String olsonID : zoneList) {
                    if (olsonIDs.contains(olsonID)) {
                        dupIDs.add(olsonID);
                    } else {
                        olsonIDs.add(olsonID);
                    }
                }
            }
        }

        if (dupIDs.isEmpty()) {
            System.out.println("[OK]");
        } else {
            System.out.println("# Duplicated Olson ID mappings");
            System.out.println("{");
            for (String dup : dupIDs) {
                System.out.println(dup);
            }
            System.out.println("}");
        }

    }

    static void checkCanonical() {
        System.out.println("### Checking if Olson IDs in the mapping data are canonical");
        Set<String> olsonIDs = MapDataUtil.getAvailableOlsonIDsInMapData(true);
        boolean isOK = true;
        for (String id : olsonIDs) {
            String canonical = TimeZone.getCanonicalID(id);
            if (!id.equals(canonical)) {
                if (isOK) {
                    System.out.println("# Non-canonical Olson IDs");
                    System.out.println("{");
                    isOK = false;
                }
                System.out.println(id + "[" + canonical + "]");
            }
        }
        if (isOK) {
            System.out.println("[OK]");
        } else {
            System.out.println("}");
        }
    }

    static void checkRegion() {
        System.out.println("### Checking if Olson ID's region in the mapping data is correct");

        boolean isOK = true;
        Set<String> dataWinIDs = MapDataUtil.getAvailableWindowsIDs(true);
        for (String winID : dataWinIDs) {
            Map<String, List<String>> regionMap = MapDataUtil.getMapData(winID, true);
            for (Entry<String, List<String>> data : regionMap.entrySet()) {
                String dataRegion = data.getKey();
                List<String> zoneList = data.getValue();
                for (String olsonID : zoneList) {
                    String region = TimeZone.getRegion(olsonID);
                    if (!dataRegion.equals(region)) {
                        if (isOK) {
                            System.out.println("# Incorrect Olson time zone's region");
                            System.out.println("{");
                            isOK = false;
                        }
                        System.out.println(olsonID + ": " + dataRegion + " -> " + region);
                    }
                }
            }
        }
        if (isOK) {
            System.out.println("[OK]");
        } else {
            System.out.println("}");
        }
    }

    static void checkNoBaseOffsetMatches() {
        System.out.println("### Checking if the hardcoded list of zones without exact base offset match");
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
            System.out.println("[OK]");
        } else {
            System.out.println("# Base offset matches");
            System.out.println("{");
            System.out.println(buf.append("}"));
        }
    }

    static void printUnmappedOlsonIDs() {
        System.out.println("### Printing Olson IDs not included in the mapping data");

        Set<String> availableIDs = MapDataUtil.getAvailableOlsonIDsInMapData(true);
        Set<String> canonicalIDs = TimeZone.getAvailableIDs(SystemTimeZoneType.CANONICAL, null, null);
        Set<String> nonCldrIDs = MapDataUtil.getNonCLDRZones();
        Set<String> noBaseOffsetMatchIDs = MapDataUtil.getNoBaseOffestMatchZones();

        System.out.println("{");
        for (String id : canonicalIDs) {
            if (nonCldrIDs.contains(id)) {
                continue;
            }
            if (!availableIDs.contains(id)) {
                if (noBaseOffsetMatchIDs.contains(id)) {
                    System.out.println(id + " [" + TimeZone.getRegion(id) + "]");
                }
            }
        }
        System.out.println("}");
    }


    static String ms2HourMin(int ms) {
        boolean negative = ms < 0;
        if (negative) {
            ms *= -1;
        }
        int tmp = ms / (1000 * 60);
        return String.format("%02d:%02d", negative ? -tmp/60 : tmp/60, tmp%60);
    }
}
