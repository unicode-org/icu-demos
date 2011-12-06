/*
 *******************************************************************************
 * Copyright (C) 2009-2011, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
package com.ibm.icu.dev.tools.wintz;

import java.util.Map;
import java.util.TreeMap;

class RegDynamicDST {
    private String _tzid;
    private Map<Integer, TZI> _entries;
    private int _firstEntry;
    private int _lastEntry;

    public RegDynamicDST(String tzid) {
        _tzid = tzid;
    }

    public void set(String line) {
        String key = RegUtil.getKey(line);
        if (key.equals("FirstEntry")) {
            _firstEntry = RegUtil.getDwordValue(line);
        } else if (key.equals("LastEntry")) {
            _lastEntry = RegUtil.getDwordValue(line);
        } else {
            try {
                Integer year = Integer.valueOf(key);
                if (_entries == null) {
                    _entries = new TreeMap<Integer, TZI>();
                }
                TZI tzi = TZI.getInstance(RegUtil.getBinaryValue(line));
                _entries.put(year, tzi);
            } catch (NumberFormatException e) {
                throw new IllegalArgumentException("Unknown Dynamic DST key - " + line);
            }
        }
    }

    public String getTZID() {
        return _tzid;
    }

    public int getFirstYear() {
        return _firstEntry;
    }

    public int getLastYear() {
        return _lastEntry;
    }

    public TZI getTZI(int year) {
        return _entries.get(Integer.valueOf(year));
    }
}
