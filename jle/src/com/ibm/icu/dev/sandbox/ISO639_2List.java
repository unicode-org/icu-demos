package com.ibm.icu.dev.sandbox;

import java.util.Set;

import com.ibm.icu.dev.tools.languagetag.ISO639_2Registry;
import com.ibm.icu.dev.tools.languagetag.ISO639_2Registry.Language;

public class ISO639_2List {
    public static void main(String[] args) {
        ISO639_2Registry reg = ISO639_2Registry.getInstance();
        printAlpha3to2Mappings(reg);
    }

    static void printAlpha3to2Mappings(ISO639_2Registry reg) {
        Set<String> keys = reg.allAlpha3Terminologic();
        for (String key : keys) {
            Language lang = reg.get(key);
            if (lang.getAlpha2Code().length() == 0) {
                continue;
            }

            StringBuilder buf = new StringBuilder("\"");
            buf.append(lang.getAlpha3TerminologicCode());
            buf.append("\", \"");
            buf.append(lang.getAlpha2Code());
            buf.append("\",");
            while (buf.length() < 16) {
                buf.append(" ");
            }
            buf.append("// ");
            buf.append(lang.getEnglishName());

            System.out.println(buf.toString());
        }
    }
}
