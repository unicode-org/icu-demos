/*
*******************************************************************************
*
*   Copyright (C) 2006-2007, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   created on: 2006Oct28
*   created by: George Rhoten
*/

#include "toolutil.h"
#include "uoptions.h"
#include "package.h"
#include "hash.h"
#include "uvector.h"
#include "tmplutil.h"
#include "listmod.h"
#include "unicode/regex.h"
#include "unicode/udata.h"
#include "unicode/ures.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define LENGTHOF(array) (int32_t)(sizeof(array)/sizeof((array)[0]))

#define DATADEP_PACKAGE "datadep"
#define BASE_DATA "base"
#define NBSP 0x00A0

U_CFUNC char datadep_dat[];

static const char *gPackage;
static const char *gCgiName;
static const char *gHtmlHeader;
static const char *gHtmlMasthead;
static const char *gHtmlLeftNav;
static const char *gHtmlFooter;
static UVector *hiddenItems;
static UVector *additionalItems;

class ItemGroup {
public:
    UBool hiddenByDefault;
    UErrorCode throwAwayStatus;
    RegexMatcher matcher;
    const char *group;
    const char *description;
    UVector items;
    ItemGroup(UBool hidden, const char *regexPattern, const char *grp, const char *desc, UErrorCode &status) :
        hiddenByDefault(hidden),
        throwAwayStatus(U_ZERO_ERROR),
        matcher(regexPattern, UREGEX_CASE_INSENSITIVE, status),
        group(grp),
        description(desc),
        items(status)
    {
    }
    ItemGroup(const ItemGroup& other) :
        hiddenByDefault(other.hiddenByDefault),
        throwAwayStatus(U_ZERO_ERROR),
        matcher(other.matcher.pattern().pattern(), UREGEX_CASE_INSENSITIVE, throwAwayStatus),
        group(other.group),
        description(other.description),
        items(throwAwayStatus)
    {
        // TODO: Copying the items isn't needed right now.
    }
    int32_t getSize(const UVector &skipList) const {
        int32_t idx, size = 0;
        int32_t length = items.size();
        Item itemToFind;
        memset(&itemToFind, 0, sizeof(Item));
        for (idx = 0; idx < length; idx++) {
            Item *currItem = ((Item *)items.elementAt(idx));
            itemToFind.name = currItem->name;
            if (!skipList.contains(&itemToFind)) {
                size += currItem->length;
            }
        }
        return size;
    }
};

static void U_EXPORT2
freeBlock(void *obj) {
    free(obj);
}
U_CAPI UBool U_EXPORT2
compareItems(const UHashTok key1, const UHashTok key2) {
    UHashTok p1;
    UHashTok p2;
    p1.pointer = ((const Item*) key1.pointer)->name;
    p2.pointer = ((const Item*) key2.pointer)->name;
    return uhash_compareChars(p1, p2);
}

double convertBytesToKilobytes(double bytes) {
    double kilobytes = ((double)bytes)/1024.0;
    if (kilobytes > 1.0) {
        kilobytes = floor(kilobytes + 0.5);  // Do standard rounding
    } else if (kilobytes > 0.0) {
        kilobytes = floor((kilobytes * 10) + 0.5) / 10;  // Do standard rounding
    }
    return kilobytes;
}

UnicodeString replaceAll(UnicodeString str, const UnicodeString &fromStr, const UnicodeString &toStr) {
    int32_t idx;
    while ((idx = str.indexOf(fromStr)) >= 0) {
        str.replace(idx, fromStr.length(), toStr);
    }
    return str;
}

