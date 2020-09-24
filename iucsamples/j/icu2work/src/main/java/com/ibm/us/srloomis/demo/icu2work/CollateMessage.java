// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

package com.ibm.us.srloomis.demo.icu2work;

import com.ibm.icu.text.Collator;
import com.ibm.icu.text.MessageFormat;
import com.ibm.us.srloomis.demo.icu2work.impl.PopulationData;

import java.util.*;

public class CollateMessage {
    public static void main(String args[]) {
        final Locale locale = Locale.getDefault();
        ResourceBundle rb = ResourceBundle.getBundle(GoodMessage.class.getName());
        String popmsg = rb.getString("population");
        System.out.println("Message: " + popmsg);

        Collator col = Collator.getInstance(locale);
        for(final PopulationData.TerritoryEntry entry : PopulationData.getTerritoryEntries(locale,
                new TreeSet<>((o1, o2) -> col.compare(o1.territoryName(), o2.territoryName())))) {
            MessageFormat m = new MessageFormat(popmsg, locale);
            Map msgArgs = new HashMap<String,Object>();
            msgArgs.put("territory", entry.territoryName());
            msgArgs.put("population", entry.population());
            System.out.println(m.format(msgArgs));
        }
    }
}
