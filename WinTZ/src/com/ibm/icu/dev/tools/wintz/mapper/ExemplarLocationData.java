/*
 *******************************************************************************
 * Copyright (C) 2011, International Business Machines Corporation and         *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
package com.ibm.icu.dev.tools.wintz.mapper;

class ExemplarLocationData {

    // Exemplar location names are coming from English time zone display name.
    // For example, the English display name for Windows TZ "SA Western Standard Time"
    // is "(UTC-04:00) Georgetown, La Paz, Manaus, San Juan", which includes
    // four exemplar locations. Many of these locations have direct 1-to-1 mapping
    // in the tz database. In this example;
    //
    //   Georgetown     -> America/Guyana
    //   La Paz         -> America/La_Paz
    //   Manaus         -> America/Manaus
    //   San Juan       -> America/Puerto_Rico
    //
    // Microsoft sometimes split an existing zone into two. For example, one city
    // in a group needs to use different rule from others, a new Windows zone is
    // is created and the city is moved to the zone. Even for such event, we can
    // track the change programmatically if the name of the location is preserved.
    //
    // Note:
    //   - The 1st element in each sub-array is a location name extracted from
    //     English time zone display name. The 2nd element is the corresponding
    //     tz database zone ID.
    //
    //   - Some locations do not have direct 1-to-1 mapping. In this case, the 2nd
    //     element is empty string.
    //
    //   - When a same name appear in multiple Windows time zones, the 2nd element
    //     is kept empty. For now, there is only one instance in this class -
    //     "La Paz" in "(UTC-07:00) Chihuahua, La Paz, Mazatlan" and
    //     "(UTC-04:00) Georgetown, La Paz, Manaus, San Juan"
    //
    static final String[][] EXEMPLAR_LOCATIONS = {
        {"Abu Dhabi", "Asia/Dubai"},
        {"Adelaide", "Australia/Adelaide"},
        {"Alaska", "America/Anchorage"},
        {"Amman", "Asia/Amman"},
        {"Amsterdam", "Europe/Amsterdam"},
        {"Arizona", "America/Phoenix"},
        {"Astana", "Asia/Almaty"},
        {"Asuncion", "America/Asuncion"},
        {"Athens", "Europe/Athens"},
        {"Auckland", "Pacific/Auckland"},
        {"Azores", "Atlantic/Azores"},
        {"Baghdad", "Asia/Baghdad"},
        {"Baja California", ""},
        {"Baku", "Asia/Baku"},
        {"Bangkok", "Asia/Bangkok"},
        {"Beijing", "Asia/Shanghai"},
        {"Beirut", "Asia/Beirut"},
        {"Belgrade", "Europe/Belgrade"},
        {"Berlin", "Europe/Berlin"},
        {"Bern", "Europe/Zurich"},
        {"Bogota", "America/Bogota"},
        {"Brasilia", "America/Sao_Paulo"},
        {"Bratislava", "Europe/Bratislava"},
        {"Brisbane", "Australia/Brisbane"},
        {"Brussels", "Europe/Brussels"},
        {"Bucharest", "Europe/Bucharest"},
        {"Budapest", "Europe/Budapest"},
        {"Buenos Aires", "America/Buenos_Aires"},
        {"Cairo", "Africa/Cairo"},
        {"Canberra", ""},
        {"Cape Verde Is.", "Atlantic/Cape_Verde"},
        {"Caracas", "America/Caracas"},
        {"Casablanca", "Africa/Casablanca"},
        {"Cayenne", "America/Cayenne"},
        {"Chennai", ""},
        {"Chihuahua", "America/Chihuahua"},
        {"Chongqing", "Asia/Chongqing"},
        {"Copenhagen", "Europe/Copenhagen"},
        {"Cuiaba", "America/Cuiaba"},
        {"Damascus", "Asia/Damascus"},
        {"Darwin", "Australia/Darwin"},
        {"Dhaka", "Asia/Dhaka"},
        {"Dublin", "Europe/Dublin"},
        {"Edinburgh", ""},
        {"Ekaterinburg", "Asia/Yekaterinburg"},
        {"Fiji", "Pacific/Fiji"},
        {"Fortaleza", "America/Fortaleza"},
        {"Georgetown", "America/Guyana"},
        {"Greenland", "America/Godthab"},
        {"Guadalajara", ""},
        {"Guam", "Pacific/Guam"},
        {"Hanoi", "Asia/Ho_Chi_Minh"},
        {"Harare", "Africa/Harare"},
        {"Hawaii", "Pacific/Honolulu"},
        {"Helsinki", "Europe/Helsinki"},
        {"Hobart", "Australia/Hobart"},
        {"Hong Kong", "Asia/Hong_Kong"},
        {"Indiana (East)", "America/Indianapolis"},
        {"Irkutsk", "Asia/Irkutsk"},
        {"Islamabad", ""},
        {"Istanbul", "Europe/Istanbul"},
        {"Jakarta", "Asia/Jakarta"},
        {"Jerusalem", "Asia/Jerusalem"},
        {"Kabul", "Asia/Kabul"},
        {"Kaliningrad", "Europe/Kaliningrad"},
        {"Karachi", "Asia/Karachi"},
        {"Kathmandu", "Asia/Kathmandu"},
        {"Kolkata", "Asia/Kolkata"},
        {"Krasnoyarsk", "Asia/Krasnoyarsk"},
        {"Kuala Lumpur", "Asia/Kuala_Lumpur"},
        {"Kuwait", "Asia/Kuwait"},
        {"Kyiv", "Europe/Kiev"},
        {"La Paz", ""}, // Duplicate - La Paz, Mexico and La Paz, Bolivia
        {"Lima", "America/Lima"},
        {"Lisbon", "Europe/Lisbon"},
        {"Ljubljana", "Europe/Ljubljana"},
        {"London", "Europe/London"},
        {"Madrid", "Europe/Madrid"},
        {"Magadan", "Asia/Magadan"},
        {"Manaus", "America/Manaus"},
        {"Mazatlan", "America/Mazatlan"},
        {"Melbourne", "Australia/Melbourne"},
        {"Mexico City", "America/Mexico_City"},
        {"Minsk", "Europe/Minsk"},
        {"Monrovia", "Africa/Monrovia"},
        {"Monterrey", "America/Monterrey"},
        {"Montevideo", "America/Montevideo"},
        {"Moscow", "Europe/Moscow"},
        {"Mumbai", ""},
        {"Muscat", "Asia/Muscat"},
        {"Nairobi", "Africa/Nairobi"},
        {"New Caledonia", "Pacific/Noumea"},
        {"New Delhi", ""},
        {"Newfoundland", "America/St_Johns"},
        {"Nicosia", "Asia/Nicosia"},
        {"Novosibirsk", "Asia/Novosibirsk"},
        {"Nuku'alofa", "Pacific/Tongatapu"},
        {"Osaka", ""},
        {"Paris", "Europe/Paris"},
        {"Perth", "Australia/Perth"},
        {"Port Louis", "Indian/Mauritius"},
        {"Port Moresby", "Pacific/Port_Moresby"},
        {"Prague", "Europe/Prague"},
        {"Pretoria", "Africa/Johannesburg"},
        {"Quito", "America/Guayaquil"},
        {"Reykjavik", "Atlantic/Reykjavik"},
        {"Riga", "Europe/Riga"},
        {"Riyadh", "Asia/Riyadh"},
        {"Rome", "Europe/Rome"},
        {"Salvador", "America/Bahia"},
        {"Samoa", "Pacific/Apia"},
        {"San Juan", "America/Puerto_Rico"},
        {"Santiago", "America/Santiago"},
        {"Sapporo", ""},
        {"Sarajevo", "Europe/Sarajevo"},
        {"Saskatchewan", "America/Regina"},
        {"Seoul", "Asia/Seoul"},
        {"Singapore", "Asia/Singapore"},
        {"Skopje", "Europe/Skopje"},
        {"Sofia", "Europe/Sofia"},
        {"Solomon Is.", "Pacific/Guadalcanal"},
        {"Sri Jayawardenepura", "Asia/Colombo"},
        {"St. Petersburg", ""},
        {"Stockholm", "Europe/Stockholm"},
        {"Sydney", "Australia/Sydney"},
        {"Taipei", "Asia/Taipei"},
        {"Tallinn", "Europe/Tallinn"},
        {"Tashkent", "Asia/Tashkent"},
        {"Tbilisi", "Asia/Tbilisi"},
        {"Tehran", "Asia/Tehran"},
        {"Tokyo", "Asia/Tokyo"},
        {"Ulaanbaatar", "Asia/Ulaanbaatar"},
        {"Urumqi", "Asia/Urumqi"},
        {"Vienna", "Europe/Vienna"},
        {"Vilnius", "Europe/Vilnius"},
        {"Vladivostok", "Asia/Vladivostok"},
        {"Volgograd", "Europe/Volgograd"},
        {"Warsaw", "Europe/Warsaw"},
        {"Wellington", ""},
        {"Windhoek", "Africa/Windhoek"},
        {"Yakutsk", "Asia/Yakutsk"},
        {"Yangon (Rangoon)", "Asia/Rangoon"},
        {"Yerevan", "Asia/Yerevan"},
        {"Zagreb", "Europe/Zagreb"},
    };

    // Not all Windows time zone display names contain exemplar location names.
    static final String[] NON_LOCATIONS = {
        "Atlantic Time (Canada)",
        "Central America",
        "Central Time (US & Canada)",
        "International Date Line West",
        "Eastern Time (US & Canada)",
        "Mid-Atlantic",
        "Mountain Time (US & Canada)",
        "Pacific Time (US & Canada)",
        "West Central Africa",
    };
}
