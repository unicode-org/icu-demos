// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

package com.ibm.us.srloomis.demo.icu2work;

import com.ibm.icu.text.MessageFormat;
import com.ibm.us.srloomis.demo.icu2work.impl.PopulationData;

import java.util.HashMap;
import java.util.Locale;
import java.util.Map;
import java.util.ResourceBundle;

public class GoodMessage {
    public static void main(String args[]) {
        final Locale locale = Locale.getDefault();
        ResourceBundle rb = ResourceBundle.getBundle(GoodMessage.class.getName());
        String popmsg = rb.getString("population");
        System.out.println("Message: " + popmsg);

        for(final PopulationData.TerritoryEntry entry : PopulationData.getTerritoryEntries(locale)) {
            MessageFormat m = new MessageFormat(popmsg, locale);
            Map msgArgs = new HashMap<String,Object>();
            msgArgs.put("territory", entry.territoryName());
            msgArgs.put("population", entry.population());
            System.out.println(m.format(msgArgs));
        }
    }
}
