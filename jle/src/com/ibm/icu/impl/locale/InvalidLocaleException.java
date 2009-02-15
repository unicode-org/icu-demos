/*
 *******************************************************************************
 * Copyright (C) 2009, International Business Machines Corporation and         *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
package com.ibm.icu.impl.locale;

public class InvalidLocaleException extends Exception {

    private static final long serialVersionUID = -7670037453528817968L;

    public InvalidLocaleException() {
        super();
    }

    public InvalidLocaleException(String s) {
        super(s);
    }
}
