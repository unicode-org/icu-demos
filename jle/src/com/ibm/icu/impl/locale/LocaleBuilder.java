/*
 *******************************************************************************
 * Copyright (C) 2009, International Business Machines Corporation and         *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
package com.ibm.icu.impl.locale;

import java.util.Map;
import java.util.Set;
import java.util.TreeMap;

public final class LocaleBuilder {

    private String _language = "";
    private String _script = "";
    private String _region = "";
    private String _variant = "";

    private FieldHandler _handler = FieldHandler.DEFAULT;

    private TreeMap<String,String> _extensions;
    private TreeMap<String,String> _keywords;
    private String _privateuse = "";

    private static final char PRIVUSE = 'x';
    private static final char LOCALESINGLETON = 'u';

    private static final String LANGTAGSEP = "-";
    private static final String LOCALESEP = "_";

    public LocaleBuilder() {
    }

    public LocaleBuilder(FieldHandler handler) {
        _handler = handler;
    }

    public LocaleBuilder setLanguage(String language) throws InvalidLocaleException {
        _language = (language == null || language.length() == 0) ?
                "" : _handler.process(FieldType.LANGUAGE, language);
        return this;
    }

    public LocaleBuilder setScript(String script) throws InvalidLocaleException {
        _script = (script == null || script.length() == 0) ?
                "" : _handler.process(FieldType.SCRIPT, script);
        return this;
    }

    public LocaleBuilder setRegion(String region) throws InvalidLocaleException {
        _region = (region == null || region.length() == 0) ?
                "" : _handler.process(FieldType.REGION, region);
        return this;
    }

    public LocaleBuilder setVariant(String variant) throws InvalidLocaleException {
        _variant = (variant == null || variant.length() == 0) ?
                "" : _handler.process(FieldType.VARIANT, variant);
        return this;
    }

    public LocaleBuilder setLocaleKeyword(String key, String type) throws InvalidLocaleException {
        if (key == null || key.length() == 0) {
            // ignore?
            return this;
        }
        if (type == null || type.length() == 0) {
            if (_keywords != null) {
                _keywords.remove(key);
            }
        } else {
            key = _handler.process(FieldType.LOCALEKEY, key);
            type = _handler.process(FieldType.LOCALETYPE, type);
            if (_keywords == null) {
                _keywords = new TreeMap<String,String>();
            }
            _keywords.put(key, type);
        }
        return this;
    }

    public LocaleBuilder setExtension(char singleton, String value) throws InvalidLocaleException {
        // singleton char to lower case
        singleton = AsciiUtil.toLower(singleton);

        if (singleton == PRIVUSE) {
            throw new InvalidLocaleException("Letter '" + singleton
                    + "' is reserved for the private use.");
        }

        value = value.replaceAll(LANGTAGSEP, LOCALESEP);

        if (singleton == LOCALESINGLETON) {
            // keep locale keywords maintained in _keywords
            TreeMap<String,String> kwds = LocaleExtension.parseKeywordSubtags(value, LOCALESEP);
            Set<Map.Entry<String,String>> entries = kwds.entrySet();
            for (Map.Entry<String,String> entry : entries) {
                setLocaleKeyword(entry.getKey(), entry.getValue());
            }
        } else if (!AsciiUtil.isAlphaNumeric(singleton)) {
            throw new InvalidLocaleException("Letter '" + singleton
                    + "' cannot be used for the singleton extensions.");
        } else {
            if (value == null || value.length() == 0) {
                if (_extensions != null) {
                    _extensions.remove(Character.toString(singleton));
                }
            } else {
                value = _handler.process(FieldType.EXTENSION, value);
                if (_extensions == null) {
                    _extensions = new TreeMap<String,String>();
                }
                _extensions.put(Character.toString(singleton).intern(), value);
            }
        }
        return this;
    }

    public LocaleBuilder setPrivateUse(String privuse) throws InvalidLocaleException {
        _privateuse = (privuse == null || privuse.length() == 0) ?
                "" : _handler.process(FieldType.PRIVATEUSE, privuse.replaceAll(LANGTAGSEP, LOCALESEP));
        return this;
    }

    public LocaleBuilder removeLocaleExtension() {
        _keywords = null;
        _extensions = null;
        _privateuse = "";
        return this;
    }

    public BaseLocale getBaseLocale() {
        return BaseLocale.get(_language, _script, _region, _variant);
    }

    public LocaleExtension getLocaleExtension() {
        TreeMap<String,String> map = null;
        if (_extensions != null || _keywords != null) {
            // Create a new map
            map = new TreeMap<String,String>();
            if (_extensions != null) {
                map.putAll(_extensions);
            }
            if (_keywords != null) {
                // Add keywords as an extension
                StringBuilder buf = new StringBuilder();
                String lockwd = LocaleExtension.mapToLocaleExtensionString(_keywords, buf);
                map.put(Character.toString(LOCALESINGLETON), lockwd);
            }
        }
        LocaleExtension locext = null;
        try {
            locext = LocaleExtension.get(map, _privateuse);
        } catch (InvalidLocaleException e) {
            // It should never happen...
        }
        return locext;
    }

    protected enum FieldType {
        LANGUAGE,
        SCRIPT,
        REGION,
        VARIANT,
        LOCALEKEY,
        LOCALETYPE,
        EXTENSION,
        PRIVATEUSE
    }

    public static class FieldHandler {
        public static FieldHandler DEFAULT = new FieldHandler();

        protected FieldHandler() {
        }

        public String process(FieldType type, String value) throws InvalidLocaleException {
            value = map(type, value);
            if (!validate(type, value)) {
                throw new InvalidLocaleException("Invalid field value: " + value);
            }
            return value.intern();
        }

        protected String map(FieldType type, String value) {
            switch (type) {
            case LANGUAGE:
                value = LanguageCode.getShortest(AsciiUtil.toLowerString(value));
                break;
            case SCRIPT:
                //TODO
                break;
            case REGION:
                value = AsciiUtil.toLowerString(value);
                break;
            case VARIANT:
                // Java variant is case sensitive
                break;
            case LOCALEKEY:
            case LOCALETYPE:
            case EXTENSION:
            case PRIVATEUSE:
                value = AsciiUtil.toLowerString(value);
                break;
            }
            return value;
        }

        protected boolean validate(FieldType type, String value) {
            boolean isValid = false;
            String[] subtags;

            switch (type) {
            case LANGUAGE:
                isValid = LanguageTag.isLanguageSubtag(value);
                break;
            case SCRIPT:
                isValid = LanguageTag.isScriptSubtag(value);
                break;
            case REGION:
                isValid = LanguageTag.isRegionSubtag(value);
                break;
            case VARIANT:
                isValid = LanguageTag.isVariantSubtag(value);
                break;
            case LOCALEKEY:
                isValid = LanguageTag.isExtensionSubtag(value);
                break;
            case LOCALETYPE:
                isValid = LanguageTag.isExtensionSubtag(value);
                break;
            case EXTENSION:
                subtags = value.split(LOCALESEP);
                for (String subtag : subtags) {
                    isValid = LanguageTag.isExtensionSubtag(subtag);
                }
                break;
            case PRIVATEUSE:
                subtags = value.split(LOCALESEP);
                for (String subtag : subtags) {
                    isValid = LanguageTag.isPrivateuseValueSubtag(subtag);
                }
                break;
            }
            return isValid;
        }
    }
}
