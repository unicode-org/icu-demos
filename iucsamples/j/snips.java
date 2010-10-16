//  Copyright (c) 2010 IBM Corporation and Others. All Rights Reserved.
import com.ibm.icu.util.*;
import com.ibm.icu.text.*;
import java.util.TreeSet;
import java.util.TreeMap;

public class snips {
    public static void collation() {
        ULocale locale = new ULocale("fr");
        Collator coll = Collator.getInstance(locale);
        // do useful things with the collator

	String s1 = "Hello";
	String s2 = "World";
	
	System.out.println("Comparing " + s1 + " / " + s2 + " = " + coll.compare(s1,s2));

	TreeSet<String> set = new TreeSet<String>(coll);
	set.add(s1);
	set.add(s2);
	for(String s : set) {
	 	System.out.println("   " + s );
	}
    }

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

    public static void msgFormat(ULocale loc) {
        UResourceBundle resourceBundle = UResourceBundle.getBundleInstance("data/myapp", loc);
        String person = resourceBundle.getString("Aunt"); // e.g. “My Aunt”
        String place = resourceBundle.getString("table"); // e.g. “on the table” 
        String thing = resourceBundle.getString("pen"); // e.g. “pen” 
        Object arguments[] = {person, place, thing};
        String pattern = resourceBundle.getString("personPlaceThing");
        MessageFormat msgFmt = new MessageFormat(pattern); 
        String message = msgFmt.format(arguments);
        System.out.println(loc + ": " + message);
    }

    public static void main(String args[]) {
        collation();
        System.err.println("collation");
	collationKey();
	System.err.println("collationKey");
        {
            String locs[] = { "es", "root" };
            for(String loc : locs) { 
                msgFormat(new ULocale(loc));
            }
        }
        System.err.println("messageFormat");
    }
}
