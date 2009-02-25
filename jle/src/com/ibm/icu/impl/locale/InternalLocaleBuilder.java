/*
 *******************************************************************************
 * Copyright (C) 2009, International Business Machines Corporation and         *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
package com.ibm.icu.impl.locale;

import java.util.HashMap;
import java.util.Map;
import java.util.TreeMap;

public final class InternalLocaleBuilder {

    private String _language = "";
    private String _script = "";
    private String _region = "";
    private String _variant = "";

    private FieldHandler _handler = FieldHandler.DEFAULT;

    private TreeMap<Character, String> _extMap;
    private TreeMap<String, String> _kwdMap;
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
            variant = variant.replaceAll(LANGTAGSEP, LOCALESEP);
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
            // key must not be empty
            return false;
        }
        if (type == null || type.length() == 0) {
            if (_kwdMap != null) {
                _kwdMap.remove(key);
            }
        } else {
            key = _handler.process(FieldType.LOCALEKEY, key);
            type = _handler.process(FieldType.LOCALETYPE, type);
            if (key == null || type == null) {
                return false;
            }
            if (_kwdMap == null) {
                _kwdMap = new TreeMap<String,String>();
            }
            _kwdMap.put(key, type);
        }

        syncLocaleExtension();

        return true;
    }

    public boolean setExtension(char singleton, String value) {
        if (!AsciiUtil.isAlphaNumeric(singleton)) {
            return false;
        }

        // singleton char to lower case
        singleton = AsciiUtil.toLower(singleton);

        if (singleton == PRIVUSE) {
            // private use must be set via setPrivateUse
            return false;
        }

        // value to lower case
        value = AsciiUtil.toLowerString(value).replaceAll(LANGTAGSEP, LOCALESEP);

        if (value == null || value.length() == 0) {
            if (_extMap != null) {
                _extMap.remove(Character.toString(singleton));
            }
            if (singleton == LOCALESINGLETON) {
                // clear the current keyword entries if the specified
                // singleton is for Unicode locale keywords
                if (_kwdMap != null) {
                    _kwdMap.clear();
                }
            }
            return true;
        }

        if (singleton == LOCALESINGLETON) {
            if (value == null || value.length() == 0) {
                // clear the current keyword entries if the specified
                // singleton is for Unicode locale keywords
                if (_kwdMap != null) {
                    _kwdMap.clear();
                }
            } else {
                String[] kwdtags = value.split(LOCALESEP);
                if ((kwdtags.length % 2) != 0) {
                    // number of keyword subtags must be even
                    return false;
                }

                // validate all locale key/type pairs first
                Map<String, String> kwds = new HashMap<String, String>();
                int idx = 0;
                while (idx < kwdtags.length) {
                    String kwkey = _handler.process(FieldType.LOCALEKEY, kwdtags[idx++]);
                    String kwtype = _handler.process(FieldType.LOCALETYPE, kwdtags[idx++]);
                    if (kwkey == null || kwtype == null) {
                        // invalid Unicode locale keyword syntax
                        return false;
                    }
                    if (kwds.put(kwkey, kwtype) != null) {
                        // duplicated keys
                        return false;
                    }
                }

                // locale keywords are also maintained in _keywords
                if (_kwdMap == null) {
                    _kwdMap = new TreeMap<String, String>();
                } else {
                    // clear all keywords
                    _kwdMap.clear();
                }

                // put the validated key/type pairs into _keywords
                _kwdMap.putAll(kwds);
            }

            syncLocaleExtension();

        } else {
            value = _handler.process(FieldType.EXTENSION, value);
            if (value == null) {
                return false;
            }
            // Add this extension
            if (_extMap == null) {
                _extMap = new TreeMap<Character, String>();
            }
            _extMap.put(Character.valueOf(singleton), value);
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
        _extMap = null;
        _kwdMap = null;
        _privateuse = "";
        return this;
    }

    public BaseLocale getBaseLocale() {
        return BaseLocale.getInstance(_language, _script, _region, _variant);
    }

    public LocaleExtensions getLocaleExtensions() {
        TreeMap<Character, String> extMap = (_extMap == null || _extMap.size() == 0) ? null : _extMap;
        TreeMap<String, String> kwdMap = (_kwdMap == null || _kwdMap.size() == 0) ? null : _kwdMap;
        return LocaleExtensions.getInstance(extMap, kwdMap, _privateuse);
    }

    private void syncLocaleExtension() {
        // this implementation maintain the Unicode locale keywords
        // and the corresponding extension ("u") in sync.
        if (_kwdMap == null || _kwdMap.size() == 0) {
            if (_extMap != null) {
                _extMap.remove(Character.valueOf(LOCALESINGLETON));
            }
        } else {
            StringBuilder buf = new StringBuilder();
            LocaleExtensions.mapToLocaleExtensionString(_kwdMap, buf);
            if (_extMap == null) {
                _extMap = new TreeMap<Character, String>();
            }
            _extMap.put(Character.valueOf(LOCALESINGLETON), buf.toString());
        }
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
                value = LanguageTag.getShortLanguageCode(AsciiUtil.toLowerString(value));
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
                // variant field could have multiple subtags
                subtags = value.split(LOCALESEP);
                for (String subtag : subtags) {
                    isValid = LanguageTag.isVariantSubtag(subtag);
                    if (!isValid) {
                        break;
                    }
                }
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
                    if (!isValid) {
                        break;
                    }
                }
                break;
            case PRIVATEUSE:
                subtags = value.split(LOCALESEP);
                for (String subtag : subtags) {
                    isValid = LanguageTag.isPrivateuseValueSubtag(subtag);
                    if (!isValid) {
                        break;
                    }
                }
                break;
            }
            return isValid;
        }
    }
}
