//  Copyright (c) 2010-2012 IBM Corporation and Others. All Rights Reserved.
import java.util.TreeMap;

import com.ibm.icu.text.CollationKey;
import com.ibm.icu.text.Collator;
import com.ibm.icu.text.RawCollationKey;
import com.ibm.icu.util.ULocale;

public class s56 {
    public static void collationKey() {
        ULocale locale = new ULocale("fr");
        Collator coll = Collator.getInstance(locale);

	String strs[] = { "bad", "baz", "Bat", "BAD", "bat" };

	TreeMap<RawCollationKey, String> rawMap = new TreeMap<RawCollationKey, String>();
	TreeMap<CollationKey, String> keyMap = new TreeMap<CollationKey, String>();
	for(String s : strs) {

            RawCollationKey rk = coll.getRawCollationKey(s, null);
            rawMap.put(rk,s);
            
            System.out.println("str " + s + ", raw: " + rk.toString());


            CollationKey key = coll.getCollationKey(s);
            keyMap.put(key, s);
	}

        System.out.println("in raw map order: " ) ;
        for(String s : rawMap.values()) {
            System.out.println("\t"+s);
        }
        System.out.println("in key map order: " ) ;
        for(String s : keyMap.values()) {
            System.out.println("\t"+s);
        }

        RawCollationKey bazKey = coll.getRawCollationKey("baz",null);
        System.out.println("I want 'baz', key " + bazKey);
        System.out.println(" -> " + rawMap.get(bazKey));
    }
 
    public static void main(String args[]) {
  	collationKey();
	System.err.println("collationKey");
    }
}
