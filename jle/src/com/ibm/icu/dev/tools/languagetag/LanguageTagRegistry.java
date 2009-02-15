/*
 *******************************************************************************
 * Copyright (C) 2009, International Business Machines Corporation and         *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
package com.ibm.icu.dev.tools.languagetag;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.ListIterator;

public class LanguageTagRegistry {

    private static final String DELIM = "%%";
    private static final String COLON = ":";

    private static final String FLD_FILEDATE = "File-Date";

    private String fileDate;
    private List<Record> records = new LinkedList<Record>();

    public static LanguageTagRegistry getInstance() {
//        InputStream is = ClassLoader.getSystemResourceAsStream("com/ibm/icu/dev/tools/languagetag/language-subtag-registry.txt");
        InputStream is = ClassLoader.getSystemResourceAsStream("com/ibm/icu/dev/tools/languagetag/language-subtag-registry-rfc4645bis09.txt");
        if (is == null) {
            throw new RuntimeException("Missing language-subtag-registry.txt");
        }
        LanguageTagRegistry reg = null;
        try {
            Reader reader = new InputStreamReader(is, "ISO-8859-1");
            reg = getInstance(reader);
            reader.close();
        } catch (IOException e) {
            throw new RuntimeException("IO error while parsing language-subtag-registry.txt");
        }
        return reg;
    }

    public static LanguageTagRegistry getInstance(Reader reader) throws IOException {
        LanguageTagRegistry reg = new LanguageTagRegistry();
        BufferedReader r = new BufferedReader(reader);

        Record rec = new Record();
        String prevField = null;
        String[] fields = new String[2];
        while (true) {
            String line = r.readLine();
            if (line == null) {
                if (rec.getType() != Record.UNKNOWN) {
                    reg.records.add(rec);
                }
                break;
            }
            if (line.trim().equals(DELIM)) {
                if (rec.getType() != Record.UNKNOWN) {
                    reg.records.add(rec);
                }
                rec = new Record();
                prevField = null;
                continue;
            }
            if (prevField != null && (line.startsWith(" ") || line.startsWith("\t"))) {
                // contiguous line
                rec.appendFieldValue(prevField, line.trim());
                continue;
            }
            parseFields(line, fields);
            if (fields[0].equalsIgnoreCase(FLD_FILEDATE)) {
                reg.fileDate = fields[1];
            } else {
                rec.setField(fields[0], fields[1]);
                prevField = fields[0];
            }
        }

        return reg;
    }

    private LanguageTagRegistry() {
    }

    public String getFileDate() {
        return fileDate;
    }

    public ListIterator<Record> getIterator() {
        return records.listIterator();
    }

    private static void parseFields(String line, String[] fields) {
        int colIdx = line.indexOf(COLON);
        if (colIdx >= 0) {
            fields[0] = line.substring(0, colIdx).trim();
            fields[1] = line.substring(colIdx + 1).trim();
        } else {
            fields[0] = line.trim();
            fields[1] = "";
        }
    }

    public static class Record {
        public static final int UNKNOWN = 0;
        public static final int LANGUAGE = 1;
        public static final int EXTLANG = 2;
        public static final int SCRIPT = 3;
        public static final int REGION = 4;
        public static final int VARIANT = 5;
        public static final int GRANDFATHERED = 6;
        public static final int REDUNDANT = 7;

        private static final String[] TYPES = {
            "",
            "language",
            "extlang",
            "script",
            "region",
            "variant",
            "grandfathered",
            "redundant",
        };

        private static final String FLD_TYPE = "Type";
        private static final String FLD_SUBTAG = "Subtag";
        private static final String FLD_TAG = "Tag";
        private static final String FLD_DESCRIPTION = "Description";
        private static final String FLD_ADDED = "Added";
        private static final String FLD_PREFERREDVALUE = "Preferred-Value";
        private static final String FLD_DEPRECATED = "Deprecated";
        private static final String FLD_PREFIX = "Prefix";
        private static final String FLD_COMMENTS = "Comments";
        private static final String FLD_SUPPRESSSCRIPT = "Suppress-Script";
        private static final String FLD_MACROLANGUAGE = "Macrolanguage";
        private static final String FLD_SCOPE = "Scope";

        private int type;
        private String subtag;
        private String tag;
        private String description;
        private String added;
        private String preferredValue;
        private String deprecated;
        private ArrayList<String> prefix;
        private String comments;
        private String suppressScript;
        private String macroLanguage;
        private String scope;

        public int getType() {
            return type;
        }

        public String getSubtag() {
            return subtag;
        }

        public String getTag() {
            return tag;
        }

        public String getDescription() {
            return description;
        }

        public String getAdded() {
            return added;
        }

        public String getPreferredValue() {
            return preferredValue;
        }

        public String getDeprecated() {
            return deprecated;
        }

        public String getPrefix(int index) {
            return prefix.get(index);
        }

        public String getComments() {
            return comments;
        }

        public String getSuppressScript() {
            return suppressScript;
        }

        public String getScope() {
            return scope;
        }

        public String toString() {
            StringBuilder buf = new StringBuilder();
            buf.append("type=");
            buf.append(TYPES[type]);
            if (tag != null) {
                buf.append(";tag=");
                buf.append(tag);
            }
            if (subtag != null) {
                buf.append(";subtag=");
                buf.append(subtag);
            }
            if (description != null) {
                buf.append(";description=");
                buf.append(description);
            }
            if (added != null) {
                buf.append(";added=");
                buf.append(added);
            }
            if (preferredValue != null) {
                buf.append(";preferredValue=");
                buf.append(preferredValue);
            }
            if (deprecated != null) {
                buf.append(";deprecated=");
                buf.append(deprecated);
            }
            if (prefix != null) {
                buf.append(";prefix={");
                Iterator<String> itr = prefix.iterator();
                while (itr.hasNext()) {
                    buf.append(itr.next());
                    buf.append(",");
                }
                buf.append("}");
            }
            if (comments != null) {
                buf.append(";comments=");
                buf.append(comments);
            }
            if (suppressScript != null) {
                buf.append(";suppressScript=");
                buf.append(suppressScript);
            }
            if (macroLanguage != null) {
                buf.append(";macroLanguage=");
                buf.append(macroLanguage);
            }
            if (scope != null) {
                buf.append(";scope=");
                buf.append(scope);
            }
            return buf.toString();
        }

        private void setField(String field, String value) {
            if (field.equalsIgnoreCase(FLD_TYPE)) {
                for (int i = LANGUAGE; i < TYPES.length; i++) {
                    if (TYPES[i].equals(value)) {
                        type = i;
                        break;
                    }
                }
            } else if (field.equalsIgnoreCase(FLD_SUBTAG)) {
                subtag = value;
            } else if (field.equalsIgnoreCase(FLD_TAG)) {
                tag = value;
            } else if (field.equalsIgnoreCase(FLD_DESCRIPTION)) {
                description = value;
            } else if (field.equalsIgnoreCase(FLD_ADDED)) {
                added = value;
            } else if (field.equalsIgnoreCase(FLD_PREFERREDVALUE)) {
                preferredValue = value;
            } else if (field.equalsIgnoreCase(FLD_DEPRECATED)) {
                deprecated = value;
            } else if (field.equalsIgnoreCase(FLD_PREFIX)) {
                if (prefix == null) {
                    prefix = new ArrayList<String>();
                }
                prefix.add(value);
            } else if (field.equalsIgnoreCase(FLD_COMMENTS)) {
                comments = value;
            } else if (field.equalsIgnoreCase(FLD_SUPPRESSSCRIPT)) {
                suppressScript = value;
            } else if (field.equalsIgnoreCase(FLD_MACROLANGUAGE)) {
                macroLanguage = value;
            } else if (field.equalsIgnoreCase(FLD_SCOPE)) {
                scope = value;
            } else {
                throw new IllegalArgumentException("Unknown field: " + field);
            }
        }

        private void appendFieldValue(String field, String value) {
            if (field.equalsIgnoreCase(FLD_SUBTAG)) {
                subtag = append(field, subtag, value);
            } else if (field.equalsIgnoreCase(FLD_TAG)) {
                tag = append(field, tag, value);
            } else if (field.equalsIgnoreCase(FLD_DESCRIPTION)) {
                description = append(field, description, value);
            } else if (field.equalsIgnoreCase(FLD_ADDED)) {
                added = append(field, added, value);
            } else if (field.equalsIgnoreCase(FLD_PREFERREDVALUE)) {
                preferredValue = append(field, preferredValue, value);
            } else if (field.equalsIgnoreCase(FLD_DEPRECATED)) {
                deprecated = append(field, deprecated, value);
            } else if (field.equalsIgnoreCase(FLD_PREFIX)) {
                if (prefix == null) {
                    throw new IllegalStateException("No value is currently available in the field: prefix");
                }
                int lastIdx = prefix.size() - 1;
                String current = prefix.get(lastIdx);
                prefix.set(lastIdx, current + " " + value);
            } else if (field.equalsIgnoreCase(FLD_COMMENTS)) {
                comments = append(field, comments, value);
            } else if (field.equalsIgnoreCase(FLD_SUPPRESSSCRIPT)) {
                suppressScript = append(field, suppressScript, value);
            } else if (field.equalsIgnoreCase(FLD_MACROLANGUAGE)) {
                macroLanguage = append(field, macroLanguage, value);
            } else if (field.equalsIgnoreCase(FLD_SCOPE)) {
                scope = append(field, scope, value);
            } else {
                throw new IllegalArgumentException("Inappropriate field: " + field);
            }
        }

        private String append(String field, String current, String value) {
            if (current == null) {
                throw new IllegalStateException("No value is currently set in the field: " + field);
            }
            return current + " " + value;
        }
    }
}
