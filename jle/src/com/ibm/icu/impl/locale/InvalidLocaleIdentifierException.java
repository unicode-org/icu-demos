/*
 *******************************************************************************
 * Copyright (C) 2009, International Business Machines Corporation and         *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
package com.ibm.icu.impl.locale;

public class InvalidLocaleIdentifierException extends Exception {

    private static final long serialVersionUID = -1659935255469965702L;

    public InvalidLocaleIdentifierException() {
        super();
    }

    public InvalidLocaleIdentifierException(String s) {
        super(s);
    }
}
