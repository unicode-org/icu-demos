package com.ibm.icu.dev.tools.languagetag;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

public class LocodeRegistry {

    private List<Locode> _codelist;
    private Map<String, Locode> _map;

    public static LocodeRegistry getInstance() {
        InputStream is = ClassLoader.getSystemResourceAsStream("com/ibm/icu/dev/tools/languagetag/2007_UNLOCODE_CodeList.txt");
        if (is == null) {
            throw new RuntimeException("Missing 2007_UNLOCODE_CodeList.txt");
        }
        LocodeRegistry reg = null;
        try {
            Reader reader = new InputStreamReader(is, "ISO-8859-1");
            reg = getInstance(reader);
            reader.close();
        } catch (IOException e) {
            throw new RuntimeException("IO error while parsing 2007_UNLOCODE_CodeList.txt");
        }
        return reg;
    }

    public static LocodeRegistry getInstance(Reader reader) throws IOException {
        LocodeRegistry reg = new LocodeRegistry();
        BufferedReader r = new BufferedReader(reader);

        reg._codelist = new LinkedList<Locode>();
        reg._map = new HashMap<String, Locode>();

        while (true) {
            String line = r.readLine();
            if (line == null) {
                break;
            }
            Locode locode = new Locode(line);
            reg._codelist.add(locode);
            reg._map.put(locode.getLOCODE().replaceAll(" ", ""), locode);
        }
        return reg;
    }

    public Locode getByCode(String code) {
        code.replaceAll(" ", "");
        return _map.get(code);
    }

    public static class Locode {
        private String _change;
        private String _locode;
        private String _name;
        private String _nameWoDiacritics;
        private String _subdivision;
        private String _function;
        private String _status;
        private String _date;
        private String _iata;
        private String _coordinates;
        private String _remarks;

        private static final int[] COL_CHANGE       = {0, 2};
        private static final int[] COL_LOCODE       = {3, 9};
        private static final int[] COL_NAME         = {10, 45};
        private static final int[] COL_NAMEWODIACRITICS = {46, 81};
        private static final int[] COL_SUBDIVISION  = {82, 85};
        private static final int[] COL_FUNCTION     = {86, 94};
        private static final int[] COL_STATUS       = {95, 97};
        private static final int[] COL_DATE         = {98, 102};
        private static final int[] COL_IATA         = {103, 107};
        private static final int[] COL_COORDINATES  = {108, 120};
        private static final int[] COL_REMARKS       = {121, 171};

        private Locode(String line) {
if (line.length() < 170) {
    System.out.println(line);
}
            _change = line.substring(COL_CHANGE[0], COL_CHANGE[1]).trim();
            _locode = line.substring(COL_LOCODE[0], COL_LOCODE[1]).trim();
            _name = line.substring(COL_NAME[0], COL_NAME[1]).trim();
            _nameWoDiacritics= line.substring(COL_NAMEWODIACRITICS[0], COL_NAMEWODIACRITICS[1]).trim();
            _subdivision = line.substring(COL_SUBDIVISION[0], COL_SUBDIVISION[1]).trim();
            _function = line.substring(COL_FUNCTION[0], COL_FUNCTION[1]).trim();
            _status = line.substring(COL_STATUS[0], COL_STATUS[1]).trim();
            _date = line.substring(COL_DATE[0], COL_DATE[1]).trim();
            _iata = line.substring(COL_IATA[0], COL_IATA[1]).trim();
            _coordinates = line.substring(COL_COORDINATES[0], COL_COORDINATES[1]).trim();
            _remarks = line.substring(COL_REMARKS[0], COL_REMARKS[1]).trim();
        }

        public String getChangeIndicator() {
            return _change;
        }

        public String getLOCODE() {
            return _locode;
        }

        public String getName() {
            return _name;
        }

        public String getNameWithoutDiacritics() {
            return _nameWoDiacritics;
        }

        public String getSubdivision() {
            return _subdivision;
        }

        public String getFunction() {
            return _function;
        }

        public String getStatus() {
            return _status;
        }

        public String getDate() {
            return _date;
        }

        public String getIATA() {
            return _iata;
        }

        public String getCoordinates() {
            return _coordinates;
        }

        public String getRemark() {
            return _remarks;
        }
    }
}
