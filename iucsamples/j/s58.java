//  Copyright (c) 2010 IBM Corporation and Others. All Rights Reserved.
import com.ibm.icu.util.*;
import com.ibm.icu.text.*;
import java.util.TreeSet;
import java.util.TreeMap;

public class s58 {

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
        {
            String locs[] = { "es", "root" };
            for(String loc : locs) { 
                msgFormat(new ULocale(loc));
            }
        }
        System.err.println("messageFormat");
    }
}
