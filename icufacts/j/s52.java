//  Copyright (c) 2010-2012 IBM Corporation and Others. All Rights Reserved.
import java.util.TreeSet;

import com.ibm.icu.text.Collator;
import com.ibm.icu.util.ULocale;

public class s52 {
	public static void collation() {
		ULocale locale = new ULocale("fr");
		Collator coll = Collator.getInstance(locale);
		// do useful things with the collator

		String s1 = "World";
		String s2 = "Hello";

		System.out.println("Comparing " + s1 + " / " + s2 + " = "
				+ coll.compare(s1, s2));

		TreeSet<String> set = new TreeSet<String>(coll);
		set.add(s1);
		set.add(s2);
		for (String s : set) {
			System.out.println("   " + s);
		}
	}

	public static void main(String args[]) {
		collation();
	}
}