UnicodeString getTranslation(const char *key) {
    static const UChar NBSP_STR[] = {NBSP,0};
    const UChar *string = NULL;
    UnicodeString uniStr;
    UErrorCode status = U_ZERO_ERROR;
    UResourceBundle *res = ures_open(DATADEP_PACKAGE, NULL, &status);
    if(U_FAILURE(status)) {
        fprintf(stderr, "Error in ures_open("DATADEP_PACKAGE"): %s.\n", u_errorName(status));
        exit(status);
    }

    // See if the key is translated
    string = ures_getStringByKey(res, key, NULL, &status);

    ures_close(res);
    
    if (string == NULL) {
        UnicodeString keyUniStr(key);
        if (keyUniStr.endsWith(".res")) {
            char localeStr[ULOC_FULLNAME_CAPACITY];
            int32_t startOfLocale = keyUniStr.indexOf('/') + 1; // if -1 is returned, then 0 is used.
            int32_t endOfLocale = keyUniStr.lastIndexOf('.');
            strncpy(localeStr, key + startOfLocale, endOfLocale - startOfLocale);
            localeStr[endOfLocale - startOfLocale] = 0;
            Locale(localeStr).getDisplayName(uniStr);
        }
        else {
            uniStr = NBSP_STR;
        }
    }
    else {
        uniStr = string;
    }
    uniStr = replaceAll(uniStr, "&", "&amp;"); // Perform this substitution first
    uniStr = replaceAll(uniStr, "<", "&lt;");
    uniStr = replaceAll(uniStr, ">", "&gt;");

    return uniStr;
}

/* TODO Move this information to the list file. */
#define ICU4J_DEPENDENCIES "['ubidi_icu', 'ucase_icu', 'unorm_icu', 'uprops_icu']"

/* Consider xxx.res to be the longest locale */
#define MAX_RES_WITH_LANG_LEN 7
class DependencyInfo {
public:
    Hashtable dependsOn;
    Hashtable neededBy;
    DependencyInfo(UErrorCode &status)
        : dependsOn(status),
        neededBy(status)
    {
        dependsOn.setKeyCompartor(uhash_compareUnicodeString);
        dependsOn.setValueDeleter(uhash_deleteUVector);
        neededBy.setKeyCompartor(uhash_compareUnicodeString);
        neededBy.setValueDeleter(uhash_deleteUVector);
    }
    void addDependencyRoundtrip(const char *itemName, const UnicodeString &targetName, UErrorCode &status) {
        UnicodeString itemNameUStr(itemName);
        const UHashElement *elem = dependsOn.find(itemNameUStr);
        UVector *dependsOnVector;
        UnicodeString *targetNameStrPtr = new UnicodeString(targetName); // freed by dependsOnVector
        if (elem == NULL) {
            dependsOnVector = new UVector(uhash_deleteUnicodeString, uhash_compareUnicodeString, status);
            dependsOn.put(itemNameUStr, dependsOnVector, status);
        }
        else {
            dependsOnVector = (UVector*)elem->value.pointer;
        }
        if (!dependsOnVector->contains(targetNameStrPtr)) {
            dependsOnVector->addElement(targetNameStrPtr, status);
        }
        UVector *neededByVector;
        elem = neededBy.find(*targetNameStrPtr);
        if (elem == NULL) {
            neededByVector = new UVector(NULL, uhash_compareUnicodeString, status);
            neededBy.put(*targetNameStrPtr, neededByVector, status);
        }
        else {
            neededByVector = (UVector*)elem->value.pointer;
        }
        if (!neededByVector->contains(&itemNameUStr)) {
            neededByVector->addElement(new UnicodeString(itemNameUStr), status);
        }
    }
};

static void checkDependency(void *context, const char *itemName, const char *targetName) {
    DependencyInfo *myThis = (DependencyInfo*)context;
    UErrorCode status = U_ZERO_ERROR;
    myThis->addDependencyRoundtrip(itemName, targetName, status);
    //fprintf(stderr, "Item %s depends on missing item %s\n", itemName, targetName);
}

/*
Normally checkDependency should get everything.
This function allows the items not in the package by default to get decent dependencies too.
*/
void enumerateDefaultDependencies(const char *itemName, DependencyInfo *depInfo, UErrorCode &status) {
    const char *extension = strstr(itemName, ".res");
    if (extension && strcmp(extension, ".res") == 0) {
        UnicodeString itemNameUStr(itemName);
        char *itemNameCopy = strdup(itemName);
        const UHashElement *elem = depInfo->dependsOn.find(itemNameUStr);
        UnicodeString parentLocale = NULL; // freed by dependsOnVector
        char *separator = strrchr(itemNameCopy, '_');
        if (separator && strstr(itemNameCopy, "res_index.res") == NULL) {
            strcpy(separator, ".res");
            parentLocale = UnicodeString(itemNameCopy);
        }
        else {
            char *treeSeparator = strrchr(itemNameCopy, U_TREE_ENTRY_SEP_CHAR);
            // Check if the string is less than or equal to than xxx.res
            if (treeSeparator && strlen(treeSeparator + 1) <= MAX_RES_WITH_LANG_LEN) {
                treeSeparator[0] = 0;
                parentLocale = UnicodeString(UnicodeString(itemNameCopy) + UnicodeString(U_TREE_ENTRY_SEP_STRING "root.res"));
                treeSeparator[0] = U_TREE_ENTRY_SEP_CHAR;
            }
            else if (strlen(itemNameCopy) <= MAX_RES_WITH_LANG_LEN) {
                parentLocale = UnicodeString("root.res");
            }
        }
        if (parentLocale != NULL) {
            depInfo->addDependencyRoundtrip(itemName, parentLocale, status);
        }
    }
}

