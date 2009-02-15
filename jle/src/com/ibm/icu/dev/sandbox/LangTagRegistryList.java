/*
 *******************************************************************************
 * Copyright (C) 2009, International Business Machines Corporation and         *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
package com.ibm.icu.dev.sandbox;

import java.util.ListIterator;

import com.ibm.icu.dev.tools.languagetag.LanguageTagRegistry;
import com.ibm.icu.dev.tools.languagetag.LanguageTagRegistry.Record;

public class LangTagRegistryList {
    public static void main(String[] args) {
        allGrandfathered();
        //allVariants();
    }

    private static void allGrandfathered() {
        LanguageTagRegistry reg = LanguageTagRegistry.getInstance();
        ListIterator<Record> itr = reg.getIterator();

        while (itr.hasNext()) {
            Record rec = itr.next();
            if (rec.getType() == Record.GRANDFATHERED) {
                String tag = rec.getTag();
                String preferred = rec.getPreferredValue();
                String deprecated = rec.getDeprecated();
                StringBuilder buf = new StringBuilder();
                buf.append(tag);
                if (deprecated != null) {
                    buf.append("(deprecated)");
                    if (preferred != null) {
                        buf.append(" -> " + preferred);
                    }
                }
                System.out.println(buf);
            }
        }
    }

    private static void allVariants() {
        LanguageTagRegistry reg = LanguageTagRegistry.getInstance();
        ListIterator<Record> itr = reg.getIterator();

        while (itr.hasNext()) {
            Record rec = itr.next();
            if (rec.getType() == Record.VARIANT) {
                String var = rec.getSubtag();
                System.out.println(var);
            }
        }
    }
}
