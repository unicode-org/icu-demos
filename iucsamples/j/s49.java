//  Copyright (c) 2010 IBM Corporation and Others. All Rights Reserved.

import com.ibm.icu.util.ULocale;
import com.ibm.icu.util.VersionInfo;
public class s49 {
    public static void main(String[] args) {
        System.out.println("Hello, " + new ULocale("und_001")
                           .getDisplayCountry(ULocale.GERMAN)
                           + "! ICU " + VersionInfo.ICU_VERSION);
    }
}
