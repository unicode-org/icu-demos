/*
*******************************************************************************
*
*   Copyright (C) 2005-2006, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  demo_settings.h
*   encoding:   US-ASCII
*   tab size:   4 (not used)
*   indentation:4
*
*   created on: 2005May24
*   created by: George Rhoten
*
*   These are the settings that are common to all demos.
*
*/

#ifndef DEMO_SETTINGS_H
#define DEMO_SETTINGS_H 1

#define DEMO_COMMON_DIR "data/"
#define DEMO_COMMON_MASTHEAD DEMO_COMMON_DIR "common-masthead.html"
#define DEMO_COMMON_LEFTNAV DEMO_COMMON_DIR "common-leftnav.html"
#define DEMO_COMMON_FOOTER DEMO_COMMON_DIR "common-footer.html"

/* #define DEMO_BEGIN_LEFT_NAV "<table width=\"100%\" border=\"0\""\
"cellspacing=\"0\" cellpadding=\"0\" id=\"v14-body-table\">\n"\
"<tr valign=\"top\">\n"\
"<td width=\"150\" id=\"navigation\">\n" */
#define DEMO_BEGIN_LEFT_NAV "<!-- DEMO_BEGIN_LEFT_NAV -->"

#define DEMO_END_LEFT_NAV "</td>\n"
#define DEMO_BEGIN_CONTENT "<td colspan=\"2\" valign=\"top\" style=\"padding-left: 5px; padding-right: 5px;\"><a name=\"main\"></a>\n"
#define DEMO_END_CONTENT "</td>\n</tr>\n</table>\n"

#define ICU_URL "http://www.icu-project.org/"

#define DEMO_BREAD_CRUMB_BAR     "<a class=\"bctl\" href=\"//www.icu-project.org/\">ICU</a><span class=\"bct\">&nbsp;&nbsp;&gt;&nbsp;</span>\n"\
    "<a class=\"bctl\" href=\"http://demo.icu-project.org/icu-bin/icudemos\">Demonstrations</a><span class=\"bct\">&nbsp;&nbsp;&gt;&nbsp;</span>\n"
    
#endif

