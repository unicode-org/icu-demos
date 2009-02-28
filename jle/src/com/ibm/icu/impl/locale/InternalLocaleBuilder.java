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

    private HashMap<Character, String> _extMap;
    private HashMap<String, String> _kwdMap;
    private String _privateuse = "";

    private static final char PRIVUSE = 'x';
    private static final char LOCALESINGLETON = 'u';

    private static final String LANGTAGSEP = "-";
    private static final String LOCALESEP = "_";

    private static final int DEFAULTMAPCAPACITY = 4;

    public InternalLocaleBuilder() {
    }

    public InternalLocaleBuilder(FieldHandler handler) {
        _handler = handler;
    }

    public InternalLocaleBuilder setLanguage(String language) {
        _language = (language == null) ? "" : language;
        return this;
    }

    public InternalLocaleBuilder setScript(String script) {
        _script = (script == null) ? "" : script;
        return this;
    }

    public InternalLocaleBuilder setRegion(String region) {
        _region = (region == null) ? "" : region;
        return this;
    }

    public InternalLocaleBuilder setVariant(String variant) {
        _variant = (variant == null) ? "" : variant;
        return this;
    }

    public InternalLocaleBuilder setLocaleKeyword(String key, String type) {
        if (key == null || key.length() == 0) {
            // do nothing
            return this;
        }
        if (_kwdMap == null) {
            _kwdMap = new HashMap<String, String>(DEFAULTMAPCAPACITY);
        }
        // type could be null/empty, which is used as a mark for deletion
        // when merging the contents in _kwdMap with the value
        // set by setExtension('u', "<locale keyword subtags>")
        _kwdMap.put(key, type);
        return this;
    }

    public InternalLocaleBuilder setExtension(char singleton, String value) {
        // singleton char to lower case
        singleton = AsciiUtil.toLower(singleton);

        if (singleton == LOCALESINGLETON) {
            if (_kwdMap != null) {
                // blow out the keywords currently set
                _kwdMap.clear();
            }
        }
        if (value == null || value.length() == 0) {
            if (_extMap != null) {
                _extMap.remove(Character.toString(singleton));
            }
        } else {
            if (_extMap == null) {
                _extMap = new HashMap<Character, String>(DEFAULTMAPCAPACITY);
            }
            _extMap.put(Character.valueOf(singleton), value);
        }
        return this;
    }

    public InternalLocaleBuilder setPrivateUse(String privateuse) {
        _privateuse = (privateuse == null) ? "" : privateuse;
        return this;
    }

    public InternalLocaleBuilder clear() {
        _language = "";
        _script = "";
        _region = "";
        _variant = "";
        _privateuse = "";
        if (_extMap != null) {
            _extMap.clear();
        }
        if (_kwdMap != null) {
            _kwdMap.clear();
        }
        return this;
    }

    public InternalLocaleBuilder removeLocaleExtensions() {
        _extMap = null;
        _kwdMap = null;
        _privateuse = "";
        return this;
    }

    public BaseLocale getBaseLocale(boolean bStrict) {
        String language = _handler.process(FieldType.LANGUAGE, _language);
        String script = _handler.process(FieldType.SCRIPT, _script);
        String region = _handler.process(FieldType.REGION, _region);
        String variant = _handler.process(FieldType.VARIANT, _variant);

        if (bStrict) {
            // when an invalid value was set in any base locale fields,
            // returns null to signal the error in strict mode.
            if (language == null || script == null || region == null || variant == null) {
                return null;
            }
        }
        return BaseLocale.getInstance(language, script, region, variant);
    }

    public LocaleExtensions getLocaleExtensions(boolean bStrict) {
        TreeMap<Character, String> extMap = null;
        TreeMap<String, String> kwdMap = null;
        String locKwdExt = null;

        // process keywords
        if (_kwdMap != null && _kwdMap.size() > 0) {
            Set<Map.Entry<String, String>> kwds = _kwdMap.entrySet();
            for (Map.Entry<String, String> kwd : kwds) {
                String type = kwd.getValue();
                if (type == null || type.length() == 0) {
                    // marked for deletion
                    continue;
                }

                String key = _handler.process(FieldType.LOCALEKEY, kwd.getKey());
                type = _handler.process(FieldType.LOCALETYPE, type);

                if (key == null || type == null) {
                    if (bStrict) {
                        // error return in strict mode
                        return null;
                    }
                    continue;
                }

                if (kwdMap == null) {
                    kwdMap = new TreeMap<String, String>();
                }
                kwdMap.put(key.intern(), type.intern());
            }
        }

        // process extensions
        if (_extMap != null) {
            Set<Map.Entry<Character, String>> exts = _extMap.entrySet();
            for (Map.Entry<Character, String> ext : exts) {
                Character key = ext.getKey();
                char singleton = key.charValue();

                if (!AsciiUtil.isAlpha(singleton) || singleton == PRIVUSE) {
                    if (bStrict) {
                        // error return when non-alpha extension key is detected
                        // or 'x' which is reserved for private use
                        return null;
                    }
                    continue;
                }

                String value = _handler.process(FieldType.EXTENSION, ext.getValue());
                if (value == null) {
                    if (bStrict) {
                        // invalid extension value
                        return null;
                    }
                    continue;
                }

                if (singleton == LOCALESINGLETON) {
                    // locale keywords will set by setExtension will be processed later
                    locKwdExt = value;
                    continue;
                }

                if (extMap == null) {
                    extMap = new TreeMap<Character, String>();
                }
                extMap.put(key, value.intern());
            }
        }

        // merging locale keywords set via setExtension
        if (locKwdExt != null) {
            String[] kwdtags = locKwdExt.split(LOCALESEP);
            if ((kwdtags.length % 2) != 0) {
                // number of keyword subtags must be even number
                if (bStrict) {
                    return null;
                }
            }

            int idx = 0;
            while (idx < kwdtags.length - 1 /* -1 for preventing index overrun for odd number case */) {
                String kwkey = _handler.process(FieldType.LOCALEKEY, kwdtags[idx++]);
                String kwtype = kwdtags[idx++];

                if (kwkey == null) {
                    if (bStrict) {
                        return null;
                    }
                    continue;
                }
                // check if the key was overridden by setLocaleKeyword after this single keyword
                // string was set by setExtension
                if (_kwdMap != null) {
                    if (_kwdMap.containsKey(kwkey)) {
                        // new value was set
                        continue;
                    }
                }
                kwtype = _handler.process(FieldType.LOCALETYPE, kwtype);
                if (kwtype == null) {
                    if (bStrict) {
                        return null;
                    } else {
                        // ignore this key/type pair
                        continue;
                    }
                }
                if (kwdMap == null) {
                    kwdMap = new TreeMap<String, String>();
                }
                kwdMap.put(kwkey.intern(), kwtype.intern());
            }
        }

        // set canonical locale keyword extension string to the extension map
        if (kwdMap != null) {
            StringBuilder buf = new StringBuilder();
            LocaleExtensions.mapToLocaleExtensionString(kwdMap, buf);
            if (extMap == null) {
                extMap = new TreeMap<Character, String>();
            }
            extMap.put(Character.valueOf(LOCALESINGLETON), buf.toString().intern());
        }

        // private use
        String privateuse = _handler.process(FieldType.PRIVATEUSE, _privateuse);
        if (privateuse == null) {
            if (bStrict) {
                return null;
            }
            privateuse = "";
        }

        return LocaleExtensions.getInstance(extMap, kwdMap, privateuse);
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
            if (value.length() > 0 && !validate(type, value)) {
                return null;
            }
            return value;
        }

        protected String map(FieldType type, String value) {
            switch (type) {
            case LANGUAGE:
                value = LanguageTag.getShortLanguageCode(AsciiUtil.toLowerString(value));
                break;
            case SCRIPT:
                if (value.length() > 0) {
                    StringBuilder buf = new StringBuilder();
                    buf.append(AsciiUtil.toUpper(value.charAt(0)));
                    for (int i = 1; i < value.length(); i++) {
                        buf.append(AsciiUtil.toLower(value.charAt(i)));
                    }
                    value = buf.toString();
                }
                break;
            case REGION:
                value = AsciiUtil.toUpperString(value);
                break;
            case VARIANT:
                // Java variant is case sensitive - so no case mapping here
                value = value.replaceAll(LANGTAGSEP, LOCALESEP);
                break;
            case LOCALEKEY:
            case LOCALETYPE:
            case EXTENSION:
            case PRIVATEUSE:
                value = AsciiUtil.toLowerString(value).replaceAll(LANGTAGSEP, LOCALESEP);
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
