/*
 *******************************************************************************
 * Copyright (C) 2009, International Business Machines Corporation and         *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
package com.ibm.icu.util;

import java.util.Iterator;
import java.util.Set;

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

        Builder bldr = new Builder();

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
     * Builder is used to build instances of Locale of
     * Locale from values configured by the setter.  
     * <p>
     * Builder supports the 'langtag' production of RFC 4646.
     * Language tags consist of the ASCII digits, upper and lower case
     * letters, and hyphen (which appears only as a field separator).
     * As a convenience, underscores are accepted and normalized to
     * hyphen.  Values with any other character are ill-formed.  Since
     * language tags are case-insensitive, they are normalized
     * internally to lower case, case distinctions are <b>not</b>
     * preserved by the builder.
     * <p> 
     * Two creation methods are available, <code>create()</code> and
     * <code> createStrict()</code>.  <code>Create()</code> ignores
     * all ill-formed values, always returning a Locale.
     * <code>CreateStrict()</code> will always return null if any
     * value is ill-formed.
     * <p>
     * Builders can be reused; <code>clear()</code> resets all fields
     * to their default values.
     * @see Builder#create
     * @see Builder#createStrict
     * @see Builder#clear
     */
    public static class Builder {

        private InternalLocaleBuilder _locbld = new InternalLocaleBuilder();

        /**
         * <span style="background-color: #00ccff; font-weight:
         * bold">New API</span> Constructs an empty Builder.
         * The default values of all fields, extensions, and private
         * use information are empty, the language is undefined.
         */
        public Builder() {
        }

        /**
         * <span style="background-color: #00ccff; font-weight: bold">New API</span>
         * Resets the builder to match the provided locale.  Fields that do not
         * conform to BCP47 syntax are ill-formed.
         * @param loc the locale
         * @return this builder
         * @thorws IllegalArgumentException if <code>loc</code> has any ill-formed
         * fields.
         * @throws NullPointerException if <code>loc</code> is null
         */
        public Builder setLocale(Locale loc) {
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
         * Resets the builder to match the provided languge tag.
         * @param loc the locale
         * @return this builder
         * @thorws IllegalArgumentException if <code>langtag</code> is not well-formed.
         * @throws NullPointerException if <code>langtag</code> is null
         * @see Locale#forLanguageTag(String)
         */
        public Builder setLanguageTag(String langtag) {
            LanguageTag tag = null;
            try {
                tag = LanguageTag.parse(langtag);
            } catch (InvalidLocaleIdentifierException e) {
                throw new IllegalArgumentException(e);
            }

            // base locale fields
            setLanguage(tag.getLanguage()).setScript(tag.getScript())
                .setRegion(tag.getRegion()).setVariant(tag.getVariant());

            // extensions
            Set<Extension> exts = tag.getExtensions();
            if (exts != null) {
                Iterator<Extension> itr = exts.iterator();
                while (itr.hasNext()) {
                    Extension e = itr.next();
                    setExtension(e.getSingleton(), e.getValue());
                }
            }
            // private use
            setPrivateUse(tag.getPrivateUse());
            return this;
        }

        /**
         * <span style="background-color: #00ccff; font-weight: bold">New API</span>
         * Sets the language.  If language is the empty string,
         * the language is defaulted.  Language should be a two or 
         * three-letter language code as defined in ISO639.
         * Well-formed values are any string of two to eight ASCII letters.
         * @param language the language
         * @return this builder
         * @thorws IllegalArgumentException if <code>language</code> is not well-formed
         * @throws NullPointerException if <code>language</code> is null
         */
        public Builder setLanguage(String language) {
            try {
                _locbld.setLanguage(language);
            } catch (InvalidLocaleIdentifierException e) {
                throw new IllegalArgumentException(e);
            }
            return this;
        }

        /**
         * <span style="background-color: #00ccff; font-weight:
         * bold">New API</span> Sets the script.  If script is 
         * the empty string, the script is defaulted.  Scripts should
         * be a four-letter script code as defined in ISO 15924.
         * Well-formed values are any string of four ASCII letters.
         * @param script the script
         * @return this builder
         * @thorws IllegalArgumentException if <code>script</code> is not well-formed
         * @throws NullPointerException if <code>script</code> is null
         */
        public Builder setScript(String script) {
            try {
                _locbld.setScript(script);
            } catch (InvalidLocaleIdentifierException e) {
                throw new IllegalArgumentException(e);
            }
            return this;
        }

        /**
         * <span style="background-color: #00ccff; font-weight:
         * bold">New API</span> Sets the region.  If region is
         * the empty string, the region is defaulted.  Regions should
         * be a two-letter ISO 3166 code or a three-digit M. 49 code.
         * Well-formed values are any two-letter or three-digit
         * string.
         * @param region the region
         * @return this builder
         * @thorws IllegalArgumentException if <code>region</code> is not well-formed
         * @throws NullPointerException if <code>region</code> is null
         */
        public Builder setRegion(String region) {
            try {
                _locbld.setRegion(region);
            } catch (InvalidLocaleIdentifierException e) {
                throw new IllegalArgumentException(e);
            }
            return this;
        }

        /**
         * <span style="background-color: #00ccff; font-weight:
         * bold">New API</span> Sets the variant.  If variant is
         * or the empty string, the variant is defaulted.  Variants
         * should be registered variants (see 
         * <a href="http://www.iana.org/assignments/language-subtag-registry">
         * IANA Language Subtag Registry</a>) for the prefix.  Well-formed
         * variants are any series of fields of either four characters
         * starting with a digit, or five to eight alphanumeric
         * characters, separated by hyphen or underscore.
         * @param variant the variant
         * @return this builder
         * @thorws IllegalArgumentException if <code>variant</code> is not well-formed
         * @throws NullPointerException if <code>variant</code> is null
         */
        public Builder setVariant(String variant) {
            try {
                _locbld.setVariant(variant);
            } catch (InvalidLocaleIdentifierException e) {
                throw new IllegalArgumentException(e);
            }
            return this;
        }

        /**
         * <span style="background-color: #00ccff; font-weight: bold">New API</span>
         * Sets the extension for the given key.  If the value is
         * the empty string, the extension is removed.  Well-formed
         * keys are the <code>[0-9][a-wy-zA-WY-Z]</code>.  Well-formed
         * values are any series of fields of two to eight
         * alphanumeric characters, separated by hyphen or underscore.
         * <p>
         * <b>note</b>:The extension 'u' is used for Locale Keywords.
         * Setting the 'u' extension replaces any existing Locale
         * keyword values with those defined in the extension.  To be
         * well-formed, a value for the 'u' extension must meet the
         * additional constraint that the number of fields be even
         * (fields represent key value pairs, where the value is
         * mandatory).
         * @param key the extension key
         * @param value the extension value
         * @return this builder
         * @thorws IllegalArgumentException if <code>key</code> or <code>value</code>
         * is not well-formed
         * @throws NullPointerException if <code>value</code> is null
         * @see #setLocaleKeyword
         */
        public Builder setExtension(char key, String value) {
            try {
                _locbld.setExtension(key, value);
            } catch (InvalidLocaleIdentifierException e) {
                throw new IllegalArgumentException(e);
            }
            return this;
        }

        /**
         * <span style="background-color: #00ccff; font-weight: bold">New API</span>
         * Sets the Unicode locale type for the given key.  If the type is
         * the empty string, the locale key is removed.  Key must not be null.
         * Well-formed types are strings of 2 to 8 alphanums.
         * <p>
         * <b>Note</b>:Setting the 'u' extension replaces all locale
         * keywords with those defined in the extension.
         * @param key the  key
         * @param type the Unicode locale type
         * @return this builder
         * @thorws IllegalArgumentException if <code>key</code> or <code>type</code>
         * is not well-formed
         * @throws NullPointerException if <code>key</code> or <code>type</code> is null
         * @see #setExtension
         */
        public Builder setLocaleKeyword(String key, String type) {
            try {
                _locbld.setLocaleKeyword(key, type);
            } catch (InvalidLocaleIdentifierException e) {
                throw new IllegalArgumentException(e);
            }
            return this;
        }

        /**
         * <span style="background-color: #00ccff; font-weight: bold">New API</span>
         * Sets the private use code.  If privuse is the empty
         * string, the private use code is removed.  Well-formed
         * values are any series of fields of one to eight alphanums,
         * separated by hyphen or underscore.
         * @param privuse the private use
         * @return this builder
         * @thorws IllegalArgumentException if <code>privuse</code> is not well-formed
         * @throws NullPointerException if <code>privuse</code> is null
         */
        public Builder setPrivateUse(String privuse) {
            try {
                _locbld.setPrivateUse(privuse);
            } catch (InvalidLocaleIdentifierException e) {
                throw new IllegalArgumentException(e);
            }
            return this;
        }

        /**
         * <span style="background-color: #00ccff; font-weight: bold">New API</span>
         * Resets the buffer to its initial, default state.
         * @return this builder
         */
        public Builder clear() {
            _locbld.clear();
            return this;
        }

        /**
         * <span style="background-color: #00ccff; font-weight: bold">New API</span>
         * Returns an instance of Locale created from the fields set
         * on this builder.
         * @return a new Locale
         */
        public Locale create() {
            BaseLocale base = _locbld.getBaseLocale();
            LocaleExtensions ext = _locbld.getLocaleExtensions();
            return Locale.getInstance(base.getLanguage(), base.getScript(), base.getRegion(), base.getVariant(), ext);
        }
    }
}
