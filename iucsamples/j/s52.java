//  Copyright (c) 2010 IBM Corporation and Others. All Rights Reserved.
import com.ibm.icu.util.*;
import com.ibm.icu.text.*;
import java.util.TreeSet;
import java.util.TreeMap;

public class s52 {
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


    public static void main(String args[]) {
        collation();
        System.err.println("collation");
    }
}
