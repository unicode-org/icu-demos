package com.ibm.icu.dev.sandbox;

import java.util.Random;

import java.util.Locale;

public class LocalePerf extends Thread {
    static String[] LOCIDS = {
        "",
        "ja_JP",
        "es_PE",
        "en",
        "ja_JP_JP",
        "es_PA",
        "sr_BA",
        "mk",
        "es_GT",
        "ar_AE",
        "no_NO",
        "sq_AL",
        "bg",
        "ar_IQ",
        "ar_YE",
        "hu",
        "pt_PT",
        "el_CY",
        "ar_QA",
        "mk_MK",
        "sv",
        "de_CH",
        "en_US",
        "fi_FI",
        "is",
        "cs",
        "en_MT",
        "sl_SI",
        "sk_SK",
        "it",
        "tr_TR",
        "zh",
        "th",
        "ar_SA",
        "no",
        "en_GB",
        "sr_CS",
        "lt",
        "ro",
        "en_NZ",
        "no_NO_NY",
        "lt_LT",
        "es_NI",
        "nl",
        "ga_IE",
        "fr_BE",
        "es_ES",
        "ar_LB",
        "ko",
        "fr_CA",
        "et_EE",
        "ar_KW",
        "sr_RS",
        "es_US",
        "es_MX",
        "ar_SD",
        "in_ID",
        "ru",
        "lv",
        "es_UY",
        "lv_LV",
        "iw",
        "pt_BR",
        "ar_SY",
        "hr",
        "et",
        "es_DO",
        "fr_CH",
        "hi_IN",
        "es_VE",
        "ar_BH",
        "en_PH",
        "ar_TN",
        "fi",
        "de_AT",
        "es",
        "nl_NL",
        "es_EC",
        "zh_TW",
        "ar_JO",
        "be",
        "is_IS",
        "es_CO",
        "es_CR",
        "es_CL",
        "ar_EG",
        "en_ZA",
        "th_TH",
        "el_GR",
        "it_IT",
        "ca",
        "hu_HU",
        "fr",
        "en_IE",
        "uk_UA",
        "pl_PL",
        "fr_LU",
        "nl_BE",
        "en_IN",
        "ca_ES",
        "ar_MA",
        "es_BO",
        "en_AU",
        "sr",
        "zh_SG",
        "pt",
        "uk",
        "es_SV",
        "ru_RU",
        "ko_KR",
        "vi",
        "ar_DZ",
        "vi_VN",
        "sr_ME",
        "sq",
        "ar_LY",
        "ar",
        "zh_CN",
        "be_BY",
        "zh_HK",
        "ja",
        "iw_IL",
        "bg_BG",
        "in",
        "mt_MT",
        "es_PY",
        "sl",
        "fr_FR",
        "cs_CZ",
        "it_CH",
        "ro_RO",
        "es_PR",
        "en_CA",
        "de_DE",
        "ga",
        "de_LU",
        "de",
        "es_AR",
        "sk",
        "ms_MY",
        "hr_HR",
        "en_SG",
        "da",
        "mt",
        "pl",
        "ar_OM",
        "tr",
        "th_TH_TH",
        "el",
        "ms",
        "sv_SE",
        "da_DK",
        "es_HN",
    };

    private static final boolean DEBUG = false;

    public static void main(String[] args) {
        int numThread = 5;
        int numIteration = 5000000;

        LocalePerf[] tests = new LocalePerf[numThread];

        for (int i = 0; i < numThread; i++) {
            tests[i] = new LocalePerf("Thread" + i, numIteration);
        }
        for (int i = 0; i < tests.length; i++) {
            tests[i].start();
        }
    }

    public void run() {
        roundTripTest(_itr);
        equalityTest1(_itr);
        equalityTest2(_itr);
    }

    private String _name;
    private int _itr;
    private Timer tm;

    public LocalePerf(String name, int n) {
        super(name);
        _name = name;
        _itr = n;
        tm = new Timer();
    }

    void roundTripTest(int iteration) {
        Random rnd = new Random();
        tm.reset("Start roundTripTest (" + _name + ") : iteration - " + iteration);
        for (int i = 0; i < iteration; i++) {
            if (i == iteration / 2) System.gc();
            String s = roundTrip(LOCIDS[rnd.nextInt(LOCIDS.length)]);
            if (DEBUG) {
                System.out.println(s);
            }
        }
        tm.log("End roundTripTest (" + _name + ") : elapsed time");
    }

    void equalityTest1(int iteration) {
        Locale[] all = getAll();
        Random rnd = new Random();

        tm.reset("Start equalityTest1 (" + _name + ") : iteration - " + iteration);
        for (int i = 0; i < iteration; i++) {
            int idx = rnd.nextInt(all.length);
            Locale l = all[idx];
            for (int j = 0; j < all.length; j++) {
                if (l.equals(all[j])) {
                    if (DEBUG) {
                        System.out.println(i==j);
                    }
                    break;
                }
            }
        }
        tm.log("End equalityTest1 (" + _name + ") : elapsed time");
    }

    void equalityTest2(int iteration) {
        Locale[] all = getAll();
        Locale[] all2 = getAll();
        Random rnd = new Random();

        tm.reset("Start equalityTest2 (" + _name + ") : iteration - " + iteration);
        for (int i = 0; i < iteration; i++) {
            int idx = rnd.nextInt(all.length);
            Locale l = all[idx];
            for (int j = 0; j < all.length; j++) {
                if (l.equals(all2[j])) {
                    break;
                }
            }
        }
        tm.log("End equalityTest2 (" + _name + ") : elapsed time");
    }

    static String roundTrip(String id) {
        return fromID(id).toString();
    }

    static Locale fromID(String id) {
        int start = 0;
        int idx = id.indexOf('_');
        String lang = "", cnty = "", vart = "";
        if (idx > 0) {
            lang = id.substring(0, idx);
            start = idx + 1;
            idx = id.indexOf("_", start);
            if (idx > 0) {
                cnty = id.substring(start, idx);
                start = idx + 1;
                vart = id.substring(start);
            } else {
                cnty = id.substring(start);
            }
        } else {
            lang = id;
        }

        return new Locale(lang, cnty, vart);
//        return Locale.forLocale(lang, cnty, vart);
//        return Locale.forID(id);
    }

    static Locale[] getAll() {
        Locale[] all = new Locale[LOCIDS.length];
        for (int i = 0; i < LOCIDS.length; i++) {
            all[i] = fromID(LOCIDS[i]);
        }
        return all;
    }

    static class Timer {
        long t;

        public long reset(String message) {
            if (message != null) {
                System.out.println(message);
            }
            t = System.currentTimeMillis();
            return t;
        }

        public long reset() {
            return reset(null);
        }

        public long log() {
            return log("");
        }

        public long log(String prefix) {
            long now = System.currentTimeMillis();
            System.out.println(prefix + " - " + (now - t) + "ms");
            return now;
        }
    }
}
