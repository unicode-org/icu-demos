/*
 *******************************************************************************
 * Copyright (C) 2009, International Business Machines Corporation and         *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
package com.ibm.icu.util;

import java.util.Iterator;
import java.util.Set;
import java.util.Locale.LocaleBuilder;

import com.ibm.icu.impl.locale.BaseLocale;
import com.ibm.icu.impl.locale.InternalLocaleBuilder;
import com.ibm.icu.impl.locale.InvalidLocaleIdentifierException;
import com.ibm.icu.impl.locale.LanguageTag;
import com.ibm.icu.impl.locale.LocaleExtensions;
import com.ibm.icu.impl.locale.LocaleObjectCache;
import com.ibm.icu.impl.locale.BaseLocale.BaseLocaleKey;
import com.ibm.icu.impl.locale.LanguageTag.Extension;

public class Locale {
    private final static LocaleObjectCache<LocaleKey, Locale> LOCALECACHE =
        new LocaleObjectCache<LocaleKey, Locale>();

    private static Locale ROOT = getInstance("", "", "", "", LocaleExtensions.EMPTY_EXTENSIONS);

    private BaseLocale _baseLocale;
    private LocaleExtensions _extensions = LocaleExtensions.EMPTY_EXTENSIONS;


    private Locale(String language, String script, String country, String variant, LocaleExtensions extensions) {
        _baseLocale = BaseLocale.getInstance(language, script, country, variant);
        _extensions = extensions;
    }

    public Locale(String language, String region, String variant) {
        _baseLocale = BaseLocale.getInstance(language, "", region, variant);
    }

    private static Locale getInstance(String language, String script, String country, String variant, LocaleExtensions extensions) {
        if (language== null || script == null || country == null || variant == null) {
            throw new NullPointerException();
        }

        if (extensions == null) {
            extensions = LocaleExtensions.EMPTY_EXTENSIONS;
        }

        LocaleKey key = new LocaleKey(language, script, country, variant, extensions);
        Locale locale = LOCALECACHE.get(key);
        if (locale == null) {
            locale = new Locale(language, script, country, variant, extensions);
            LOCALECACHE.put(
                    new LocaleKey(
                            locale.getLanguage(),
                            locale.getScript(),
                            locale.getCountry(),
                            locale.getVariant(),
                            locale._extensions),
                    locale);
        }
        return locale;
    }

    private static class LocaleKey implements Comparable<LocaleKey> {
        private BaseLocaleKey _baseKey;
        private String _extKey;

        private LocaleKey(String language, String script, String country, String variant, LocaleExtensions extensions) {
            _baseKey = new BaseLocaleKey(language, script, country, variant);
            _extKey = extensions.getID();
        }

        public boolean equals(Object obj) {
            if (this == obj) {
                return true;
            }
            if (!(obj instanceof LocaleKey)) {
                return false;
            }
            LocaleKey other = (LocaleKey)obj;
            return _baseKey.equals(other._baseKey) && _extKey.equals(other._extKey);
        }

        public int compareTo(LocaleKey other) {
            int res = _baseKey.compareTo(other._baseKey);
            if (res == 0) {
                res = _extKey.compareTo(other._extKey);
            }
            return res;
        }

        public int hashCode() {
            return _baseKey.hashCode() | _extKey.hashCode();
        }
    }

    public boolean equals(Object obj) {
        return this == obj ||
                (obj instanceof Locale)
                && _baseLocale.equals(((Locale)obj)._baseLocale)
                && _extensions.equals(((Locale)obj)._extensions);
    }

    public int hashCode() {
        return _baseLocale.hashCode() | _extensions.hashCode();
    }

    public String toString() {
        return _baseLocale.getJava6String();
    }

    public String getLanguage() {
        return _baseLocale.getLanguage();
    }

    public String getScript() {
        return _baseLocale.getScript();
    }

    public String getCountry() {
        return _baseLocale.getRegion();
    }

    public String getVariant() {
        return _baseLocale.getVariant();
    }

    /**
     * <span style="background-color: #00ccff; font-weight: bold">New API</span>
     * Returns the extension string associated with the specified character key
     * for this locale.
     * @param key The character extension key
     * @return The extension string, or null if the extension associated
     * with the specified key is not available for this locale.
     */
    public String getExtension(char key) {
        return _extensions.getExtensionValue(Character.valueOf(key));
    }

    /**
     * <span style="background-color: #00ccff; font-weight: bold">New API</span>
     * Returns the set of the extension character keys for this locale.
     * @return The set of the extension character keys, or null if no extensions
     * are available for this locale.
     */
    public Set<Character> getExtensionKeys() {
        return _extensions.getExtensionKeys();
    }

    /**
     * <span style="background-color: #00ccff; font-weight: bold">New API</span>
     * Returns the Unicode locale keyword type value associated with the specified
     * key for this locale.
     * @param key The Unicode locale keyword key
     * @return The Unicode locale keyword type value string, or null if the keyword
     * associated with the specified key is not available for this locale.
     */
    public String getLocaleKeywordValue(String key) {
        return _extensions.getLocaleKeywordType(key);
    }

    /**
     * <span style="background-color: #00ccff; font-weight: bold">New API</span>
     * Returns the set of Unicode locale keyword keys for this locale.
     * @return The set of the Unicode locale keyword keys, or null if no keywords
     * are available for this locale.
     */
    public Set<String> getLocaleKeywordKeys() {
        return _extensions.getLocaleKeywordKeys();
    }

    /**
     * <span style="background-color: #00ccff; font-weight: bold">New API</span>
     * Returns the private use code for this locale.
     */
    public String getPrivateUse() {
        return _extensions.getPrivateUse();
    }


    /**
     * <span style="background-color: #00ccff; font-weight: bold">New API</span>
     * Returns the language tag string for this locale.  If this locale
     * has any fields which do not satisfy the language tag's syntax
     * requirement, they will be dropped from the result language tag.
     */
    public String toLanguageTag() {
        return LanguageTag.toLanguageTag(_baseLocale, _extensions);
    }

    /**
     * <span style="background-color: #00ccff; font-weight: bold">New API</span>
     * Returns a locale for the specified language tag string.  If the
     * specified language tag contains any invalid subtags, the first
     * invalid subtag and all following subtags are ignored.
     * @param langtag the language tag
     */
    public static Locale forLanguageTag(String langtag) {
        LanguageTag tag = null;
        while (true) {
            try {
                tag = LanguageTag.parse(langtag);
                break;
            } catch (InvalidLocaleIdentifierException e) {
                // remove the last subtag and try it again
                int idx = langtag.lastIndexOf('-');
                if (idx == -1) {
                    // no more subtags
                    break;
                }
                langtag = langtag.substring(0, idx);
            }
        }
        if (tag == null) {
            return Locale.ROOT;
        }

        LocaleBuilder bldr = new LocaleBuilder();

        bldr.setLanguage(tag.getLanguage()).setScript(tag.getScript())
            .setRegion(tag.getRegion()).setVariant(tag.getVariant());

        Set<Extension> exts = tag.getExtensions();
        if (exts != null) {
            Iterator<Extension> itr = exts.iterator();
            while (itr.hasNext()) {
                Extension e = itr.next();
                bldr.setExtension(e.getSingleton(), e.getValue());
            }
        }
        bldr.setPrivateUse(tag.getPrivateUse());

        return bldr.create();
    }

    /**
     * <span style="background-color: #00ccff; font-weight: bold">New API</span>
     * Returns a locale without extensions.
     */
    public Locale getBaseLocale() {
        if (_extensions == LocaleExtensions.EMPTY_EXTENSIONS) {
            return this;
        }
        return getInstance(_baseLocale.getLanguage(), _baseLocale.getScript(),
                _baseLocale.getRegion(), _baseLocale.getVariant(), LocaleExtensions.EMPTY_EXTENSIONS);
    }

    /**
     * <span style="background-color: #00ccff; font-weight: bold">New API</span>
     * LocaleBuilder is a utility class used for building an instance of
     * Locale from the given field values configured by the setter.
     */
    public static class LocaleBuilder {

        private InternalLocaleBuilder _locbld = new InternalLocaleBuilder();

        /**
         * <span style="background-color: #00ccff; font-weight: bold">New API</span>
         * Constructs an empty LocaleBuilder object.
         */
        public LocaleBuilder() {
        }

        /**
         * <span style="background-color: #00ccff; font-weight: bold">New API</span>
         * Sets the locale to this builder.
         * @param loc the locale
         */
        public LocaleBuilder setLocale(Locale loc) {
            setLanguage(loc.getLanguage()).setScript(loc.getScript())
                .setRegion(loc.getCountry()).setVariant(loc.getVariant());

            Set<Character> extKeys = loc.getExtensionKeys();
            if (extKeys != null) {
                Iterator<Character> itr = extKeys.iterator();
                while (itr.hasNext()) {
                    char key = itr.next().charValue();
                    String value = loc.getExtension(key);
                    if (value != null && value.length() > 0) {
                        setExtension(key, value);
                    }
                }
            }
            setPrivateUse(loc.getPrivateUse());
            return this;
        }

        /**
         * <span style="background-color: #00ccff; font-weight: bold">New API</span>
         * Sets the language to this builder.
         * @param language the language
         */
        public LocaleBuilder setLanguage(String language) {
            _locbld.setLanguage(language);
            return this;
        }

        /**
         * <span style="background-color: #00ccff; font-weight: bold">New API</span>
         * Sets the script to this builder.
         * @param script the script
         */
        public LocaleBuilder setScript(String script) {
            _locbld.setScript(script);
            return this;
        }

        /**
         * <span style="background-color: #00ccff; font-weight: bold">New API</span>
         * Sets the region to this builder.
         * @param region the region
         */
        public LocaleBuilder setRegion(String region) {
            _locbld.setRegion(region);
            return this;
        }

        /**
         * <span style="background-color: #00ccff; font-weight: bold">New API</span>
         * Sets the variant to this builder.
         * @param variant the variant
         */
        public LocaleBuilder setVariant(String variant) {
            _locbld.setVariant(variant);
            return this;
        }

        /**
         * <span style="background-color: #00ccff; font-weight: bold">New API</span>
         * Sets the extension to this builder.
         * @param key the extension character key
         * @param value the extension value
         */
        public LocaleBuilder setExtension(char key, String value) {
            _locbld.setExtension(key, value);
            return this;
        }

        /**
         * <span style="background-color: #00ccff; font-weight: bold">New API</span>
         * Sets the Unicode locale keyword to this builder.
         * @param key the Unicode locale key
         * @param type the Unicode locake type
         */
        public LocaleBuilder setLocaleKeyword(String key, String type) {
            _locbld.setLocaleKeyword(key, type);
            return this;
        }

        /**
         * <span style="background-color: #00ccff; font-weight: bold">New API</span>
         * Sets the private use to this builder.
         * @param privuse the private use
         */
        public LocaleBuilder setPrivateUse(String privuse) {
            _locbld.setPrivateUse(privuse);
            return this;
        }

        /**
         * <span style="background-color: #00ccff; font-weight: bold">New API</span>
         * Clear all internal fields in this builder.
         */
        public LocaleBuilder clear() {
            _locbld.clear();
            return this;
        }

        /**
         * <span style="background-color: #00ccff; font-weight: bold">New API</span>
         * Returns an instance of locale created from locale fields configured by
         * the setters in this locale builder instance.  When any invalid values were
         * set, this method ignore them and will always return a valid locale.
         */
        public Locale create() {
            BaseLocale base = _locbld.getBaseLocale(false);
            LocaleExtensions ext = _locbld.getLocaleExtensions(false);
            return Locale.getInstance(base.getLanguage(), base.getScript(), base.getRegion(), base.getVariant(), ext);
        }

        /**
         * <span style="background-color: #00ccff; font-weight: bold">New API</span>
         * Returns an instance of locale created from locale fields configured by
         * the setters in this locale builder instance.  When any invalid values were
         * set, this method returns null.
         */
        public Locale createStrict() {
            BaseLocale base = _locbld.getBaseLocale(true);
            LocaleExtensions ext = _locbld.getLocaleExtensions(true);
            if (base == null || ext == null) {
                return null;
            }
            return Locale.getInstance(base.getLanguage(), base.getScript(), base.getRegion(), base.getVariant(), ext);
        }
    }
}
