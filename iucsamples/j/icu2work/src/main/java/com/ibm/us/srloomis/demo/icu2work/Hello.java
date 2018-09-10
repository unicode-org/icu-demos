// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

package com.ibm.us.srloomis.demo.icu2work;

import com.ibm.icu.text.LocaleDisplayNames;
import com.ibm.icu.util.ULocale;

import java.util.Locale;

/**
 * Hello world!
 *
 */
public class Hello
{
    public static void main( String[] args )
    {
        //Locale locale = Locale.getDefault();
        Locale locale = Locale.forLanguageTag("es");
        String world = LocaleDisplayNames
                .getInstance(ULocale.forLocale(locale))
                .regionDisplayName("001");
        System.out.println("Hello, " + world + "\u2603");
    }

}
