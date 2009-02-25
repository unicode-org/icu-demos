package com.ibm.icu.dev.sandbox;

import com.ibm.icu.dev.tools.languagetag.LocodeRegistry;
import com.ibm.icu.dev.tools.languagetag.LocodeRegistry.Locode;

public class LocodeCheck {
    public static void main(String[] args) {
        LocodeRegistry reg = LocodeRegistry.getInstance();
        Locode locode = reg.getByCode("USNYC");
        System.out.println(locode.getName());
    }
}
