/*
 *******************************************************************************
 * Copyright (C) 2009, International Business Machines Corporation and         *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
package com.ibm.icu.util;

import com.ibm.icu.impl.locale.BaseLocale;
import com.ibm.icu.impl.locale.LanguageTag;
import com.ibm.icu.impl.locale.LocaleBuilder;
import com.ibm.icu.impl.locale.LocaleExtension;

public class Locale {
    private BaseLocale _base;
    private LocaleExtension _ext;

    private Locale(BaseLocale base, LocaleExtension ext) {
        _base = base;
        _ext = ext;
    }

    public Locale(String language, String region, String variant) {
        _base = BaseLocale.get(language, "", region, variant);
    }

    public boolean equals(Object obj) {
        return this == obj ||
                (obj instanceof Locale)
                && _base == ((Locale)obj)._base
                && _ext == ((Locale)obj)._ext;
    }

    public int hashCode() {
        return _base.hashCode() | _ext.hashCode();
    }

    public String toString() {
        return _base.getJava6String();
    }

    public String getLanguage() {
        return _base.getLanguage();
    }

    public String getScript() {
        return _base.getScript();
    }

    public String getCountry() {
        return _base.getRegion();
    }

    public String getVariant() {
        return _base.getVariant();
    }

    public static Locale fromLanguageTag(String tag) {
        BaseLocale base = null;
        LocaleExtension ext = null;

        try {
            LanguageTag t = LanguageTag.parse(tag);
            LocaleBuilder bld = new LocaleBuilder();
            bld.setLanguage(t.getLanguage()).setScript(t.getScript())
                .setRegion(t.getRegion()).setVariant(t.getVariant())
                .setPrivateUse(t.getPrivateUse());

            base = bld.getBaseLocale();
            ext = bld.getLocaleExtension();

        } catch (Exception e) {
            e.printStackTrace();
        }
        return new Locale(base, ext);
    }

    public String toLanguageTag() {
        StringBuilder buf = new StringBuilder();
        if (_base != null) {
            buf.append(_base.toString());
        }
        if (_ext != null) {
            if (buf.length() > 0) {
                buf.append("_");
            }
            buf.append(_ext.toString());
        }
        return buf.toString().replaceAll("_", "-");
    }
}
