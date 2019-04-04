// Copyright (c) 2010 IBM Corporation and Others. All Rights Reserved.

package com.ibm.icu.dev.demo.icu4jweb;

import java.util.Enumeration;
import java.util.Hashtable;

import com.ibm.icu.util.ULocale;
import com.ibm.icu.util.UResourceBundle;
import com.ibm.icu.text.DecimalFormat;
import com.ibm.icu.text.MessageFormat;
import com.ibm.icu.text.NumberFormat;
import com.ibm.icu.text.RuleBasedNumberFormat;

public class ListPatternFormatter {
	private static final String LIST_PATTERN = "listPattern";

	private static final String STANDARD = "standard";

	private static final Object END = "end";
	private static final Object MIDDLE = "middle";
	private static final Object START = "start";

	private ULocale loc;
	
	private Hashtable<String, MessageFormat> formats = new Hashtable<String,MessageFormat>();
	
	/**
	 * Create an instance of the list pattern formatter
	 * @param locale the locale to use
	 * @return
	 */
	public  static ListPatternFormatter getInstance(ULocale locale) {
		return new ListPatternFormatter(locale, STANDARD);
	}
	private void loadMessagesFor(UResourceBundle bund, String type) {
		/*
		UResourceBundle parentBund = bund.getParent();
		if(parentBund!=null&&parentBund!=bund) {
			loadMessagesFor(parentBund);
		}*/
		UResourceBundle bnd2 = bund.get(LIST_PATTERN);
		UResourceBundle patBund = bnd2.get(type);
		for(Enumeration<String> keys = patBund.getKeys();
			keys.hasMoreElements();) {
			String key = keys.nextElement();
			String pat = patBund.getString(key);
			MessageFormat fmt = new MessageFormat(pat, loc);
			formats.put(key, fmt);
		}
	}
	private ListPatternFormatter(ULocale loc, String type) {
			this.loc = loc;
			
			// TODO: How do we load this bundle with fallback from the public API?
			
			// load root first.
			UResourceBundle bnd0 = UResourceBundle.getBundleInstance(ULocale.ROOT);
			loadMessagesFor(bnd0, type);
			
			// load bundle.
			UResourceBundle bnd1 = UResourceBundle.getBundleInstance(loc);
			loadMessagesFor(bnd1, type);
	}
	
	/**
	 * Format a list according to this locale
	 * @param list
	 * @return
	 */
	public String format(Object list[]) {
		StringBuffer result = new StringBuffer();
		
		// 00. if no elements, return null.
		if(list==null || (list.length==0)) return null;

		final int length = list.length;
		// 0. if 1 element, return it
		if(length==1) return list[0].toString();
		
		String out = null;
		MessageFormat fmt = null;
		
		// If there is an exact match, use it
		fmt = formats.get(Integer.toString(length));
		if(fmt!=null) {
			return fmt.format(list, result, null).toString();
		}
		
		// Format the last two items with the 'end' format
		MessageFormat end = formats.get(END);
		MessageFormat middle = formats.get(MIDDLE);
		MessageFormat start = formats.get(START);
		
		StringBuffer work = new StringBuffer();
		if(length>2) {
			String subList[] = {list[length-2].toString(),list[length-1].toString()};
			if(end==null) {
				throw new InternalError("Can't format a list of " + length + " items - "+END+" not defined for lists in locale " + loc);
			}
			end.format(subList, work, null);
		}
		
		// now for any 'middle'
		for(int j=length-3;j>0;j--) {
			String subList[] = {list[j].toString(), work.toString()};
			work = new StringBuffer();
			if(middle==null) {
				throw new InternalError("Can't format a list of " + length + " items - "+MIDDLE+" not defined for lists in locale " + loc);
			}
			middle.format(subList, work, null);
		}

		// now the beginning
		{
			String subList[] = {list[0].toString(), work.toString()};
			work = new StringBuffer();
			if(start==null) {
				throw new InternalError("Can't format a list of " + length + " items - "+START+" not defined for lists in locale " + loc);
			}
			start.format(subList, work, null);
		}
		
		return work.toString();
	}
	
	
	public static void main(String args[]) {
		String ll = "ru-ru";
		if(args.length>0) ll = args[0];
		ULocale inLocale = ULocale.forLanguageTag(ll);
		ListPatternFormatter lpf = ListPatternFormatter.getInstance(inLocale);
		NumberFormat rbnf = RuleBasedNumberFormat.getInstance(inLocale /*, RuleBasedNumberFormat.ORDINAL*/);
		String numlist[] = new String[10];
		for(int j=0;j<numlist.length;j++) {
			numlist[j]=rbnf.format(j);
		}
		
		for(int i=0;i<10;i++) {
			String q[] = new String[i];
			for(int j=0;j<i;j++) {
				q[j]=numlist[j];
			}
			
			System.out.println(i+": "+lpf.format(q));
		}
	}
}
