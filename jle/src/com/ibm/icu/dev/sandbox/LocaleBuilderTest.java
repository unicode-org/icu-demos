package com.ibm.icu.dev.sandbox;

import com.ibm.icu.impl.locale.BaseLocale;
import com.ibm.icu.impl.locale.InvalidLocaleException;
import com.ibm.icu.impl.locale.LocaleBuilder;
import com.ibm.icu.impl.locale.LocaleExtension;

public class LocaleBuilderTest {

    public static void main(String[] args) {
        test1();
    }

    static void test1() {
        try {
            LocaleBuilder bld1 = new LocaleBuilder();
            LocaleBuilder bld2 = new LocaleBuilder();

            bld1.setLanguage("JA");
            bld1.setRegion("jp");
            bld1.setLocaleKeyword("ca", "japanese");

            bld2.setLocaleKeyword("CA", "Japanese");
            bld2.setRegion("Jp");
            bld2.setLanguage("Ja");

            BaseLocale loc1 = bld1.getBaseLocale();
            BaseLocale loc2 = bld2.getBaseLocale();

            LocaleExtension ext1 = bld1.getLocaleExtension();
            LocaleExtension ext2 = bld2.getLocaleExtension();

            System.out.println("loc1/ext1: " + loc1 + "/" + ext1);
            System.out.println("loc2/ext2: " + loc2 + "/" + ext2);

            System.out.println("loc1 == loc2 -> " + (loc1 == loc2));
            System.out.println("ext1 == ext2 -> " + (ext1 == ext2));

        } catch (InvalidLocaleException e) {
            e.printStackTrace();
        }
    }
}
