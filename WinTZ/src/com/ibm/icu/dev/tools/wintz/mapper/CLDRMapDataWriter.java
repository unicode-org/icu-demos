package com.ibm.icu.dev.tools.wintz.mapper;

import java.io.PrintWriter;
import java.util.Collection;
import java.util.Map.Entry;
import java.util.SortedMap;
import java.util.SortedSet;
import java.util.TreeMap;
import java.util.TreeSet;

import com.ibm.icu.dev.tools.wintz.TimeZoneRegistry;
import com.ibm.icu.dev.tools.wintz.mapper.MapDataUtil.MapDataEntry;
import com.ibm.icu.util.TimeZone;

public class CLDRMapDataWriter {
    public void write(PrintWriter pw, int baseIndent) {
        int indent = baseIndent;

        println(pw, "<windowsZones>", indent);
        indent++;
        println(pw, "<mapTimezones typeVersion=\"" + MapDataUtil.getTZDataVersion() 
                + "\" otherVersion=\"" + MapDataUtil.getWINTZDataVersion() + "\">", indent);
        indent++;

        // Collect map data
        SortedMap<WindowsZoneMapKey, WindowsZoneMapEntries> map = new TreeMap<WindowsZoneMapKey, WindowsZoneMapEntries>();
        Collection<MapDataEntry> all = MapDataUtil.getAllEntriesInMapData();
        for (MapDataEntry dataEntry : all) {
            WindowsZoneMapKey key = new WindowsZoneMapKey(dataEntry.windowsID());
            WindowsZoneMapEntries value = map.get(key);
            if (value == null) {
                value = new WindowsZoneMapEntries();
                map.put(key,  value);
            }
            String region = dataEntry.region();
            String olsonID = dataEntry.olsonID();
            if (dataEntry.isDefault()) {
                value.addDefault(region, olsonID);
            } else if (dataEntry.isRegionDefault()) {
                value.addRegionalDefault(region, olsonID);
            } else {
                value.add(region, olsonID);
            }
        }

        // Add Unmappable zones (so we can put a comment line)
        for (String unmapWinID : MapDataUtil.getUnmappableWindowsZones()) {
            WindowsZoneMapKey key = new WindowsZoneMapKey(unmapWinID);
            WindowsZoneMapEntries value = map.get(key);
            if (value != null) {
                // Unmappable Windows zone should not be there
                throw new RuntimeException("Map data for " + unmapWinID + " exists");
            }
            // Create empty map entry
            map.put(key, new WindowsZoneMapEntries());
        }

        // Emit map data
        for (Entry<WindowsZoneMapKey, WindowsZoneMapEntries> entry : map.entrySet()) {
            WindowsZoneMapKey key = entry.getKey();
            WindowsZoneMapEntries value = entry.getValue();

            pw.println();
            value.printMapZoneElements(pw, key.getWindowsID(), key.getDisplayName(), indent);
        }

        indent--;
        println(pw, "</mapTimezones>", indent);
        indent--;
        println(pw, "</windowsZones>", indent);
    }

    private static void println(PrintWriter pw, String text, int indent) {
        for (int i = 0; i < indent; i++) {
            pw.print('\u0009');
        }
        pw.println(text);
    }

    private static class WindowsZoneMapKey implements Comparable<WindowsZoneMapKey> {
        private String _winID;
        private String _dispName;
        private int _baseOffset;

        private static TimeZoneRegistry WINTZREG = TimeZoneRegistry.get();

        WindowsZoneMapKey(String winID) {
            _winID = winID;
            _dispName = WINTZREG.getDisplayName(winID);
            TimeZone winTZ = WINTZREG.getTimeZone(winID);
            _baseOffset = winTZ.getRawOffset();
        }

        String getWindowsID() {
            return _winID;
        }

        String getDisplayName() {
            return _dispName;
        }

        @Override
        public int compareTo(WindowsZoneMapKey o) {
            if (_baseOffset == o._baseOffset) {
                return _dispName.compareTo(o._dispName);  
            }
            return _baseOffset - o._baseOffset > 0 ? 1 : -1;
        }

        @Override
        public boolean equals(Object obj) {
            if (obj == null) {
                return false;
            }
            if (!(obj instanceof WindowsZoneMapKey)) {
                return false;
            }
            return _winID.equals(((WindowsZoneMapKey)obj)._winID);
        }

        @Override
        public int hashCode() {
            return _winID.hashCode();
        }
    }

    private static class WindowsZoneMapEntries {
        private String _defOlsonID;
        private SortedMap<String, RegionalEntries> _regionalMap = new TreeMap<String, RegionalEntries>();

        void addDefault(String region, String olsonID) {
            if (_defOlsonID != null) {
                throw new RuntimeException("Default zone is already set");
            }
            _defOlsonID = olsonID;
            addRegionalDefault(region, olsonID);
        }

        void addRegionalDefault(String region, String olsonID) {
            RegionalEntries regEntries = _regionalMap.get(region);
            if (regEntries == null) {
                regEntries = new RegionalEntries();
                _regionalMap.put(region, regEntries);
            }
            regEntries.addDefault(olsonID);
        }

        void add(String region, String olsonID) {
            RegionalEntries regEntries = _regionalMap.get(region);
            if (regEntries == null) {
                regEntries = new RegionalEntries();
                _regionalMap.put(region, regEntries);
            }
            regEntries.add(olsonID);
        }

        void printMapZoneElements(PrintWriter pw, String winID, String winDispName, int indent) {
            // First, print comment line
            println(pw, "<!-- " + winDispName + " -->", indent);

            if (_regionalMap.size() == 0) {
                // No mapping data available
                println(pw, "<!-- Unmappable -->", indent);
            } else {
                if (_defOlsonID == null) {
                    throw new RuntimeException("Default zone is not set");
                }

                // Default mapping
                println(pw, "<mapZone other=\"" + winID + "\" territory=\"001\" type=\"" + _defOlsonID + "\"/>", indent);

                // Regional mappings
                for (Entry<String, RegionalEntries> regEntry : _regionalMap.entrySet()) {
                    println(pw, "<mapZone other=\"" + winID + "\" territory=\"" + regEntry.getKey()
                            + "\" type=\"" + regEntry.getValue().getZoneList() + "\"/>", indent);
                }
            }
        }

        private static class RegionalEntries {
            private String _regDefOlsonID;
            private SortedSet<String> _olsonIDs = new TreeSet<String>();

            void addDefault(String olsonID) {
                if (_regDefOlsonID != null) {
                    throw new RuntimeException("Regional default zone is already set");
                }
                _regDefOlsonID = olsonID;
            }

            void add(String olsonID) {
                _olsonIDs.add(olsonID);
            }

            String getZoneList() {
                if (_regDefOlsonID == null) {
                    throw new RuntimeException("Regional default zone is not set");
                }
                StringBuilder buf = new StringBuilder(_regDefOlsonID);
                for (String id : _olsonIDs) {
                    buf.append(" ");
                    buf.append(id);
                }
                return buf.toString();
            }
        }
    }

    public static void main(String... args) {
        CLDRMapDataWriter mapDataWriter = new CLDRMapDataWriter();
        mapDataWriter.write(new PrintWriter(System.out, true), 1);
    }
}
