/**
 * General utility functions.
 * @author Alan Liu
 */

IS_IE = navigator.appName.indexOf("Microsoft") != -1;

/**
 * onKeyPress handler for text fields.  By setting the key press
 * handler of a field to the function returned by this object, you can
 * trap ENTER key presses.  Use it like this:
 *
 *   document.FORM.ID.onkeypress = trapEnter(foo);
 *
 * causes the ENTER key pressed in the FORM.ID field to call foo() and
 * NOT submit the form.  The handler may submit the form explicitly if
 * desired.
 */
function trapEnter(func, context) {
    return function(e) {
        // Get the Event object
        keyCode = IS_IE ? window.event.keyCode : e.which;
        if (keyCode == 13) {
            // Call user-supplied hook
            func(context);
            // Disable further handling of this event
            if (IS_IE) {
                window.event.returnValue = false;
            } else {
                return false;
            }
        }
    };
}

// Magic value indicating a cookie has been deleted. This is used
// in case the browser doesn't delete the cookie immediately.
DELETED_COOKIE = '__DELETED_COOKIE__';

/**
 * Set a cookie.  Both key and value will be escaped (may contain
 * special characters).
 * @param key string name of the cookie to set
 * @param value the string value
 * @param expiration optional number of DAYS before expiration
 */
function setCookie(key, value, expiration) {
    var exp = "";
    if (expiration) {
        exp = new Date();
        exp = new Date(exp.getTime() + expiration*24*3600000);
        exp = ";expires=" + exp.toGMTString();
    }
    document.cookie = escape(key) + "=" + escape(value) + exp;
}

/**
 * Delete a cookie.
 */
function deleteCookie(key) {
    setCookie(key, DELETED_COOKIE, -1);
}

/**
 * Get a cookie.
 * @param key the string name of the cookie to retrieve
 * @return value associated with this key (cookie value) or null.
 */
function getCookie(key) {
    var allCookies = document.cookie;
    var value = null;
    key = escape(key) + "=";
    var start = allCookies.indexOf(key);
    if (start != -1) {
        start += key.length;
        var end = allCookies.indexOf(";", start);
        if (end == -1) end = allCookies.length;
        value = unescape(allCookies.substring(start, end));
        if (value == DELETED_COOKIE) value = null;
    }
    return value;
}

/**
 * Get all available cookie keys.
 * @return an array of cookie keys
 */
function getCookieKeys() {
    var allCookies = document.cookie;
    var keys = new Array();
    var pos = 0;
    do {
        while (pos < allCookies.length && allCookies.charAt(pos) == " ") ++pos;
        var eq = allCookies.indexOf("=", pos);
        if (eq == -1) break;
        var end = allCookies.indexOf(";", eq);
        if (end == -1) end = allCookies.length;
        if (allCookies.substring(eq+1, end) != DELETED_COOKIE) {
            keys.push(unescape(allCookies.substring(pos, eq)));
        }
        pos = end+1;
    } while (pos < allCookies.length);
    return keys;
}

/**
 * Set the options in a SELECT element to the given array
 * of strings.
 * @param select select object
 * @param options array of strings
 */
function setOptions(select, options) {
    var a = select.options;
    var i;
    var j = 0;
    for (i=0; i<a.length && j<options.length; ++i) {
        if (a[i] == null) continue;
        a[i].text = a[i].value = options[j];
        ++j;
    }
    while (j < options.length) {
        var opt = new Option(options[j], options[j]);
        a.length = i+1;
        a[i++] = opt;
        ++j;
    }
    a.length = i;
}

/**
 * Get the value of the option currently selected by a SELECT element,
 * or if the value is empty, the text.
 */
function getSelectedOption(select) {
    var o = select.options[select.selectedIndex];
    return o.value ? o.value : o.text;
}

/**
 * Given option text and the selection for this element,
 * emit an <option> element.
 */
function _generateOption(text, selection) {
  document.write(text == selection ? "<option selected>" : "<option>");
  document.write(text);
  document.writeln("</option>");
}

/**
 * Generate the options list dynamically.  The other way to do this is
 * to modify the SELECT object with setOptions(), but that causes too
 * many visible updates.  This way, the user doesn't see any screen
 * flashing.
 * @param options array of strings
 * @param selection current selection (string), should be == one of
 * options[i]
 */
function generateOptions(options, selection) {
  for (i=0; i<options.length; ++i) {
    _generateOption(options[i], selection);
  }
}

/**
 * Trim the whitespace from a string.  This trims either leading
 * and trailing whitespace, or if 'all' is true, all whitespace.
 * @param all if true, trim all whitespace.  Otherwise only leading
 * and trailing.
 */
function trimSpaces(str, all) {
    if (all) {
        return str.replace(/\s+/g, "");
    }
    str = str.replace(/^\s+/, "");
    return str.replace(/\s+$/, "");
}

/**
 * A case-insensitive sort function for strings.  Too bad there
 * is no cmp comparator in JavaScript.
 */
function ciSort(a, b) {
    var la = a.toLowerCase();
    var lb = b.toLowerCase();
    return (la > lb) ? 1 : ((la < lb) ? -1 : 0);
}

/**
 * Return true if the given array contains the given string.
 * If ignoreCase is true, then ignore case.
 */
function arrayContainsString(a, s, ignoreCase) {
    var i;
    if (ignoreCase) {
        s = s.toLowerCase();
        for (i=0; i<a.length; ++i) {
            if (s == a[i].toLowerCase()) return true;
        }
    } else {
        for (i=0; i<a.length; ++i) {
            if (s == a[i]) return true;
        }
    }
    return false;
}
