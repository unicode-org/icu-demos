package com.ibm.icu.dev.sandbox;

import java.util.Locale;

public class LanguageTagTest {
    public static void main(String[] args) {
        test1();
    }

    static Locale[] TESTLOCALES = {
        new Locale("en", "This is not a region", "test"),
        new Locale("en", "US"),
        new Locale("ja"),
        new Locale("", "US"),
        new Locale("", "", "1984"),
        new Locale("123"),
        new Locale("en", "023"),
        new Locale("en", "This is not a region", "test"),
        new Locale("en-ab", "US"),
    };

    static void test1() {
        for (Locale l : TESTLOCALES) {
            String tag = l.toLanguageTag();
            System.out.println(l.toString() + " -> " + tag);
        }
    }
}
