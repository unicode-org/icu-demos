package com.ibm.icu.dev.tools.wintz.mapper;

import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;
import java.util.TreeSet;
import java.util.Map.Entry;

public class MapDataUtil {
    /*
     * Windows ID - region - zone list map
     */
    private static final Map<String, Map<String, List<String>>> MAP_DATA;

    static {
        MAP_DATA = new TreeMap<String, Map<String, List<String>>>();
        for (String[] entry : MapData.MAP_DATA_ARRAY) {
            String winID = entry[0];
            String region = entry[1];
            String olsonID = entry[2];

            Map<String, List<String>> regionMap = MAP_DATA.get(winID);
            if (regionMap == null) {
                regionMap = new TreeMap<String, List<String>>();
                MAP_DATA.put(winID, regionMap);
            }
            List<String> zoneList = regionMap.get(region);
            if (zoneList == null) {
                zoneList = new LinkedList<String>();
                regionMap.put(region, zoneList);
            }
            zoneList.add(olsonID);
        }
    }

    /*
     * Windows ID - region - zone list map / supplemental mapping data including zones only matches base offset.
     */
    private static final Map<String, Map<String, List<String>>> SUPPLEMENTAL_MAP_DATA;

    static {
        SUPPLEMENTAL_MAP_DATA = new TreeMap<String, Map<String, List<String>>>();
        for (String[] entry : MapData.SUPPLEMENTAL_MAP_DATA_ARRAY) {
            String winID = entry[0];
            String region = entry[1];
            String olsonID = entry[2];

            Map<String, List<String>> regionMap = SUPPLEMENTAL_MAP_DATA.get(winID);
            if (regionMap == null) {
                regionMap = new TreeMap<String, List<String>>();
                SUPPLEMENTAL_MAP_DATA.put(winID, regionMap);
            }
            List<String> zoneList = regionMap.get(region);
            if (zoneList == null) {
                zoneList = new LinkedList<String>();
                regionMap.put(region, zoneList);
            }
            zoneList.add(olsonID);
        }
    }

    private static final Set<String> NON_CLDR_ZONES;

    static {
        NON_CLDR_ZONES = new TreeSet<String>();
        for (String zone : MapData.NON_CLDR_ZONES_ARRAY) {
            NON_CLDR_ZONES.add(zone);
        }
    }


    private static final Set<String> NO_BASE_OFFSET_MATCH_ZONES;

    static {
        NO_BASE_OFFSET_MATCH_ZONES = new TreeSet<String>();
        for (String zone : MapData.NO_BASE_OFFSET_MATCH_ZONES_ARRAY) {
            NO_BASE_OFFSET_MATCH_ZONES.add(zone);
        }
    }

    public static Set<String> getAvailableWindowsIDs(boolean includesSupplemental) {
        Set<String> availWinIDs = new TreeSet<String>(MAP_DATA.keySet());
        if (includesSupplemental) {
            availWinIDs.addAll(SUPPLEMENTAL_MAP_DATA.keySet());
        }
        return availWinIDs;
    }

    public static Map<String, List<String>> getMapData(String winID, boolean includesSupplemental) {
        Map<String, List<String>> regionMap = new TreeMap<String, List<String>>();

        // Read MAP_DATA
        Map<String, List<String>> regionMap1 = MAP_DATA.get(winID);
        if (regionMap1 != null) {
            for (Entry<String, List<String>> entry : regionMap1.entrySet()) {
                regionMap.put(entry.getKey(), new LinkedList<String>(entry.getValue()));
            }
        }

        if (includesSupplemental) {
            Map<String, List<String>> regionMap2 = SUPPLEMENTAL_MAP_DATA.get(winID);
            if (regionMap2 != null) {
                for (Entry<String, List<String>> entry : regionMap2.entrySet()) {
                    String region = entry.getKey();
                    List<String> zoneList = entry.getValue();

                    List<String> resultZoneList = regionMap.get(region);
                    if (resultZoneList == null) {
                        regionMap.put(region, new LinkedList<String>(zoneList));
                    } else {
                        resultZoneList.addAll(zoneList);
                    }
                }
            }
        }

        return regionMap;
    }

    public static Set<String> getAvailableOlsonIDsInMapData(boolean includesSupplemental) {
        Set<String> olsonIDs = new TreeSet<String>();
        for (String[] data : MapData.MAP_DATA_ARRAY) {
            olsonIDs.add(data[2]);
        }

        if (includesSupplemental) {
            for (String[] data : MapData.SUPPLEMENTAL_MAP_DATA_ARRAY) {
                olsonIDs.add(data[2]);
            }
        }
        return olsonIDs;
    }

    public static Set<String> getNonCLDRZones() {
        return Collections.unmodifiableSet(NON_CLDR_ZONES);
    }

    public static Set<String> getNoBaseOffestMatchZones() {
        return Collections.unmodifiableSet(NO_BASE_OFFSET_MATCH_ZONES);
    }

}
