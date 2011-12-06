/*
 *******************************************************************************
 * Copyright (C) 2011, International Business Machines Corporation and         *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
package com.ibm.icu.dev.tools.wintz.mapper;

import java.util.Set;
import java.util.TreeSet;

public class ExemplarLocationUtil {
    static final String UTC_PREFIX = "Coordinated Universal Time";

    /**
     * Extracts exemplar location(s) from the given Windows time zone display name in English.
     * For example, "Canberra", "Melbourne" and "Sydney" are returned for the input display name
     * "(UTC+10:00) Canberra, Melbourne, Sydney".
     * <p>
     * Note: Some Windows time zone display names do not contain any exemplar locations (such as
     * "(UTC-05:00) Eastern Time (US & Canada)"). For those names without exemplar locations,
     * this method returns an empty set. The list of non-exemplar locations are maintained in
     * {@link ExemplarLocationData} in the same package manually.
     * 
     * @param dispName Windows time zone display name
     * @return A set of exemplar locations
     */
    public static Set<String> extractLocationsFromDisplayName(String dispName) {
        Set<String> locNames = new TreeSet<String>();
        int substrIdx = dispName.indexOf(')');
        if (substrIdx > 0) {
            substrIdx++;
            String[] tokens = dispName.substring(substrIdx).trim().split(",");
            for (String s: tokens) {
                s = s.trim();
                if (!isNonExemplarLocations(s)) {
                    locNames.add(s);
                }
            }
        }
        return locNames;
    }

    private static boolean isNonExemplarLocations(String token) {
        if (token.startsWith(UTC_PREFIX)) {
            return true;
        }
        for (String excl : ExemplarLocationData.NON_LOCATIONS) {
            if (token.equals(excl)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Returns the Olson tzid for the given Windows exemplar location if available.
     * If there is no 1-to-1 mapping for the given location or the given location
     * is unknown, this method returns null.
     * 
     * @param location The exemplar location name extracted from Windows time zone English display name.
     * @return Olson tzid if available, or null.
     */
    public static String getOlsonIdByLocation(String location) {
        String olsonID = "";
        for (String[] locMapData : ExemplarLocationData.EXEMPLAR_LOCATIONS) {
            if (location.equals(locMapData[0])) {
                olsonID = locMapData[1];
                break;
            }
        }
        if (olsonID.length() == 0) {
            return null;
        }
        return olsonID;
    }

    public static Set<String> getExemplarLocations() {
        Set<String> locations = new TreeSet<String>();
        for (String[] locMapData : ExemplarLocationData.EXEMPLAR_LOCATIONS) {
            locations.add(locMapData[0]);
        }
        return locations;
    }
}
