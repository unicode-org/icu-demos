/*
 *******************************************************************************
 * Copyright (C) 2009, International Business Machines Corporation and         *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
package com.ibm.icu.impl.locale;

import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Set;
import java.util.TreeSet;

public final class LanguageTag {

    private String _languageTag = "";   // entire language tag
    private String _grandfathered = ""; // grandfathered tag
    private String _privateuse = "";    // privateuse, not including leading "x-"
    private String _language = "";      // language subtag
    private String[] _extlang;          // array of extlang subtags
    private String _script = "";        // script subtag
    private String _region = "";        // region subtag
    private TreeSet<String> _variants;  // variant subtags in a single string
    private TreeSet<Extension> _extensions; // extension key/value pairs

    private static final int MINLEN = 2; // minimum length of a valid language tag

    private static final String SEP = "-";
    private static final String PRIVATEUSE = "x";

    public static String UNDETERMINED = "und";

    // Map contains grandfathered tags and its preferred mappings from
    // http://www.ietf.org/internet-drafts/draft-ietf-ltru-4645bis-09.txt
    private static final HashMap<String,String> GRANDFATHERED = new HashMap<String,String>();

    static {
        final String[][] entries = {
          //{"tag",         "preferred"},
            {"art-lojban",  "jbo"},
            {"cel-gaulish", ""},
            {"en-GB-oed",   ""},
            {"i-ami",       "ami"},
            {"i-bnn",       "bnn"},
            {"i-default",   ""},
            {"i-enochian",  ""},
            {"i-hak",       "hak"},
            {"i-klingon",   "tlh"},
            {"i-lux",       "lb"},
            {"i-mingo",     ""},
            {"i-navajo",    "nv"},
            {"i-pwn",       "pwn"},
            {"i-tao",       "tao"},
            {"i-tay",       "tay"},
            {"i-tsu",       "tsu"},
            {"no-bok",      "nb"},
            {"no-nyn",      "nn"},
            {"sgn-BE-FR",   "sfb"},
            {"sgn-BE-NL",   "vgt"},
            {"sgn-CH-DE",   "sgg"},
            {"zh-guoyu",    "cmn"},
            {"zh-hakka",    "hak"},
            {"zh-min",      ""},
            {"zh-min-nan",  "nan"},
            {"zh-xiang",    "hsn"},
        };
        for (String[] e : entries) {
            GRANDFATHERED.put(e[0], e[1]);
        }
    }

    private static final String[][] DEPRECATEDLANGS = {
        // {<deprecated>, <current>},
        {"iw", "he"},
        {"ji", "yi"},
        {"in", "id"},
    };

    private static final HashMap<String,String> THREE_TO_TWO = new HashMap<String, String>();

    static {
        String[] alpha3to2 = {
            "aar", "aa",    // Afar
            "abk", "ab",    // Abkhazian
            "afr", "af",    // Afrikaans
            "aka", "ak",    // Akan
            "amh", "am",    // Amharic
            "ara", "ar",    // Arabic
            "arg", "an",    // Aragonese
            "asm", "as",    // Assamese
            "ava", "av",    // Avaric
            "ave", "ae",    // Avestan
            "aym", "ay",    // Aymara
            "aze", "az",    // Azerbaijani
            "bak", "ba",    // Bashkir
            "bam", "bm",    // Bambara
            "bel", "be",    // Belarusian
            "ben", "bn",    // Bengali
            "bih", "bh",    // Bihari
            "bis", "bi",    // Bislama
            "bod", "bo",    // Tibetan
            "bos", "bs",    // Bosnian
            "bre", "br",    // Breton
            "bul", "bg",    // Bulgarian
            "cat", "ca",    // Catalan; Valencian
            "ces", "cs",    // Czech
            "cha", "ch",    // Chamorro
            "che", "ce",    // Chechen
            "chu", "cu",    // Church Slavic; Old Slavonic; Church Slavonic; Old Bulgarian; Old Church Slavonic
            "chv", "cv",    // Chuvash
            "cor", "kw",    // Cornish
            "cos", "co",    // Corsican
            "cre", "cr",    // Cree
            "cym", "cy",    // Welsh
            "dan", "da",    // Danish
            "deu", "de",    // German
            "div", "dv",    // Divehi; Dhivehi; Maldivian
            "dzo", "dz",    // Dzongkha
            "ell", "el",    // Greek, Modern (1453-)
            "eng", "en",    // English
            "epo", "eo",    // Esperanto
            "est", "et",    // Estonian
            "eus", "eu",    // Basque
            "ewe", "ee",    // Ewe
            "fao", "fo",    // Faroese
            "fas", "fa",    // Persian
            "fij", "fj",    // Fijian
            "fin", "fi",    // Finnish
            "fra", "fr",    // French
            "fry", "fy",    // Western Frisian
            "ful", "ff",    // Fulah
            "gla", "gd",    // Gaelic; Scottish Gaelic
            "gle", "ga",    // Irish
            "glg", "gl",    // Galician
            "glv", "gv",    // Manx
            "grn", "gn",    // Guarani
            "guj", "gu",    // Gujarati
            "hat", "ht",    // Haitian; Haitian Creole
            "hau", "ha",    // Hausa
            "heb", "he",    // Hebrew
            "her", "hz",    // Herero
            "hin", "hi",    // Hindi
            "hmo", "ho",    // Hiri Motu
            "hrv", "hr",    // Croatian
            "hun", "hu",    // Hungarian
            "hye", "hy",    // Armenian
            "ibo", "ig",    // Igbo
            "ido", "io",    // Ido
            "iii", "ii",    // Sichuan Yi; Nuosu
            "iku", "iu",    // Inuktitut
            "ile", "ie",    // Interlingue; Occidental
            "ina", "ia",    // Interlingua (International Auxiliary Language Association)
            "ind", "id",    // Indonesian
            "ipk", "ik",    // Inupiaq
            "isl", "is",    // Icelandic
            "ita", "it",    // Italian
            "jav", "jv",    // Javanese
            "jpn", "ja",    // Japanese
            "kal", "kl",    // Kalaallisut; Greenlandic
            "kan", "kn",    // Kannada
            "kas", "ks",    // Kashmiri
            "kat", "ka",    // Georgian
            "kau", "kr",    // Kanuri
            "kaz", "kk",    // Kazakh
            "khm", "km",    // Central Khmer
            "kik", "ki",    // Kikuyu; Gikuyu
            "kin", "rw",    // Kinyarwanda
            "kir", "ky",    // Kirghiz; Kyrgyz
            "kom", "kv",    // Komi
            "kon", "kg",    // Kongo
            "kor", "ko",    // Korean
            "kua", "kj",    // Kuanyama; Kwanyama
            "kur", "ku",    // Kurdish
            "lao", "lo",    // Lao
            "lat", "la",    // Latin
            "lav", "lv",    // Latvian
            "lim", "li",    // Limburgan; Limburger; Limburgish
            "lin", "ln",    // Lingala
            "lit", "lt",    // Lithuanian
            "ltz", "lb",    // Luxembourgish; Letzeburgesch
            "lub", "lu",    // Luba-Katanga
            "lug", "lg",    // Ganda
            "mah", "mh",    // Marshallese
            "mal", "ml",    // Malayalam
            "mar", "mr",    // Marathi
            "mkd", "mk",    // Macedonian
            "mlg", "mg",    // Malagasy
            "mlt", "mt",    // Maltese
            "mon", "mn",    // Mongolian
            "mri", "mi",    // Maori
            "msa", "ms",    // Malay
            "mya", "my",    // Burmese
            "nau", "na",    // Nauru
            "nav", "nv",    // Navajo; Navaho
            "nbl", "nr",    // Ndebele, South; South Ndebele
            "nde", "nd",    // Ndebele, North; North Ndebele
            "ndo", "ng",    // Ndonga
            "nep", "ne",    // Nepali
            "nld", "nl",    // Dutch; Flemish
            "nno", "nn",    // Norwegian Nynorsk; Nynorsk, Norwegian
            "nob", "nb",    // Bokm&#x00E5;l, Norwegian; Norwegian Bokm&#x00E5;l
            "nor", "no",    // Norwegian
            "nya", "ny",    // Chichewa; Chewa; Nyanja
            "oci", "oc",    // Occitan (post 1500); Proven&#x00E7;al
            "oji", "oj",    // Ojibwa
            "ori", "or",    // Oriya
            "orm", "om",    // Oromo
            "oss", "os",    // Ossetian; Ossetic
            "pan", "pa",    // Panjabi; Punjabi
            "pli", "pi",    // Pali
            "pol", "pl",    // Polish
            "por", "pt",    // Portuguese
            "pus", "ps",    // Pushto; Pashto
            "que", "qu",    // Quechua
            "roh", "rm",    // Romansh
            "ron", "ro",    // Romanian; Moldavian; Moldovan
            "run", "rn",    // Rundi
            "rus", "ru",    // Russian
            "sag", "sg",    // Sango
            "san", "sa",    // Sanskrit
            "sin", "si",    // Sinhala; Sinhalese
            "slk", "sk",    // Slovak
            "slv", "sl",    // Slovenian
            "sme", "se",    // Northern Sami
            "smo", "sm",    // Samoan
            "sna", "sn",    // Shona
            "snd", "sd",    // Sindhi
            "som", "so",    // Somali
            "sot", "st",    // Sotho, Southern
            "spa", "es",    // Spanish; Castilian
            "sqi", "sq",    // Albanian
            "srd", "sc",    // Sardinian
            "srp", "sr",    // Serbian
            "ssw", "ss",    // Swati
            "sun", "su",    // Sundanese
            "swa", "sw",    // Swahili
            "swe", "sv",    // Swedish
            "tah", "ty",    // Tahitian
            "tam", "ta",    // Tamil
            "tat", "tt",    // Tatar
            "tel", "te",    // Telugu
            "tgk", "tg",    // Tajik
            "tgl", "tl",    // Tagalog
            "tha", "th",    // Thai
            "tir", "ti",    // Tigrinya
            "ton", "to",    // Tonga (Tonga Islands)
            "tsn", "tn",    // Tswana
            "tso", "ts",    // Tsonga
            "tuk", "tk",    // Turkmen
            "tur", "tr",    // Turkish
            "twi", "tw",    // Twi
            "uig", "ug",    // Uighur; Uyghur
            "ukr", "uk",    // Ukrainian
            "urd", "ur",    // Urdu
            "uzb", "uz",    // Uzbek
            "ven", "ve",    // Venda
            "vie", "vi",    // Vietnamese
            "vol", "vo",    // Volap&#x00FC;k
            "wln", "wa",    // Walloon
            "wol", "wo",    // Wolof
            "xho", "xh",    // Xhosa
            "yid", "yi",    // Yiddish
            "yor", "yo",    // Yoruba
            "zha", "za",    // Zhuang; Chuang
            "zho", "zh",    // Chinese
            "zul", "zu",    // Zulu
        };
        int i = 0;
        while (i < alpha3to2.length) {
            THREE_TO_TWO.put(alpha3to2[i], alpha3to2[i+1]);
            i += 2;
        }
    }


    private LanguageTag(String tag) {
        _languageTag = tag;
    }

    // Bit flags used by the language tag parser
    private static final int LANG = 0x0001;
    private static final int EXTL = 0x0002;
    private static final int SCRT = 0x0004;
    private static final int REGN = 0x0008;
    private static final int VART = 0x0010;
    private static final int EXTS = 0x0020;
    private static final int EXTV = 0x0040;
    private static final int PRIV = 0x0080;

    public static LanguageTag parse(String tag) throws InvalidLocaleIdentifierException {
        if (tag.length() < MINLEN) {
            throw new InvalidLocaleIdentifierException("The specified tag '"
                    + tag + "' is too short");
        }

        if (tag.endsWith(SEP)) {
            // This code utilizes String#split, which drops off the last empty segment.
            // We need to check if the tag ends with '-' here.
            throw new InvalidLocaleIdentifierException("The specified tag '"
                    + tag + "' ends with " + SEP);
        }

        LanguageTag t = new LanguageTag(tag);

        tag = AsciiUtil.toLowerString(tag);

        // Check if the tag is grandfathered
        if (GRANDFATHERED.containsKey(tag)) {
            t._grandfathered = tag;
            // Preferred mapping
            String preferred = GRANDFATHERED.get(tag);
            if (preferred.length() > 0) {
                t._language = preferred;
            }
            return t;
        }

        // langtag       = language
        //                 ["-" script]
        //                 ["-" region]
        //                 *("-" variant)
        //                 *("-" extension)
        //                 ["-" privateuse]

        String[] subtags = tag.split(SEP);
        int idx = 0;
        int extlangIdx = 0;
        String extSingleton = null;
        StringBuilder extBuf = null;
        int next = LANG | PRIV;
        while (true) {
            if (idx >= subtags.length) {
                break;
            }
            if ((next & LANG) != 0) {
                if (isLanguageSubtag(subtags[idx])) {
                    t._language = getShortLanguageCode(subtags[idx++]);
                    next = EXTL | SCRT | REGN | VART | EXTS | PRIV;
                    continue;
                }
            }
            if ((next & EXTL) != 0) {
                if (isExtlangSubtag(subtags[idx])) {
                    if (extlangIdx == 0) {
                        t._extlang = new String[3];
                    }
                    t._extlang[extlangIdx++] = subtags[idx++];
                    if (extlangIdx < 3) {
                        next = EXTL | SCRT | REGN | VART | EXTS | PRIV;
                    } else {
                        next = SCRT | REGN | VART | EXTS | PRIV;
                    }
                    continue;
                }
            }
            if ((next & SCRT) != 0) {
                if (isScriptSubtag(subtags[idx])) {
                    t._script = subtags[idx++];
                    next = REGN | VART | EXTS | PRIV;
                    continue;
                }
            }
            if ((next & REGN) != 0) {
                if (isRegionSubtag(subtags[idx])) {
                    t._region = subtags[idx++];
                    next = VART | EXTS | PRIV;
                    continue;
                }
            }
            if ((next & VART) != 0) {
                if (isVariantSubtag(subtags[idx])) {
                    if (t._variants == null) {
                        t._variants = new TreeSet<String>();
                    }
                    t._variants.add(subtags[idx++]);
                    next = VART | EXTS | PRIV;
                    continue;
                }
            }
            if ((next & EXTS) != 0) {
                if (isExtensionSingleton(subtags[idx])) {
                    if (extSingleton != null) {
                        if (extBuf == null) {
                            throw new InvalidLocaleIdentifierException("The specified tag '"
                                    + tag + "' contains an incomplete extension: "
                                    + extSingleton);
                        }
                        // Emit the previous extension key/value pair
                        if (t._extensions == null) {
                            t._extensions = new TreeSet<Extension>();
                        }
                        Extension e = new Extension(extSingleton.charAt(0), extBuf.toString());
                        t._extensions.add(e);
                    }
                    extSingleton = subtags[idx++];
                    extBuf = null; // Clear the extension value buffer
                    next = EXTV;
                    continue;
                }
            }
            if ((next & EXTV) != 0) {
                if (isExtensionSubtag(subtags[idx])) {
                    if (extBuf == null) {
                        extBuf = new StringBuilder(subtags[idx++]);
                    } else {
                        extBuf.append(SEP);
                        extBuf.append(subtags[idx++]);
                    }
                    next = EXTS | EXTV | PRIV;
                    continue;
                }
            }
            if ((next & PRIV) != 0) {
                if (AsciiUtil.caseIgnoreMatch(PRIVATEUSE, subtags[idx])) {
                    // The rest of part will be private use value subtags
                    StringBuilder puBuf = new StringBuilder();
                    idx++;
                    for (boolean bFirst = true ; idx < subtags.length; idx++) {
                        if (!isPrivateuseValueSubtag(subtags[idx])) {
                            throw new InvalidLocaleIdentifierException("The specified tag '"
                                    + tag + "' contains an illegal private use subtag: "
                                    + (subtags[idx].length() == 0 ? "<empty>" : subtags[idx]));
                        }
                        if (bFirst) {
                            bFirst = false;
                        } else {
                            puBuf.append(SEP);
                        }
                        puBuf.append(subtags[idx]);
                    }
                    t._privateuse = puBuf.toString();
                    if (t._privateuse.length() == 0) {
                        // Empty privateuse value
                        throw new InvalidLocaleIdentifierException("The specified tag '"
                                + tag + "' contains an empty private use subtag");
                    }
                    break;
                }
            }
            // If we fell through here, it means this subtag is illegal
            throw new InvalidLocaleIdentifierException("The specified tag '" + tag
                    + "' contains an illegal subtag: "
                    + (subtags[idx].length() == 0 ? "<empty>" : subtags[idx]));
        }

        if (extSingleton != null) {
            if (extBuf == null) {
                // extension singleton without following extension value
                throw new InvalidLocaleIdentifierException("The specified tag '"
                        + tag + "' contains an incomplete extension: "
                        + extSingleton);
            }
            // Emit the last extension key/value pair
            if (t._extensions == null) {
                t._extensions = new TreeSet<Extension>();
            }
            Extension e = new Extension(extSingleton.charAt(0), extBuf.toString());
            t._extensions.add(e);
        }

        return t;
    }

    public String getTag() {
        return _languageTag;
    }

    public String getLanguage() {
        return _language;
    }

    public String getJDKLanguage() {
        String lang = _language;
        for (String[] langMap : DEPRECATEDLANGS) {
            if (AsciiUtil.caseIgnoreCompare(lang, langMap[1]) == 0) {
                // use the old code
                lang = langMap[0];
                break;
            }
        }
        return lang;
    }

    public String getExtlang(int idx) {
        if (_extlang != null && idx < _extlang.length) {
            return _extlang[idx];
        }
        return null;
    }

    public String getScript() {
        return _script;
    }

    public String getRegion() {
        return _region;
    }

    public String getVariant() {
        if (_variants != null) {
            StringBuilder buf = new StringBuilder();
            Iterator<String> itr = _variants.iterator();
            while (itr.hasNext()) {
                if (buf.length() > 0) {
                    buf.append(SEP);
                }
                buf.append(itr.next());
            }
            return buf.toString();
        }
        return "";
    }

    public Set<String> getVarinats() {
        return Collections.unmodifiableSet(_variants);
    }

    public Set<Extension> getExtensions() {
        if (_extensions != null) {
            return Collections.unmodifiableSet(_extensions);
        }
        return null;
    }

    public String getPrivateUse() {
        return _privateuse;
    }

    public String getGrandfathered() {
        return _grandfathered;
    }

    public static boolean isLanguageSubtag(String s) {
        // language      = 2*3ALPHA            ; shortest ISO 639 code
        //                 ["-" extlang]       ; sometimes followed by
        //                                     ;   extended language subtags
        //               / 4ALPHA              ; or reserved for future use
        //               / 5*8ALPHA            ; or registered language subtag
        return (s.length() >= 2) && (s.length() <= 8) && AsciiUtil.isAlphaString(s);
    }

    public static boolean isExtlangSubtag(String s) {
        // extlang       = 3ALPHA              ; selected ISO 639 codes
        //                 *2("-" 3ALPHA)      ; permanently reserved
        return (s.length() == 3) && AsciiUtil.isAlphaString(s);
    }

    public static boolean isScriptSubtag(String s) {
        // script        = 4ALPHA              ; ISO 15924 code
        return (s.length() == 4) && AsciiUtil.isAlphaString(s);
    }

    public static boolean isRegionSubtag(String s) {
        // region        = 2ALPHA              ; ISO 3166-1 code
        //               / 3DIGIT              ; UN M.49 code
        return ((s.length() == 2) && AsciiUtil.isAlphaString(s))
                || ((s.length() == 3) && AsciiUtil.isNumericString(s));
    }

    public static boolean isVariantSubtag(String s) {
        // variant       = 5*8alphanum         ; registered variants
        //               / (DIGIT 3alphanum)
        int len = s.length();
        if (len >= 5 && len <= 8) {
            return AsciiUtil.isAlphaNumericString(s);
        }
        if (len == 4) {
            return AsciiUtil.isNumeric(s.charAt(0))
                    && AsciiUtil.isAlphaNumeric(s.charAt(1))
                    && AsciiUtil.isAlphaNumeric(s.charAt(2))
                    && AsciiUtil.isAlphaNumeric(s.charAt(3));
        }
        return false;
    }

    public static boolean isExtensionSingleton(String s) {
        // extension     = singleton 1*("-" (2*8alphanum))
        return (s.length() == 1)
                && AsciiUtil.isAlphaString(s)
                && !AsciiUtil.caseIgnoreMatch(PRIVATEUSE, s);
    }

    public static boolean isExtensionSubtag(String s) {
        // extension     = singleton 1*("-" (2*8alphanum))
        return (s.length() >= 2) && (s.length() <= 8) && AsciiUtil.isAlphaNumericString(s);
    }

    public static boolean isPrivateuseValueSubtag(String s) {
        // privateuse    = "x" 1*("-" (1*8alphanum))
        return (s.length() >= 1) && (s.length() <= 8) && AsciiUtil.isAlphaNumericString(s);
    }

    public static String getShortLanguageCode(String code) {
        if (code.length() == 3) {
            String code3 = AsciiUtil.toLowerString(code);
            String code2 = THREE_TO_TWO.get(code3);
            if (code2 != null)
                return code2;
        }
        return code;
    }

    /*
     * Language tag extension key/value container
     */
    public static class Extension implements Comparable<Extension> {
        private char _singleton;
        private String _value;

        public Extension(char singleton, String value) {
            _singleton = AsciiUtil.toLower(singleton);
            _value = value;
        }

        public char getSingleton() {
            return _singleton;
        }

        public String getValue() {
            return _value;
        }

        public int compareTo(Extension other) {
            return (int)_singleton - (int)other._singleton;
        }
    }

    public static String toLanguageTag(BaseLocale base, LocaleExtensions ext) {
        StringBuilder buf = new StringBuilder();

        // language
        String language = base.getLanguage();
        if (language.length() == 0) {
            buf.append(UNDETERMINED);
        } else {
            if (isLanguageSubtag(language)) {
                // if deprecated language code, map to the current one
                for (String[] langMap : DEPRECATEDLANGS) {
                    if (AsciiUtil.caseIgnoreCompare(language, langMap[0]) == 0) {
                        language = langMap[1];
                        break;
                    }
                }
                buf.append(getShortLanguageCode(language));
            } else {
                buf.append(UNDETERMINED);
            }
        }

        // script
        String script = base.getScript();
        if (script.length() > 0 && isScriptSubtag(script)) {
            buf.append(SEP);
            buf.append(AsciiUtil.toLowerString(script));
        }

        // region
        String region = base.getRegion();
        if (region.length() > 0 && isRegionSubtag(region)) {
            buf.append(SEP);
            buf.append(AsciiUtil.toLowerString(region));
        }

        // variant
        String variant = base.getVariant();
        if (variant.length() > 0) {
            String[] variants = variant.split("_");
            for (String var : variants) {
                if (isVariantSubtag(var)) {
                    buf.append(SEP);
                    buf.append(AsciiUtil.toLowerString(var));
                }
            }
        }

        if (ext != null) {
            String exttags = ext.getCanonicalString();
            if (exttags.length() > 0) {
                // extensions including private use
                buf.append(SEP);
                buf.append(ext.getCanonicalString());
            }
        }
        return buf.toString();
    }
}
