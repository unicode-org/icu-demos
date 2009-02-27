package com.ibm.icu.dev.sandbox;

import com.ibm.icu.util.Locale;

public class LanguageTagParseTest {
    public static void main(String[] args) {
        test1();
    }

    static void test1() {
        Locale l = Locale.forLanguageTag("en-us-Posix-JPXTA-x-icu");
        System.out.println(l.toLanguageTag());
    }
}
