/*
 *******************************************************************************
 * Copyright (C) 2009, International Business Machines Corporation and         *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
package com.ibm.icu.impl.locale;

import java.util.HashMap;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;

public final class InternalLocaleBuilder {

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

    public InternalLocaleBuilder() {
    }

    public InternalLocaleBuilder(FieldHandler handler) {
        _handler = handler;
    }

    public String setLanguage(String language) {
        if (language == null || language.length() == 0) {
            _language = "";
        } else {
            String newval = _handler.process(FieldType.LANGUAGE, language);
            if (newval == null) {
                return null;
            }
            _language = newval;
        }
        return _language;
    }

    public String setScript(String script) {
        if (script == null || script.length() == 0) {
            _script = "";
        } else {
            String newval = _handler.process(FieldType.SCRIPT, script);
            if (newval == null) {
                return null;
            }
            _script = newval;
        }
        return _script;
    }

    public String setRegion(String region) {
        if (region == null || region.length() == 0) {
            _region = "";
        } else {
            String newval = _handler.process(FieldType.REGION, region);
            if (newval == null) {
                return null;
            }
            _region = newval;
        }
        return _region;
    }

    public String setVariant(String variant) {
        if (variant == null || variant.length() == 0) {
            _variant = "";
        } else {
            String newval = _handler.process(FieldType.VARIANT, variant);
            if (newval == null) {
                return null;
            }
            _variant = newval;
        }
        return _variant;
    }

    public boolean setLocaleKeyword(String key, String type) {
        if (key == null || key.length() == 0) {
            throw new IllegalArgumentException("The specified key is null or empty");
        }
        if (type == null || type.length() == 0) {
            if (_keywords != null) {
                _keywords.remove(key);
            }
            return true;
        }

        key = _handler.process(FieldType.LOCALEKEY, key);
        type = _handler.process(FieldType.LOCALETYPE, type);
        if (key == null || type == null) {
            return false;
        }
        if (_keywords == null) {
            _keywords = new TreeMap<String,String>();
        }
        _keywords.put(key, type);

        return true;
    }

    public boolean setExtension(char singleton, String value) {
        if (!AsciiUtil.isAlphaNumeric(singleton)) {
            throw new IllegalArgumentException("Letter '" + singleton
                    + "' cannot be used for the singleton extensions.");
        }

        // singleton char to lower case
        singleton = AsciiUtil.toLower(singleton);

        if (singleton == PRIVUSE) {
            throw new IllegalArgumentException("Letter '" + singleton
                    + "' is reserved for the private use.");
        }

        value = value.replaceAll(LANGTAGSEP, LOCALESEP);

        if (singleton == LOCALESINGLETON) {
            // keep locale keywords maintained in _keywords
            Map<String,String> kwds = LocaleExtension.parseKeywordSubtags(value, LOCALESEP);
            Set<Map.Entry<String,String>> entries = kwds.entrySet();

            // check if all key/type pairs are valid, so we can put the set of key/type
            // pairs atomically.
            Map<String,String> validkwds = new HashMap<String,String>(kwds.size());
            for (Map.Entry<String,String> entry : entries) {
                String kwkey = _handler.process(FieldType.LOCALEKEY, entry.getKey());
                String kwtype = _handler.process(FieldType.LOCALETYPE, entry.getValue());
                if (kwkey == null || kwtype == null) {
                    return false;
                }
                validkwds.put(kwkey, kwtype);
            }
            // put the validated key/type pairs in _keywords
            Set<Map.Entry<String,String>> newentries = validkwds.entrySet();
            for (Map.Entry<String,String> newentry : newentries) {
                _keywords.put(newentry.getKey(), newentry.getValue());
            }
        } else {
            if (value == null || value.length() == 0) {
                if (_extensions != null) {
                    _extensions.remove(Character.toString(singleton));
                }
            } else {
                value = _handler.process(FieldType.EXTENSION, value);
                if (value == null) {
                    return false;
                }
                if (_extensions == null) {
                    _extensions = new TreeMap<String,String>();
                }
                _extensions.put(Character.toString(singleton).intern(), value);
            }
        }
        return true;
    }

    public String setPrivateUse(String privuse) {
        if (privuse == null || privuse.length() == 0) {
            _privateuse = "";
        } else {
            String newval = _handler.process(FieldType.PRIVATEUSE, privuse.replaceAll(LANGTAGSEP, LOCALESEP));
            if (newval == null) {
                return null;
            }
            _privateuse = newval;
        }
        return _privateuse;
    }

    public InternalLocaleBuilder removeLocaleExtension() {
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
        return LocaleExtension.get(map, _privateuse);
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

        public String process(FieldType type, String value) {
            value = map(type, value);
            if (!validate(type, value)) {
                return null;
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