UnicodeString createValidID(UnicodeString str) {
    int32_t idx;
    while ((idx = str.indexOf('/')) != -1) {
        str.replace(idx, 1, (UChar)'_');
    }
    while ((idx = str.indexOf('.')) != -1) {
        str.replace(idx, 1, (UChar)'_');
    }
    while ((idx = str.indexOf('-')) != -1) {
        str.replace(idx, 1, (UChar)'_');
    }
    return str;
}

UnicodeString generateDependencyList(const char *itemName, Hashtable *dependencyList) {
    UnicodeString itemNameUStr(itemName);
    UnicodeString javascriptList;
    const UHashElement *elem = dependencyList->find(itemNameUStr);
    if (elem != NULL) {
        UVector *dependencyVector = (UVector*)elem->value.pointer;
        if (dependencyVector->size() > 0) {
            javascriptList += "[";
            for (int32_t idx = 0; idx < dependencyVector->size(); idx++) {
                if (idx > 0) {
                    javascriptList += UnicodeString(",");
                }
                javascriptList += UnicodeString("'") + createValidID(*(UnicodeString *)dependencyVector->elementAt(idx))
                    + UnicodeString("'");
            }
            javascriptList += "]";
        }
        else {
            javascriptList = "null";
        }
    }
    else {
        javascriptList = "null";
    }
    return javascriptList;
}

