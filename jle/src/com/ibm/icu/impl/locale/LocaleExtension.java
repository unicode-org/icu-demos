package com.ibm.icu.impl.locale;

import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.TreeMap;

public class LocaleExtension {
    private String _extension;
    private transient Map<String,String> _keywords;

    private static final LocaleObjectPool<String,LocaleExtension> EXTENSIONPOOL =
        new LocaleObjectPool<String,LocaleExtension>();

    public static final LocaleExtension EMPTY_EXTENSION = new LocaleExtension("");

    private static final String KEYWORDSEP = ";";
    private static final String VALUESEP = "=";
    private static final String PRIVATEUSE = "x";

    private LocaleExtension(String extension) {
        _extension = extension;
    }

    public static LocaleExtension getInstance(String extension) {
        if (extension == null || extension.length() == 0) {
            return EMPTY_EXTENSION;
        }
        // Convert to lower case
        extension = AsciiUtil.toLowerString(extension);
        LocaleExtension singleton = EXTENSIONPOOL.get(extension);
        if (singleton == null) {
            LocaleExtension locExt = getCanonical(extension);
            // Try to get from the pool with the canonicalized string
            singleton = EXTENSIONPOOL.get(locExt.toString());
            if (singleton == null) {
                singleton = locExt;
                EXTENSIONPOOL.put(locExt.toString(), locExt);
            }
        }
        return singleton;
    }

    public Iterator<String> getKeys() {
        if (_keywords != null) {
            return _keywords.keySet().iterator();
        }
        return null;
    }

    public String getValue(String key) {
        if (_keywords != null) {
            return _keywords.get(key);
        }
        return null;
    }

    public boolean containsKey(String key) {
        if (_keywords != null) {
            return _keywords.containsKey(key);
        }
        return false;
    }

    public String getExtensionString() {
        return _extension;
    }

    public String toString() {
        return _extension;
    }

    public int hashCode() {
        return _extension.hashCode();
    }

    private static LocaleExtension getCanonical(String extension) {
        // Convert to lower case
        extension = AsciiUtil.toLowerString(extension);

        // Breakdown to key/value pairs
        Map<String,String> tmpMap = new TreeMap<String,String>();
        String[] kvpairs = extension.split(KEYWORDSEP);
        for (String kv : kvpairs) {
            String[] tokens = kv.split(VALUESEP);
            if (tokens.length != 2) {
                throw new IllegalArgumentException("Invalid keyword: " + kv);
            }
            String prevVal = tmpMap.put(tokens[0], tokens[1]);
            if (prevVal != null) {
                throw new IllegalArgumentException("Multiple values for key [" + tokens[0] + "]: "
                        + prevVal + "/" + tokens[1]);
            }
        }

        StringBuilder buf = new StringBuilder();
        LinkedHashMap<String,String> keywords = new LinkedHashMap<String,String>();

        boolean hasSingletonExt = false;
        String privateUseValue = null;

        Iterator<Map.Entry<String,String>> itr = tmpMap.entrySet().iterator();
        // Collect LDML keywords first
        while (itr.hasNext()) {
            Map.Entry<String,String> entry = itr.next();
            String key = entry.getKey();
            if (key.length() > 1) { // LDML key length is always > 1
                keywords.put(key.intern(), entry.getValue().intern());
                if (buf.length() != 0) {
                    buf.append(KEYWORDSEP);
                }
                buf.append(key);
                buf.append(VALUESEP);
                buf.append(entry.getValue());
            } else if (key.equals(PRIVATEUSE)){
                privateUseValue = entry.getValue().intern();
            } else {
                hasSingletonExt = true;
            }
        }

        if (hasSingletonExt) {
            // Singleton extensions
            itr = tmpMap.entrySet().iterator();
            while (itr.hasNext()) {
                Map.Entry<String,String> entry = itr.next();
                String key = entry.getKey();
                if (key.length() == 1 && !key.equals(PRIVATEUSE)) {
                    keywords.put(key.intern(), entry.getValue().intern());
                    if (buf.length() != 0) {
                        buf.append(KEYWORDSEP);
                    }
                    buf.append(key);
                    buf.append(VALUESEP);
                    buf.append(entry.getValue());
                }
            }
        }

        if (privateUseValue != null) {
            // Finally, private use
            keywords.put(PRIVATEUSE, privateUseValue);
            if (buf.length() != 0) {
                buf.append(KEYWORDSEP);
            }
            buf.append(PRIVATEUSE);
            buf.append(VALUESEP);
            buf.append(privateUseValue);
        }

        LocaleExtension locExt = new LocaleExtension(buf.toString().intern());
        locExt._keywords = keywords;

        return locExt;
    }
}
