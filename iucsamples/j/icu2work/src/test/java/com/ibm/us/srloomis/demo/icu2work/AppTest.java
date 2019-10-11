package com.ibm.us.srloomis.demo.icu2work;

import org.junit.Test;

import java.lang.reflect.Method;

public class AppTest
{
    final static private String[] NO_ARGS = new String[0];

    enum AllApps {
        Hello,
        BadMessage
    };
    /**
     * Rigourous Test :-)
     */
    @Test
    public void testApp()
    {
        for(AllApps a : AllApps.values()) {
            final String app = a.name();
            System.out.println("** " + app);
            final String className = getClass().getPackage().getName()+"."+app;
            try {
                Class c = Class.forName(className);
                Method main = c.getMethod("main", String[].class);
                main.invoke(null, (Object) NO_ARGS);
            } catch (Throwable e) {
                System.out.println("Err: " + className);
                e.printStackTrace();
            }
        }
    }
}