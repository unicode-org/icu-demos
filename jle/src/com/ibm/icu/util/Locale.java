/*
 *******************************************************************************
 * Copyright (C) 2009, International Business Machines Corporation and         *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
package com.ibm.icu.util;

import com.ibm.icu.impl.locale.BaseLocale;
import com.ibm.icu.impl.locale.InternalLocaleBuilder;
import com.ibm.icu.impl.locale.InvalidLocaleException;
import com.ibm.icu.impl.locale.LanguageTag;
import com.ibm.icu.impl.locale.LocaleExtension;

public class Locale {
    private BaseLocale _base;
    private LocaleExtension _ext = LocaleExtension.EMPTY_EXTENSION;

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
        LocaleBuilder bld = new LocaleBuilder();
        try {
            LanguageTag t = LanguageTag.parse(tag);
            bld.setLanguage(t.getLanguage()).setScript(t.getScript())
                .setRegion(t.getRegion()).setVariant(t.getVariant())
                .setPrivateUse(t.getPrivateUse());
            //TODO extensions
        } catch (Exception e) {
            e.printStackTrace();
        }
        return bld.get();
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

    public static class LocaleBuilder {
        private InternalLocaleBuilder _locbld = new InternalLocaleBuilder();

        /**
         * Constructs an empty LocaleBuilder.
         */
        public LocaleBuilder() {
        }

        /**
         *
         * @param loc
         * @return
         * @throws InvalidLocaleException
         */
        public LocaleBuilder setLocale(Locale loc) throws InvalidLocaleException {
            //TODO
            return this;
        }

        /**
         * Sets the language to this builder.  If the specified language is empty or null,
         * this method clears the language value previously set.
         * 
         * @param language the language
         * @return this builder
         * @throws InvalidLocaleException
         */
        public LocaleBuilder setLanguage(String language) throws InvalidLocaleException {
            String newval = _locbld.setLanguage(language);
            if (newval == null) {
                throw new InvalidLocaleException("Invalid language: " + language);
            }
            return this;
        }

        /**
         * Sets the language to this builder.  If the specified script is empty or null,
         * this method clears the script value previously set.
         * 
         * @param script the script
         * @return this builder
         * @throws InvalidLocaleException
         */
        public LocaleBuilder setScript(String script) throws InvalidLocaleException {
            String newval = _locbld.setScript(script);
            if (newval == null) {
                throw new InvalidLocaleException("Invalid script: " + script);
            }
            return this;
        }

        /**
         * Sets the region to this builder.  If the specified region is empty or null,
         * this method clears the region value previously set.
         * 
         * @param region the region
         * @return this builder
         * @throws InvalidLocaleException
         */
        public LocaleBuilder setRegion(String region) throws InvalidLocaleException {
            String newval = _locbld.setRegion(region);
            if (newval == null) {
                throw new InvalidLocaleException("Invalid region: " + region);
            }
            return this;
        }

        /**
         * Sets the variant to this builder.  If the specified variant is empty or null,
         * this method clears the variant value previously set.
         * 
         * @param variant the variant
         * @return this builder
         * @throws InvalidLocaleException
         */
        public LocaleBuilder setVariant(String variant) throws InvalidLocaleException {
            String newval = _locbld.setVariant(variant);
            if (newval == null) {
                throw new InvalidLocaleException("Invalid variant: " + variant);
            }
            return this;
        }

        /**
         * Sets the locale keyword to this builder.  If the specified type is empty or null,
         * this method clears the type previously set for the key.
         * 
         * @param key the locale keyword key
         * @param type the locake keyword type
         * @return this builder
         * @throws InvalidLocaleException
         */
        public LocaleBuilder setLocaleKeyword(String key, String type) throws InvalidLocaleException {
            boolean set = _locbld.setLocaleKeyword(key, type);
            if (!set) {
                throw new InvalidLocaleException("Invalid locale keyword key/type pairs: key=" + key + "/type=" + type);
            }
            return this;
        }

        /**
         * Sets the locale extension to this builder.  If the specified value is empty or null,
         * this method clears the value previously set for the key.
         * 
         * @param key the extension character key
         * @param value the extension value
         * @return this builder
         * @throws InvalidLocaleException
         */
        public LocaleBuilder setExtension(char key, String value) throws InvalidLocaleException {
            boolean set = _locbld.setExtension(key, value);
            if (!set) {
                throw new InvalidLocaleException("Invalid extension key/value pairs: key=" + key + "/value=" + value);
            }
            return this;
        }

        /**
         * Sets the private use value to this builder.  If the specified private use value
         * is empty or null, this method clears the private use value previously set.
         * 
         * @param privuse the private use value
         * @return this builder
         * @throws InvalidLocaleException
         */
        public LocaleBuilder setPrivateUse(String privuse) throws InvalidLocaleException {
            String newval = _locbld.setPrivateUse(privuse);
            if (newval == null) {
                throw new InvalidLocaleException("Invalid private use value: " + privuse);
            }
            return this;
        }

        /**
         * Returns an instance of locale created from locale fields configured by
         * the setters in this locale builder instance.
         * 
         * @return a locale
         */
        public Locale get() {
            BaseLocale base = _locbld.getBaseLocale();
            LocaleExtension extension = _locbld.getLocaleExtension();

            Locale newloc = new Locale(base, extension);
            return newloc;
        }
    }

}