static UnicodeString
generateHTML(Package *pkg, UErrorCode &status) {
    int32_t i;
    int32_t catagoryIdx;
    UnicodeString html;
    UnicodeString variables("var gItems = [];\n");
    UnicodeString variableSizes("var gSize = [];\n");
    UnicodeString dataList;
    char kilobytesStr[64];
    char bytesStr[64];
    int32_t totalBytes = 0;
    DependencyInfo depInfo(status);

    ItemGroup catagorizedItems[] = {
        ItemGroup(FALSE, ".+\\.cnv$", "conv", "Charset Mapping Tables", status),
        ItemGroup(FALSE, "brkitr/.+", "brkiter", "Break Iterator", status),
        ItemGroup(FALSE, "(coll/.+|ucadata.icu|invuca.icu)", "coll", "Collators", status),
        ItemGroup(FALSE, "rbnf/.+", "rbnf", "Rule Based Number Format", status),
        ItemGroup(FALSE, "translit/.+", "translit", "Transliterators", status),
        ItemGroup(TRUE, "zoneinfo.res", "zoneinfo", "Timezone Data", status),
        // This next one should be the last one searching for .res locale files.
        ItemGroup(FALSE, "(...?(_|\\.).*res|root.res)$", "format", "Formatting, Display Names and Other Localized Data", status),
        ItemGroup(TRUE, "(pnames.icu|unames.icu|.+\\.spp|supplementalData.res|CurrencyData.res)", "misc", "Miscellaneous Data", status),
        ItemGroup(FALSE, ".+", BASE_DATA, "Base Data", status)
    };

    if (U_FAILURE(status)) {
        printf("Initialization failed with %s\n", u_errorName(status));
        exit(-1);
    }

    pkg->enumDependencies(&depInfo, checkDependency);

    // Categorize the items in the normal package
    for(i=0; i<pkg->getItemCount(); ++i) {
        const Item *currItem = pkg->getItem(i);
        enumerateDefaultDependencies(currItem->name, &depInfo, status);
        for(catagoryIdx = 0; catagoryIdx < LENGTHOF(catagorizedItems); ++catagoryIdx) {
            UnicodeString str(currItem->name);
            catagorizedItems[catagoryIdx].matcher.reset(str);
            if (catagorizedItems[catagoryIdx].matcher.matches(status)) {
                catagorizedItems[catagoryIdx].items.addElement((void *)currItem, status);
                break; // No need to look any further.
            }
        }
    }

    // Categorize the items in the modifier list
    for(i=0; i<additionalItems->size(); ++i) {
        Item *currItem = (Item *)additionalItems->elementAt(i);
        for(catagoryIdx = 0; catagoryIdx < LENGTHOF(catagorizedItems); ++catagoryIdx) {
            UnicodeString str(currItem->name);
            catagorizedItems[catagoryIdx].matcher.reset(str);
            if (catagorizedItems[catagoryIdx].matcher.matches(status)) {
                catagorizedItems[catagoryIdx].items.addElement((void *)currItem, status);
                break; // No need to look any further.
            }
        }
    }

    // This javascript function is needed to improve the performance of IE 7
    variables += UnicodeString("function initializeItems() {\n");


    for(catagoryIdx = 0; catagoryIdx < LENGTHOF(catagorizedItems); ++catagoryIdx) {
        const ItemGroup *currCategory = &catagorizedItems[catagoryIdx];
        int32_t length = currCategory->items.size();
        int32_t bytes = currCategory->getSize(*additionalItems);
        UBool hidden = strcmp(currCategory->group, BASE_DATA) == 0;
        double kilobytes = convertBytesToKilobytes(bytes);
        totalBytes += bytes;
        sprintf(kilobytesStr, "%g", kilobytes);
        sprintf(bytesStr, "%i", bytes);
        dataList += UnicodeString("<div><span onclick=\"ShowHide('") + UnicodeString(currCategory->group)
            + UnicodeString(".link')\" id=\"") + UnicodeString(currCategory->group)
            + UnicodeString(".link\" class=\"expander\">+</span>\n");
        if (hidden) {
            dataList += UnicodeString("<span class=\"hide\">");
        }
        dataList += UnicodeString("<input type=\"checkbox\" id=\"") + UnicodeString(currCategory->group)
            + UnicodeString("\" onclick=\"selectGroup(this, '") + UnicodeString(currCategory->group)
            + UnicodeString("')\" checked=\"checked\" />");
        if (hidden) {
            dataList += UnicodeString("</span>");
        }
        else {
            dataList += UnicodeString("<label for=\"") + UnicodeString(currCategory->group)
                + UnicodeString("\">");
        }
        dataList += UnicodeString(currCategory->description)
            + UnicodeString(" (<span id=\"") + UnicodeString(currCategory->group)
            + UnicodeString(".kilobytes\">") + UnicodeString(kilobytesStr)
            + UnicodeString("</span> KB)");
        if (!hidden) {
            dataList += UnicodeString("</label>");
        }
        dataList += UnicodeString("</div>\n");
        variableSizes += UnicodeString("gSize['") + createValidID(UnicodeString(currCategory->group))
            + UnicodeString("']=") + UnicodeString(bytesStr)
            + UnicodeString(";\n");
        dataList += UnicodeString("<div id=\"") + UnicodeString(currCategory->group)
            + UnicodeString(".link.group\" class=\"itemGroup\">\n");
        dataList += UnicodeString("<table cellspacing=\"0\" class=\"linkGroup\">\n");
        dataList += UnicodeString("<caption>Click on a table header to sort the list</caption>\n");
        dataList += UnicodeString("<thead><tr>\n")
            + UnicodeString("<th title=\"Click here to sort\" class=\"clickable\" onclick=\"Table.sort(this,{'sortType':Sort.IgnoreCase})\">Data Item</th>\n")
            + UnicodeString("<th title=\"Click here to sort\" class=\"clickable\" onclick=\"Table.sort(this,{'sortType':Sort.IgnoreCase})\">Description</th>\n")
            + UnicodeString("<th title=\"Click here to sort\" class=\"clickable\" onclick=\"Table.sort(this,{'sortType':Sort.Numeric})\">Kilobytes</th>\n")
            + UnicodeString("</tr></thead>\n");
        dataList += UnicodeString("<tbody>\n");
        for (i = 0; i < length; i++) {
            const Item *currItem = ((Item *)currCategory->items.elementAt(i));
            char itemByteSizeStr[64];
            char itemKilobyteSizeStr[64];

            sprintf(itemByteSizeStr, "%d", currItem->length);
            sprintf(itemKilobyteSizeStr, "%g", convertBytesToKilobytes(currItem->length));
            Item itemToFind;
            memset(&itemToFind, 0, sizeof(Item));
            itemToFind.name = currItem->name;
            UnicodeString checkedItem((additionalItems->contains(&itemToFind) ? "" : " checked=\"checked\""));
            UnicodeString rowClass;
            UnicodeString inputID = createValidID(UnicodeString(currItem->name));
            UBool hideItemForIndexing = currCategory->hiddenByDefault;

            if (!inputID.endsWith(UnicodeString("_res"))) {
                hideItemForIndexing = FALSE;
            }
            if (hiddenItems->contains(currItem->name)) {
                rowClass = " class=\"hide\"";
                hideItemForIndexing = TRUE;
            }
            else if (additionalItems->contains(&itemToFind)) {
                rowClass = " class=\"unselected\"";
            }
            if (inputID.endsWith(UnicodeString("root_res"))) {
                hideItemForIndexing = TRUE;
            }

            variables += UnicodeString("gItems['") + inputID
                + UnicodeString("']=new ItemInfo('") + UnicodeString(currCategory->group)
                + UnicodeString("',") + UnicodeString(hideItemForIndexing ? "1" : "0")
                + UnicodeString(",") + UnicodeString(itemByteSizeStr)
                + UnicodeString(",") + generateDependencyList(currItem->name, &depInfo.dependsOn)
                + UnicodeString(",") + generateDependencyList(currItem->name, &depInfo.neededBy)
                + UnicodeString(");\n");

            dataList += UnicodeString("<tr") + rowClass
                + UnicodeString("><td class=\"nw\">");
            if (hidden) {
                dataList += UnicodeString("<span class=\"hide\">");
            }
            else {
                dataList += UnicodeString("<label>");
            }
            dataList += UnicodeString("<input type=\"checkbox\" name=\"item\" id=\"") + inputID
                + UnicodeString("\" value=\"") + UnicodeString(currItem->name)
                + UnicodeString("\" onclick=\"modifySize(this)\"") + checkedItem
                + UnicodeString(" />");
            if (hidden) {
                dataList += UnicodeString("</span>") + UnicodeString(currItem->name);
            }
            else {
                dataList += UnicodeString(currItem->name) + UnicodeString("</label>");
            }
            dataList += UnicodeString("</td><td>") + getTranslation(currItem->name)
                + UnicodeString("</td><td class=\"s\">") + UnicodeString(itemKilobyteSizeStr)
                + UnicodeString("</td></tr>\n");
        }
        dataList += UnicodeString("</tbody>\n");
        dataList += UnicodeString("</table>\n");
        dataList += UnicodeString("</div>\n");
    }
    sprintf(kilobytesStr, "%g", convertBytesToKilobytes(totalBytes));
    sprintf(bytesStr, "%i", totalBytes);
    variableSizes += UnicodeString("var totalSize = ") + UnicodeString(bytesStr)
        + UnicodeString(";\n");
    variables += UnicodeString("gItems['ICU4C']=new ItemInfo('"BASE_DATA"',1,0,[],[]);\n");
    variables += UnicodeString("gItems['ICU4J']=new ItemInfo('"BASE_DATA"',1,0,"ICU4J_DEPENDENCIES","ICU4J_DEPENDENCIES");\n");
    variables += UnicodeString("}\n");


    html += UnicodeString("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"\n"
        "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n");
    html += UnicodeString("<html>\n");
    html += UnicodeString("<head>\n");
    html += UnicodeString("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"/>\n");
    insertTemplateFile(html, gHtmlHeader, NULL);
    html += UnicodeString("<script type=\"text/javascript\">\n");
    html += UnicodeString("//<![CDATA[\n");
    html += variableSizes;
    html += variables;
    html += UnicodeString("//]]>\n");
    html += UnicodeString("</script>\n");
    html += UnicodeString("</head>\n");
    html += UnicodeString("<body onload=\"initPage()\">\n");
    insertTemplateFile(html, gHtmlMasthead, NULL);
    html += UnicodeString(DEMO_BEGIN_LEFT_NAV);
    insertTemplateFile(html, gHtmlLeftNav, NULL);
    html += UnicodeString(DEMO_END_LEFT_NAV);
    html += UnicodeString(DEMO_BEGIN_CONTENT);
    html += UnicodeString(DEMO_BREAD_CRUMB_BAR);
    html += UnicodeString("<h1>ICU Data Library Customizer</h1>\n");
    html += UnicodeString("<noscript><p style=\"color: red;\">WARNING! Javascript must be enabled to allow this tool to work properly.</p></noscript>\n");
    //html += UnicodeString("<form method=\"post\" id=\"packageRequest\" action=\"") + UnicodeString(gCgiName) + UnicodeString("\">\n");
    // onsubmit="return false;" prevents a traditional form submission.
    html += UnicodeString("<form onsubmit=\"submitPackageRequest();return false;\" id=\"packageRequest\" action=\"") + UnicodeString(gCgiName) + UnicodeString("\">\n");

    html += dataList;

    html += UnicodeString("<p>Target Data Library Format</p>\n");
    // onfocus is needed on these radio buttons due to Opera
    // onclick is needed when reloading and focus moves elsewhere on the page.
    html += UnicodeString("<div>\n");
    html += UnicodeString("<label><input type=\"radio\" name=\"packagetype\" value=\"ICU4C\" id=\"ICU4C\" onclick=\"selectPackageType(this)\" onfocus=\"selectPackageType(this)\" checked=\"checked\" />ICU4C "U_ICU_VERSION"</label><br />\n");
    html += UnicodeString("<label><input type=\"radio\" name=\"packagetype\" value=\"ICU4J\" id=\"ICU4J\" onclick=\"selectPackageType(this)\" onfocus=\"selectPackageType(this)\" />ICU4J</label><br />\n");
    html += UnicodeString("<input type=\"hidden\" name=\"version\" id=\"version\" value=\""U_ICU_VERSION_SHORT"\" />\n");
    html += UnicodeString("<div id=\"progressOutput\" style=\"white-space: pre; padding: 1em; font-family: monospace; color: red\">&nbsp;</div>\n");
    //html += UnicodeString("<input type=\"submit\" value=\"Get Data Library\" /><br />\n");
    html += UnicodeString("<button type=\"button\" onclick=\"submitPackageRequest()\">Get Data Library</button><br />\n");
    html += UnicodeString("</div>\n");
    html += UnicodeString("<p>The estimated uncompressed size of this data library is <span id=\"totalSize.kilobytes\">") + UnicodeString(kilobytesStr)
        + UnicodeString("</span> KB</p>\n");
    html += UnicodeString("</form>\n");
    html += UnicodeString("<span onclick=\"ShowHide('advanced')\" id=\"advanced\" class=\"expander\">+</span>\n");
    html += UnicodeString("Advanced Options\n");
    html += UnicodeString("<div id=\"advanced.group\" class=\"itemGroup\">\n");
    html += UnicodeString("<table><tr><td>\n");
    html += UnicodeString("<form onsubmit=\"filterRows();return false;\" action=\"\">\n"); // return false prevents a submission
    html += UnicodeString("<fieldset><legend>Item Filtering</legend>\n");
    html += UnicodeString("<span id=\"filterResultText\">&nbsp;</span><br />\n");
    html += UnicodeString("<input type=\"text\" size=\"16\" id=\"filterText\" /> <button type=\"button\" onclick=\"filterRows();\">Filter Items</button><br />\n");
    html += UnicodeString("<button type=\"button\" onclick=\"selectFiltered(true)\">Select All</button>\n");
    html += UnicodeString("<button type=\"button\" onclick=\"selectFiltered(false)\">Deselect All</button><br />\n");
    html += UnicodeString("</fieldset>\n");
    html += UnicodeString("</form>\n");
    html += UnicodeString("<fieldset><legend>Groups</legend>\n");
    html += UnicodeString("<button type=\"button\" onclick=\"clickGroups('+')\">Expand All</button>\n");
    html += UnicodeString("<button type=\"button\" onclick=\"clickGroups('\\u2212')\">Contract All</button><br />\n");
    html += UnicodeString("</fieldset>\n");
    html += UnicodeString("</td></tr></table>\n");
    html += UnicodeString("</div><br /><br /><br />\n");
    insertTemplateFile(html, gHtmlFooter, NULL);
    html += UnicodeString("</body>\n");
    html += UnicodeString("</html>\n");
    return html;
}

static void
printHTML(const UnicodeString &outputText) {
    // Overestimate the size of the UTF-8 buffer.
    int32_t outputInitialLen = outputText.length()*sizeof(UChar);
    char *outputChars = (char *)malloc(outputInitialLen);
    int32_t len = outputText.extract(0, outputText.length(), outputChars, outputInitialLen, "UTF-8");
    if (len >= outputInitialLen) {
        // This normally shouldn't happen because most of the text is ASCII
        free(outputChars);
        outputChars = (char *)malloc(len+1);
        len = outputText.extract(0, outputText.length(), outputChars, len, "UTF-8");
    }
    outputChars[len] = 0;
    printf("%s", outputChars);
    free(outputChars);
}

static void
printUsage(const char *pname) {
    printf("The help text has not been written yet. Sorry.");
}

static UOption options[]={
    UOPTION_HELP_H,
    UOPTION_HELP_QUESTION_MARK,
    UOPTION_DEF("package", 'p', UOPT_REQUIRES_ARG),
    UOPTION_DEF("list-modifier", 'l', UOPT_REQUIRES_ARG),
    UOPTION_DEF("cgi-name", 'c', UOPT_REQUIRES_ARG),
    UOPTION_DEF("html-header", 'H', UOPT_REQUIRES_ARG),
    UOPTION_DEF("html-masthead", 'm', UOPT_REQUIRES_ARG),
    UOPTION_DEF("html-leftnav", 'n', UOPT_REQUIRES_ARG),
    UOPTION_DEF("html-footer", 'f', UOPT_REQUIRES_ARG),
};

enum {
    OPT_HELP_H,
    OPT_HELP_QUESTION_MARK,
    OPT_PACKAGE,
    OPT_LIST_MODIFIER,
    OPT_CGI_NAME,
    OPT_HTML_HEADER,
    OPT_HTML_MASTHEAD,
    OPT_HTML_LEFTNAV,
    OPT_HTML_FOOTER,
    OPT_COUNT
};

extern int
main(int argc, char *argv[]) {
    const char *pname, *inFilename;
    Package *pkg;
    UErrorCode status = U_ZERO_ERROR;

    /* get the program basename */
    pname=findBasename(argv[0]);

    argc=u_parseArgs(argc, argv, LENGTHOF(options), options);
    if(options[OPT_HELP_H].doesOccur || options[OPT_HELP_QUESTION_MARK].doesOccur) {
        printUsage(pname);
        return U_ZERO_ERROR;
    }
    if(!options[OPT_PACKAGE].doesOccur
        || !options[OPT_CGI_NAME].doesOccur
        || !options[OPT_HTML_HEADER].doesOccur
        || !options[OPT_HTML_MASTHEAD].doesOccur
        || !options[OPT_HTML_LEFTNAV].doesOccur
        || !options[OPT_HTML_FOOTER].doesOccur)
    {
        fprintf(stderr, "An argument is missing.\n");
        printUsage(pname);
        return U_ILLEGAL_ARGUMENT_ERROR;
    }
    gPackage=options[OPT_PACKAGE].value;
    gCgiName=options[OPT_CGI_NAME].value;
    gHtmlHeader=options[OPT_HTML_HEADER].value;
    gHtmlMasthead=options[OPT_HTML_MASTHEAD].value;
    gHtmlLeftNav=options[OPT_HTML_LEFTNAV].value;
    gHtmlFooter=options[OPT_HTML_FOOTER].value;

    udata_setAppData(DATADEP_PACKAGE, (const void*) datadep_dat, &status);

    u_init(&status);

    if(U_FAILURE(status)) {
        fprintf(stderr, "Error in u_init: %s.\n", u_errorName(status));
        return status;
    }

    hiddenItems = new UVector(NULL, uhash_compareChars, status);
    additionalItems = new UVector(freeBlock, compareItems, status);
    pkg = new Package;
    if(pkg==NULL || hiddenItems==NULL) {
        fprintf(stderr, "icupkg: not enough memory\n");
        return U_MEMORY_ALLOCATION_ERROR;
    }

    if (options[OPT_LIST_MODIFIER].doesOccur) {
        ListModifier::parseModificationList(options[OPT_LIST_MODIFIER].value, hiddenItems, additionalItems);
    }

    inFilename=gPackage;
    pkg->readPackage(inFilename);
    UnicodeString html(generateHTML(pkg, status));

    printHTML(html);

    delete hiddenItems;
    delete pkg;

    return 0;
}
