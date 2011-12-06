/*
 *******************************************************************************
 * Copyright (C) 2009-2011, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
package com.ibm.icu.dev.tools.wintz;

class RegTimeZoneInformation {
    private String _tzid;
    private String _display;
    private String _dlt;
    private String _std;
    private TZI _tzi;
    private boolean _isObsolete;

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
        } else if (key.equals("IsObsolete")) {
            _isObsolete = (RegUtil.getDwordValue(line) == 1);
        } else if (key.equals("TZI")) {
            _tzi = TZI.getInstance(RegUtil.getBinaryValue(line));
        } else {
            assert false: "Unknown Time Zone registry key type - " + line;
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

    public boolean isObsolete() {
        return _isObsolete;
    }

    public TZI getTZI() {
        return _tzi;
    }

}
