/*
 *******************************************************************************
 * Copyright (C) 2011-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
package com.ibm.icu.dev.tools.wintz.mapper;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Set;
import java.util.SortedMap;
import java.util.SortedSet;
import java.util.TreeMap;
import java.util.TreeSet;

public class MapDataUtil {

    public static class MapDataEntry {
        private String _winID;
        private String _region;
        private String _olsonID;
        private boolean _isDefault;
        private boolean _isRegDefault;

        MapDataEntry(String winID, String region, String olsonID, String defStatus) {
            _winID = winID;
            _region = region;
            _olsonID = olsonID;
            _isDefault = MapData.DEFAULT.equals(defStatus);
            _isRegDefault = _isDefault || MapData.REGION_DEFAULT.equals(defStatus);
        }

        MapDataEntry(String winID, String region, String olsonID) {
            this(winID, region, olsonID, null);
        }

        public String windowsID() {
            return _winID;
        }

        public String region() {
            return _region;
        }

        public String olsonID() {
            return _olsonID;
        }

        public boolean isDefault() {
            return _isDefault;
        }

        public boolean isRegionDefault() {
            return _isRegDefault;
        }
    }

    private static final Collection<MapDataEntry> MAP_DATA_ENTRIES;
    static {
        Collection<MapDataEntry> mapDataEntries = new ArrayList<MapDataEntry>(MapData.MAP_DATA_ARRAY.length);
        for (String[] data : MapData.MAP_DATA_ARRAY) {
            MapDataEntry entry;
            if (data.length > 3) {
                entry = new MapDataEntry(data[0], data[1], data[2], data[3]);
            } else {
                entry = new MapDataEntry(data[0], data[1], data[2]);
            }
            mapDataEntries.add(entry);
        }
        MAP_DATA_ENTRIES = Collections.unmodifiableCollection(mapDataEntries);
    }

    private static final SortedMap<String, Collection<MapDataEntry>> MAP;
    static {
        SortedMap<String, Collection<MapDataEntry>> map = new TreeMap<String, Collection<MapDataEntry>>();
        for (MapDataEntry entry : MAP_DATA_ENTRIES) {
            Collection<MapDataEntry> regionEntries = map.get(entry.windowsID());
            if (regionEntries == null) {
                regionEntries = new ArrayList<MapDataEntry>();
                map.put(entry.windowsID(), regionEntries);
            }
            regionEntries.add(entry);
        }
        MAP = Collections.unmodifiableSortedMap(map);
    }

    private static final SortedSet<String> UNMAPPABLE_WINDOWS_ZONES;
    static {
        SortedSet<String> unmappableWindowsZones = new TreeSet<String>();
        for (String winZone : MapData.UNMAPPALBE_WINDOWS_ZONES_ARRAY) {
            unmappableWindowsZones.add(winZone);
        }
        UNMAPPABLE_WINDOWS_ZONES = Collections.unmodifiableSortedSet(unmappableWindowsZones);
    }

    private static final SortedSet<String> NO_BASE_OFFSET_MATCH_ZONES;
    static {
        SortedSet<String> noBaseOffsetMatchZones = new TreeSet<String>();
        for (String zone : MapData.NO_BASE_OFFSET_MATCH_ZONES_ARRAY) {
            noBaseOffsetMatchZones.add(zone);
        }
        NO_BASE_OFFSET_MATCH_ZONES = Collections.unmodifiableSortedSet(noBaseOffsetMatchZones);
    }

    private static final SortedSet<String> NO_DST_RULE_MATCH_ZONES;
    static {
        SortedSet<String> noDstRuleMatchZones = new TreeSet<String>();
        for (String zone : MapData.NO_DST_RULE_MATCH_ZONES_ARRAY) {
            noDstRuleMatchZones.add(zone);
        }
        NO_DST_RULE_MATCH_ZONES = Collections.unmodifiableSortedSet(noDstRuleMatchZones);
    }

    private static final SortedSet<String> NON_CLDR_ZONES;
    static {
        SortedSet<String> nonCldrZones = new TreeSet<String>();
        for (String zone : MapData.NON_CLDR_ZONES_ARRAY) {
            nonCldrZones.add(zone);
        }
        NON_CLDR_ZONES = Collections.unmodifiableSortedSet(nonCldrZones);
    }



    /**
     * Returns a collection of all MapDateEntry in the map data
     * @return A collection of all MapDateEntry
     */
    public static Collection<MapDataEntry> getAllEntriesInMapData() {
        return MAP_DATA_ENTRIES;
    }


    /**
     * Returns a set of Windows IDs sorted ascending order in the map data.
     * @return A NavigableSet of Windows IDs
     */
    public static Set<String> getAvailableWindowsIDsInMapData() {
        return MAP.keySet();
    }

    /**
     * Returns a set of region codes sorted ascending order in the map data
     * @param winID Windows time zone ID
     * @return A NavigableSet of region codes or null if the specified Windows time zone ID is not available
     */
    public static Set<String> getAvailableRegionsInMapData(String winID) {
        Collection<MapDataEntry> entries = MAP.get(winID);
        if (entries == null) {
            return null;
        }
        Set<String> regions = new TreeSet<String>();
        for (MapDataEntry entry : entries) {
            regions.add(entry.region());
        }
        return regions;
    }

    /**
     * Returns a collection of MapDataEntry for the given Windows time zone ID and the region code.
     * @param winID Windows time zone ID
     * @param region Region code
     * @return A collection of MapDataEntry or null
     */
    public static Collection<MapDataEntry> getMapDataEntries(String winID, String region) {
        Collection<MapDataEntry> entries = MAP.get(winID);
        if (entries == null) {
            return null;
        }

        region = region.toUpperCase();
        Collection<MapDataEntry> results = null;
        for (MapDataEntry entry : entries) {
            if (entry.region().equals(region)) {
                if (results == null) {
                    results = new ArrayList<MapDataEntry>();
                }
                results.add(entry);
            }
        }
        return results;
    }

    public static Set<String> getUnmappableWindowsZones() {
        return UNMAPPABLE_WINDOWS_ZONES;
    }

    public static Set<String> getNoBaseOffestMatchZones() {
        return NO_BASE_OFFSET_MATCH_ZONES;
    }

    public static Set<String> getNoDstRuleMatchZones() {
        return NO_DST_RULE_MATCH_ZONES;
    }

    public static Set<String> getNonCLDRZones() {
        return NON_CLDR_ZONES;
    }

    public static SortedSet<String> getAllKnownWindowsIDs() {
        // All Windows IDs in the map data
        SortedSet<String> result = new TreeSet<String>(getAvailableWindowsIDsInMapData());

        // All unmappable Windows IDs
        for (String winID : getUnmappableWindowsZones()) {
            result.add(winID);
        }

        return result;
    }

    public static SortedSet<String> getAllKnownIDs() {
        SortedSet<String> result = new TreeSet<String>();

        // All Olson IDs in MapData.MAP_DATA_ARRAY;
        for (MapDataEntry entry : getAllEntriesInMapData()) {
            result.add(entry.olsonID());
        }

        // All Olson IDs in MapData.NO_BASE_OFFSET_MATCH_ZONES_ARRAY
        result.addAll(getNoBaseOffestMatchZones());

        // All Olson IDs in MapData.NO_DST_RULE_MATCH_ZONES_ARRAY
        result.addAll(getNoDstRuleMatchZones());

        // All Zone IDs in MapData.NON_CLDR_ZONES_ARRAY (ICU specific zones)
        result.addAll(getNonCLDRZones());

        return result;
    }

    public static String getTZDataVersion() {
        return MapData.TZDATA_VERSION;
    }

    public static String getWINTZDataVersion() {
        return MapData.WINTZDATA_VERSION;
    }
}
