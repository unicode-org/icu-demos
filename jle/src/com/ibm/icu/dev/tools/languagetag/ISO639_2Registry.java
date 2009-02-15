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
import java.util.Set;
import java.util.TreeMap;

public class ISO639_2Registry {

    private TreeMap<String,Language> _map = new TreeMap<String,Language>();

    private static final String SEP = "\\|";

    private ISO639_2Registry() {
    }

    public static ISO639_2Registry getInstance() {
        InputStream is = ClassLoader.getSystemResourceAsStream("com/ibm/icu/dev/tools/languagetag/ISO-639-2_utf-8.txt");
        if (is == null) {
            throw new RuntimeException("Missing language-subtag-registry.txt");
        }
        ISO639_2Registry reg = null;
        try {
            Reader reader = new InputStreamReader(is, "UTF-8");
            reg = getInstance(reader);
            reader.close();
        } catch (IOException e) {
            throw new RuntimeException("IO error while parsing language-subtag-registry.txt");
        }
        return reg;
    }

    public static ISO639_2Registry getInstance(Reader reader) throws IOException {
        ISO639_2Registry reg = new ISO639_2Registry();
        BufferedReader r = new BufferedReader(reader);

        while (true) {
            String line = r.readLine();
            if (line == null) {
                break;
            }
            String[] fields = line.split(SEP);
            String code3b = fields[0].trim();
            if (code3b.length() != 3) {
                // "qaa-qtz|||Reserved for local use|..."
                continue;
            }

            String code3t = fields[1].trim();
            if (code3t.length() == 0) {
                code3t = code3b;
            }
            String code2 = fields[2].trim();
            String nameEN = fields[3].trim();
            String nameFR = fields[4].trim();

            Language entry = new Language(code3b, code3t, code2, nameEN, nameFR);

            // use alpha 3 terminologic code as a key
            reg._map.put(code3t, entry);
        }
        return reg;
    }

    public Set<String> allAlpha3Terminologic() {
        return _map.keySet();
    }

    public Language get(String alpha3terminologic) {
        return _map.get(alpha3terminologic);
    }

    public static class Language {
        private String _code3b;  // bibliographic
        private String _code3t; // terminologic if different from bibliographic
        private String _code2;
        private String _nameEN;
        private String _nameFR;

        public Language(String code3b, String code3t, String code2, String nameEN, String nameFR) {
            _code3b = code3b;
            _code3t = code3t;
            _code2 = code2;
            _nameEN = nameEN;
            _nameFR = nameFR;
        }

        public String getAlpha3TerminologicCode() {
            return _code3t;
        }

        public String getAlpha3BibliographicCode() {
            return _code3b;
        }

        public String getAlpha2Code() {
            return _code2;
        }

        public String getEnglishName() {
            return _nameEN;
        }

        public String getFrenchName() {
            return _nameFR;
        }
    }
}
