package com.ibm.icu.dev.sandbox;

import java.util.Locale;
import java.util.Locale.LocaleBuilder;

//import com.ibm.icu.util.Locale;
//import com.ibm.icu.util.Locale.LocaleBuilder;

public class LocaleBuilderTest {

    public static void main(String[] args) {
        test1();
    }

    static void test1() {
        try {
            LocaleBuilder bldr = new LocaleBuilder();

            bldr.setLanguage("JA");
            bldr.setRegion("jp");
            bldr.setLocaleKeyword("ca", "japanese");
            bldr.setPrivateUse("Yoshito");

            Locale loc1 = bldr.create();

            bldr.clear();
//            bld2.setLocaleKeyword("CA", "Japanese");
            bldr.setExtension('u', "co_standard_ca_gregory");
            bldr.setLocaleKeyword("ca", "japanese");
            bldr.setLocaleKeyword("co", "");
//            bld2.setExtension('a', "123");
            bldr.setPrivateUse("yoshiTO");

            bldr.setRegion("Jp");
            bldr.setLanguage("Ja");

            Locale loc2 = bldr.create();

            System.out.println("loc1: " + loc1);
            System.out.println("loc2: " + loc2);

            System.out.println("loc1 == loc2 -> " + (loc1 == loc2));
            System.out.println(loc1.toLanguageTag());
            System.out.println(loc2.toLanguageTag());

            Locale loc3 = new LocaleBuilder().setLanguage("en").setScript("Latn").setVariant("1994").create();
            System.out.println(Locale.forLanguageTag(loc3.toLanguageTag()));

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
