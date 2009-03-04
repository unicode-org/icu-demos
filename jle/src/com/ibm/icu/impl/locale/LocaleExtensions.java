/*
 *******************************************************************************
 * Copyright (C) 2009, International Business Machines Corporation and         *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
package com.ibm.icu.impl.locale;

import java.util.Collections;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;

public final class LocaleExtensions {
    public static final LocaleExtensions EMPTY_EXTENSIONS = new LocaleExtensions("");

    private static final String LOCALEEXTSEP = "_";
    private static final String LDMLSINGLETON = "u";
    private static final String PRIVUSE = "x";
    private static final String PRIVUSEBOUND = "_x_";
    private static final int MINLEN = 3; // minimum length of string representation "x_?"

    private String _extensions;
    private TreeMap<Character, String> _extMap;
    private String _privuse = "";
    private TreeMap<String, String> _kwdMap;

    private LocaleExtensions(String extensions) {
        _extensions = extensions == null ? "" : extensions;
    }

    public static LocaleExtensions getInstance(String extensions) {
        if (extensions == null || extensions.length() == 0) {
            return EMPTY_EXTENSIONS;
        }
        // convert to lower case and normalize separators to "_"
        extensions = extensions.toLowerCase().replaceAll("-", LOCALEEXTSEP);

        if (extensions.length() < MINLEN) {
            // malformed extensions - too short
            return new LocaleExtensions(extensions);
        }

        String privuse = null;
        String exttags = null;

        if (extensions.charAt(0) == PRIVUSE.charAt(0)) {
            if (extensions.charAt(1) == LOCALEEXTSEP.charAt(0)) {
                privuse = extensions.substring(2);
            } else {
                // malformed extensions, "x" followed by non separator
                return new LocaleExtensions(extensions);
            }
        } else {
            int idx = extensions.indexOf(PRIVUSEBOUND);
            if (idx == -1) {
                exttags = extensions;
            } else {
                exttags = extensions.substring(0, idx);
                privuse = extensions.substring(idx + 3);
            }
        }

        TreeMap<Character, String> extMap = null;
        TreeMap<String, String> kwdMap = null;
        boolean bParseFailure = false;

        // parse the extension subtags
        if (exttags != null) {
            String[] subtags = exttags.split(LOCALEEXTSEP);
            String letter = null;
            extMap = new TreeMap<Character, String>();
            StringBuilder buf = new StringBuilder();
            boolean inLocaleKeywords = false;
            String kwkey = null;

            for (int i = 0; i < subtags.length; i++) {
                if (subtags[i].length() == 0) {
                    // empty subtag
                    bParseFailure = true;
                    break;
                }
                if (subtags[i].length() == 1) {
                    if (letter != null) {
                        // next extension singleton
                        if (extMap.containsKey(subtags[i])) {
                            // duplicated singleton extension letter
                            bParseFailure = true;
                            break;
                        }

                        // write out the previous extension
                        if (inLocaleKeywords) {
                            if (kwkey != null) {
                                // no locale keyword key
                                bParseFailure = true;
                                break;
                            }
                            // creating a single string including locale keyword key/type pairs
                            mapToLocaleExtensionString(kwdMap, buf);
                            inLocaleKeywords = false;
                        }
                        if (buf.length() == 0) {
                            // empty subtag
                            bParseFailure = true;
                            break;
                        }
                        extMap.put(Character.valueOf(letter.charAt(0)), buf.toString().intern());

                        // preparation for next extension
                        if (subtags[i].equals(LDMLSINGLETON)) {
                            kwdMap = new TreeMap<String, String>();
                            inLocaleKeywords = true;
                        }
                        buf.setLength(0);
                    }
                    letter = subtags[i];
                    continue;
                }
                if (inLocaleKeywords) {
                    if (kwkey == null) {
                        kwkey = subtags[i];
                    } else {
                        kwdMap.put(kwkey.intern(), subtags[i].intern());
                        kwkey = null;
                    }
                } else {
                    // append an extension subtag
                    if (buf.length() > 0) {
                        buf.append(LOCALEEXTSEP);
                    }
                    buf.append(subtags[i]);
                }
            }
            if (!bParseFailure) {
                // process the last extension
                if (inLocaleKeywords) {
                    if (kwkey != null) {
                        bParseFailure = true;
                    } else {
                        // creating a single string including locale keyword key/type pairs
                        mapToLocaleExtensionString(kwdMap, buf);
                    }
                }
                if (buf.length() == 0) {
                    // empty subtag at the end
                    bParseFailure = true;
                } else {
                    extMap.put(Character.valueOf(letter.charAt(0)), buf.toString().intern());
                }
            }
        }

        if (bParseFailure) {
            // parsing the extension string failed.
            // do not set any partial results in the result.
            return new LocaleExtensions(extensions);
        }

        // Reconstruct a locale extension string
        StringBuilder canonicalbuf = new StringBuilder();
        if (extMap != null) {
            Set<Map.Entry<Character, String>> entries = extMap.entrySet();
            for (Map.Entry<Character, String> entry : entries) {
                if (canonicalbuf.length() > 0) {
                    canonicalbuf.append(LOCALEEXTSEP);
                }
                canonicalbuf.append(entry.getKey());
                canonicalbuf.append(LOCALEEXTSEP);
                canonicalbuf.append(entry.getValue());
            }
        }
        if (privuse != null) {
            if (canonicalbuf.length() > 0) {
                canonicalbuf.append(LOCALEEXTSEP);
            }
            canonicalbuf.append(PRIVUSE);
            canonicalbuf.append(LOCALEEXTSEP);
            canonicalbuf.append(privuse);
        }

        LocaleExtensions le = new LocaleExtensions(canonicalbuf.toString().intern());
        le._extMap = extMap;
        le._kwdMap = kwdMap;
        if (privuse != null) {
            le._privuse = privuse.intern();
        }

        return le;
    }

    // This method assumes extension map, locale keyword map and private use
    // are all in canonicalized format.  This method is only used by
    // InternalLocaleBuilder.
    static LocaleExtensions getInstance(TreeMap<Character, String> extMap, TreeMap<String ,String> kwdMap, String privuse) {
        if (extMap == null && privuse == null) {
            return EMPTY_EXTENSIONS;
        }
        StringBuilder canonicalbuf = new StringBuilder();
        if (extMap != null) {
            Set<Map.Entry<Character, String>> entries = extMap.entrySet();
            for (Map.Entry<Character, String> entry : entries) {
                canonicalbuf.append(entry.getKey());
                canonicalbuf.append(LOCALEEXTSEP);
                canonicalbuf.append(entry.getValue());
            }
        }
        if (privuse != null) {
            if (canonicalbuf.length() > 0) {
                canonicalbuf.append(LOCALEEXTSEP);
            }
            canonicalbuf.append(privuse);
        }
        LocaleExtensions le = new LocaleExtensions(canonicalbuf.toString().intern());
        le._extMap = extMap;
        le._kwdMap = kwdMap;
        if (privuse != null) {
            le._privuse = privuse.intern();
        }

        return le;
    }

    public boolean equals(Object obj) {
        return (this == obj) ||
            ((obj instanceof LocaleExtensions) && _extensions == (((LocaleExtensions)obj)._extensions));
    }

    public int hashCode() {
        return _extensions.hashCode();
    }

    public Set<Character> getExtensionKeys() {
        if (_extMap != null) {
            return Collections.unmodifiableSet(_extMap.keySet());
        }
        return null;
    }

    public String getExtensionValue(char key) {
        if (_extMap != null) {
            return _extMap.get(Character.valueOf(key));
        }
        return null;
    }

    public Set<String> getLocaleKeywordKeys() {
        if (_kwdMap != null) {
            return Collections.unmodifiableSet(_kwdMap.keySet());
        }
        return null;
    }

    public String getLocaleKeywordType(String key) {
        if (_kwdMap != null) {
            return _kwdMap.get(key);
        }
        return null;
    }

    public String getPrivateUse() {
        return _privuse;
    }

    public String getID() {
        return _extensions;
    }

    public String toString() {
        return _extensions;
    }

    static void mapToLocaleExtensionString(TreeMap<String, String> map, StringBuilder buf) {
        Set<Map.Entry<String, String>> entries = map.entrySet();
        for (Map.Entry<String, String> entry : entries) {
            if (buf.length() > 0) {
                buf.append(LOCALEEXTSEP);
            }
            buf.append(entry.getKey());
            buf.append(LOCALEEXTSEP);
            buf.append(entry.getValue());
        }
    }

}
