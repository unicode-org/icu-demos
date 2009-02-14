package com.ibm.icu.impl.locale;


public class BaseLocale {

    private String _language = "";
    private String _script = "";
    private String _region = "";
    private String _variant = "";

    private transient String _id = "";
    private transient String _java6id = "";
    private transient BaseLocale _parent;

    private static final char SEPCHAR = '_';

    private static LocaleDataPool<BaseLocaleKey,BaseLocale> BASELOCALEPOOL
        = new LocaleDataPool<BaseLocaleKey,BaseLocale>();

    private static final BaseLocale ROOT;

    static {
        ROOT = new BaseLocale("", "", "", "");
        BASELOCALEPOOL.putPermanent(ROOT.createKey(), ROOT);
    }

    private BaseLocale(String language, String script, String region, String variant) {
        if (language != null) {
            _language = language;
        }
        if (script != null) {
            _script = script;
        }
        if (region != null) {
            _region = region;
        }
        if (variant != null) {
            _variant = variant;
        }
    }

    public static BaseLocale getInstance(String language, String script, String region, String variant) {
        BaseLocaleKey key = new BaseLocaleKey(language, script, region, variant);
        BaseLocale singleton = BASELOCALEPOOL.get(key);
        if (singleton == null) {
            // Create a canonical BaseLocale instance
            singleton = new BaseLocale(language, script, region, variant).canonicalize();
            BASELOCALEPOOL.put(singleton.createKey(), singleton);
        }
        return singleton;
    }

    public boolean equals(Object obj) {
        // Multiple canonicalized BaseLocale instances with the same
        // fields created via the factory method do not co-exist
        // at any moment.
        return (this == obj);
    }

    public int hashCode() {
        return _id.hashCode();
    }

    public String getID() {
        return _id;
    }

    public String getJava6ID() {
        return _java6id;
    }

    public String getLanguage() {
        return _language;
    }

    public String getScript() {
        return _script;
    }

    public String getRegion() {
        return _region;
    }

    public String getVariant() {
        return _variant;
    }

    public BaseLocale getParent() {
        return _parent;
    }

    public String toString() {
        return "language=" + _language + ";script=" + _script
                + ";region=" + _region + ";variant=" + _variant;
    }

    private BaseLocale canonicalize() {

        StringBuilder id = new StringBuilder();

        int languageLen = _language.length();
        int scriptLen = _script.length();
        int regionLen = _region.length();
        int variantLen = _variant.length();

        if (languageLen > 0) {
            // language to lower case
            _language = AsciiUtil.toLowerString(_language).intern();

            id.append(_language);
        }

        if (scriptLen > 0) {
            // script - the first letter to upper case, the rest to lower case
            StringBuilder buf = new StringBuilder(AsciiUtil.toUpper(_script.charAt(0)));
            for (int i = 1; i < _script.length(); i++) {
                buf.append(AsciiUtil.toLower(_script.charAt(i)));
            }
            _script = buf.toString().intern();

            if (languageLen > 0) {
                id.append(SEPCHAR);
            }
            id.append(_script);
        }

        if (regionLen > 0) {
            // region to upper case
            _region = AsciiUtil.toUpperString(_region).intern();

            id.append(SEPCHAR);
            id.append(_region);
        }

        if (variantLen > 0) {
            // variant is case sensitive in JDK
            _variant = _variant.intern();

            if (regionLen == 0) {
                id.append(SEPCHAR);
            }
            id.append(SEPCHAR);
            id.append(_variant);
        }

        _id = id.toString().intern();

        // Compose legacy JDK ID string if required
        if (scriptLen > 0) {
            StringBuilder buf = new StringBuilder(_language);
            if (regionLen > 0) {
                buf.append(SEPCHAR);
                buf.append(_region);
            }
            if (variantLen > 0) {
                buf.append(SEPCHAR);
                buf.append(_variant);
            }
            _java6id = buf.toString().intern();
        } else if (languageLen == 0 && regionLen == 0 && variantLen > 0) {
            _java6id = "";
        } else {
            _java6id = _id;
        }

        // Resolve parent
        if (variantLen > 0) {
            _parent = getInstance(_language, _script, _region, "");
        } else if (regionLen > 0) {
            _parent = getInstance(_language, _script, "", "");
        } else if (scriptLen > 0) {
            _parent = getInstance(_language, "", "", "");
        } else if (languageLen > 0) {
            _parent = ROOT;
        } else {
            // This is the root
            // We should never get here, because ROOT is pre-populated.
            _parent = null;
        }
        return this;
    }

    private BaseLocaleKey createKey() {
        return new BaseLocaleKey(_language, _script, _region, _variant);
    }

    private static class BaseLocaleKey {
        private String _lang;
        private String _scrt;
        private String _regn;
        private String _vart;
        private int _hash; // Default to 0

        public BaseLocaleKey(String language, String script, String region, String variant) {
            _lang = language;
            _scrt = script;
            _regn = region;
            _vart = variant;
        }

        public boolean equals(Object obj) {
            return (this == obj) ||
                    (obj instanceof BaseLocaleKey)
                    && AsciiUtil.caseIgnoreMatch(((BaseLocaleKey)obj)._lang, this._lang)
                    && AsciiUtil.caseIgnoreMatch(((BaseLocaleKey)obj)._scrt, this._scrt)
                    && AsciiUtil.caseIgnoreMatch(((BaseLocaleKey)obj)._regn, this._regn)
                    && ((BaseLocaleKey)obj)._vart.equals(_vart); // variant is case sensitive in JDK!
        }

        public int hashCode() {
            int h = _hash;
            if (h == 0) {
                // Generating a hash value from language, script, region and variant
                for (int i = 0; i < _lang.length(); i++) {
                    h = 31*h + AsciiUtil.toLower(_lang.charAt(i));
                }
                for (int i = 0; i < _scrt.length(); i++) {
                    h = 31*h + AsciiUtil.toLower(_scrt.charAt(i));
                }
                for (int i = 0; i < _regn.length(); i++) {
                    h = 31*h + AsciiUtil.toLower(_regn.charAt(i));
                }
                for (int i = 0; i < _vart.length(); i++) {
                    h = 31*h + AsciiUtil.toLower(_vart.charAt(i));
                }
                _hash = h;
            }
            return h;
        }
    }
}
