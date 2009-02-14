package com.ibm.icu.util;

import com.ibm.icu.impl.locale.BaseLocale;
import com.ibm.icu.impl.locale.LocaleExtension;

public class Locale {
    private BaseLocale _base;
    private LocaleExtension _ext;

    public Locale(String language, String region, String variant) {
        _base = BaseLocale.getInstance(language, "", region, variant);
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
        return _base.getJava6ID();
    }
}
