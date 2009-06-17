/*
 *******************************************************************************
 * Copyright (C) 2009, International Business Machines Corporation and         *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
package com.ibm.icu.dev.tools.wintz;

public class RegTimeZoneInformation {
    private String _tzid;
    private String _display;
    private String _dlt;
    private String _std;
    private String _mapID;
    private int _index;
    private TZI _tzi;

    public RegTimeZoneInformation(String tzid) {
        _tzid = tzid;
    }

    public void set(String line) {
        String key = RegUtil.getKey(line);
        if (key.equals("Display")) {
            _display = RegUtil.getStringValue(line);
        } else if (key.equals("Dlt")) {
            _dlt = RegUtil.getStringValue(line);
        } else if (key.equals("Std")) {
            _std = RegUtil.getStringValue(line);
        } else if (key.equals("MapID")) {
            _mapID = RegUtil.getStringValue(line);
        } else if (key.equals("Index")) {
            _index = RegUtil.getDwordValue(line);
        } else if (key.equals("TZI")) {
            _tzi = TZI.getInstance(RegUtil.getBinaryValue(line));
        } else {
//            throw new IllegalArgumentException("Unknown Time Zone registry key type - " + line);
        }
    }

    public String getTZID() {
        return _tzid;
    }

    public String getDisplay() {
        return _display;
    }

    public String getDlt() {
        return _dlt;
    }

    public String getStd() {
        return _std;
    }

    public String getMapID() {
        return _mapID;
    }

    public int getIndex() {
        return _index;
    }

    public TZI getTZI() {
        return _tzi;
    }

}
