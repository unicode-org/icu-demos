/*
 *******************************************************************************
 * Copyright (C) 2011-2013, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
package com.ibm.icu.dev.tools.wintz.mapper;

public class MapData {
    static final String TZDATA_VERSION = "2013d";
    static final String WINTZDATA_VERSION = "7dd0003";

    static final String DEFAULT = "D";
    static final String REGION_DEFAULT = "R";

    /*
     * Raw Windows ID - region - zones mapping data
     * 
     * Rules:
     *   - Each entry has either length of 2 or 3.
     *   - 1st value is Windows zone ID.
     *   - 2nd value is region code (ISO 3166) from zone.tab in the tz database
     *   - 3rd value (optional) indicates if the Olson zone is default for Windows zone (DEFAULT), or default for Windows zone and region (REGION_DEFAULT).
     *   - Each Windows zone must have exactly one DEFAULT Olson zone mapping.
     *   - Each unique combination of a Windows zone and a region must have exactly one REGION_DEFAULT or DEFAULT zone
     */
    static final String[][] MAP_DATA_ARRAY = {
        // --------------------------------------------------------------------
        // (UTC-12:00) International Date Line West
        {"Dateline Standard Time", "ZZ", "Etc/GMT+12", DEFAULT},


        // --------------------------------------------------------------------
        // (UTC-11:00) Coordinated Universal Time-11
        {"UTC-11", "AS", "Pacific/Pago_Pago", REGION_DEFAULT},

        {"UTC-11", "NU", "Pacific/Niue", REGION_DEFAULT},

        {"UTC-11", "UM", "Pacific/Midway", REGION_DEFAULT},

        {"UTC-11", "ZZ", "Etc/GMT+11", DEFAULT},


        // --------------------------------------------------------------------
        // (UTC-10:00) Hawaii
        {"Hawaiian Standard Time", "US", "Pacific/Honolulu", DEFAULT}, // *Hawaii

        {"Hawaiian Standard Time", "UM", "Pacific/Johnston", REGION_DEFAULT},

        // This is the only Windows time zone with UTC offset -10:00 without DST
        {"Hawaiian Standard Time", "ZZ", "Etc/GMT+10", REGION_DEFAULT},

        // Following locations don't belong to Hawaii. Because this is only
        // the Windows time zone with UTC-10:00/no DST, they cannot use anything else.
        {"Hawaiian Standard Time", "CK", "Pacific/Rarotonga", REGION_DEFAULT},

        {"Hawaiian Standard Time", "PF", "Pacific/Tahiti", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC-09:00) Alaska
        {"Alaskan Standard Time", "US", "America/Anchorage", DEFAULT}, // *Alaska
        {"Alaskan Standard Time", "US", "America/Juneau"},
        {"Alaskan Standard Time", "US", "America/Nome"},
        {"Alaskan Standard Time", "US", "America/Sitka"},
        {"Alaskan Standard Time", "US", "America/Yakutat"},


        // --------------------------------------------------------------------
        // (UTC-08:00) Pacific Time (US & Canada)
        {"Pacific Standard Time", "CA", "America/Vancouver", REGION_DEFAULT},
        {"Pacific Standard Time", "CA", "America/Dawson"},
        {"Pacific Standard Time", "CA", "America/Whitehorse"},

        {"Pacific Standard Time", "MX", "America/Tijuana", REGION_DEFAULT},

        {"Pacific Standard Time", "US", "America/Los_Angeles", DEFAULT},

        {"Pacific Standard Time", "ZZ", "PST8PDT", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC-08:00) Baja California
        {"Pacific Standard Time (Mexico)", "MX", "America/Santa_Isabel", DEFAULT},


        // --------------------------------------------------------------------
        // (UTC-07:00) Mountain Time (US & Canada)
        {"Mountain Standard Time", "CA", "America/Edmonton", REGION_DEFAULT},
        {"Mountain Standard Time", "CA", "America/Cambridge_Bay"},
        {"Mountain Standard Time", "CA", "America/Inuvik"},
        {"Mountain Standard Time", "CA", "America/Yellowknife"},

        {"Mountain Standard Time", "MX", "America/Ojinaga", REGION_DEFAULT},

        {"Mountain Standard Time", "US", "America/Boise"},
        {"Mountain Standard Time", "US", "America/Denver", DEFAULT},
        {"Mountain Standard Time", "US", "America/Shiprock"},

        {"Mountain Standard Time", "ZZ", "MST7MDT", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC-07:00) Chihuahua, La Paz, Mazatlan
        {"Mountain Standard Time (Mexico)", "MX", "America/Chihuahua", DEFAULT}, // Chihuahua
        {"Mountain Standard Time (Mexico)", "MX", "America/Mazatlan"}, // Mazatlan


        // --------------------------------------------------------------------
        // (UTC-07:00) Arizona
        {"US Mountain Standard Time", "CA", "America/Dawson_Creek", REGION_DEFAULT},
        {"US Mountain Standard Time", "CA", "America/Creston"},

        {"US Mountain Standard Time", "MX", "America/Hermosillo", REGION_DEFAULT},

        {"US Mountain Standard Time", "US", "America/Phoenix", DEFAULT}, // *Arizona

        // This is the only Windows time zone with UTC offset -07:00 without DST
        {"US Mountain Standard Time", "ZZ", "Etc/GMT+7", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC-06:00) Saskatchewan
        {"Canada Central Standard Time", "CA", "America/Regina", DEFAULT}, // *Saskatchewan
        {"Canada Central Standard Time", "CA", "America/Swift_Current"},


        // --------------------------------------------------------------------
        // (UTC-06:00) Central America
        {"Central America Standard Time", "BZ", "America/Belize", REGION_DEFAULT},

        {"Central America Standard Time", "CR", "America/Costa_Rica", REGION_DEFAULT},

        {"Central America Standard Time", "SV", "America/El_Salvador", REGION_DEFAULT},

        {"Central America Standard Time", "GT", "America/Guatemala", DEFAULT},

        {"Central America Standard Time", "NI", "America/Managua", REGION_DEFAULT},

        {"Central America Standard Time", "HN", "America/Tegucigalpa", REGION_DEFAULT},

        {"Central America Standard Time", "EC", "Pacific/Galapagos", REGION_DEFAULT},

        // There are two Windows time zones with UTC offset -06:00 without DST for now.
        // Another one is (UTC-06:00) Saskatchewan. Because the coverage of this zone is
        // broader, we'll use this zone as default UTC-06:00/no DST zone.
        {"Central America Standard Time", "ZZ", "Etc/GMT+6", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC-06:00) Central Time (US & Canada)
        {"Central Standard Time", "CA", "America/Rainy_River"},
        {"Central Standard Time", "CA", "America/Rankin_Inlet"},
        {"Central Standard Time", "CA", "America/Resolute"},
        {"Central Standard Time", "CA", "America/Winnipeg", REGION_DEFAULT},

        {"Central Standard Time", "MX", "America/Matamoros", REGION_DEFAULT},

        {"Central Standard Time", "US", "America/Chicago", DEFAULT},
        {"Central Standard Time", "US", "America/Indiana/Knox"},
        {"Central Standard Time", "US", "America/Indiana/Tell_City"},
        {"Central Standard Time", "US", "America/Menominee"},
        {"Central Standard Time", "US", "America/North_Dakota/Center"},
        {"Central Standard Time", "US", "America/North_Dakota/New_Salem"},
        {"Central Standard Time", "US", "America/North_Dakota/Beulah"},

        {"Central Standard Time", "ZZ", "CST6CDT", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC-06:00) Guadalajara, Mexico City, Monterrey
        {"Central Standard Time (Mexico)", "MX", "America/Bahia_Banderas"},
        {"Central Standard Time (Mexico)", "MX", "America/Cancun"},
        {"Central Standard Time (Mexico)", "MX", "America/Merida"},
        {"Central Standard Time (Mexico)", "MX", "America/Mexico_City", DEFAULT}, // Mexico City
        {"Central Standard Time (Mexico)", "MX", "America/Monterrey"}, // Monterrey


        // --------------------------------------------------------------------
        // (UTC-05:00) Eastern Time (US & Canada)
        {"Eastern Standard Time", "BS", "America/Nassau", REGION_DEFAULT},

        {"Eastern Standard Time", "CA", "America/Iqaluit"},
        {"Eastern Standard Time", "CA", "America/Montreal"},
        {"Eastern Standard Time", "CA", "America/Nipigon"},
        {"Eastern Standard Time", "CA", "America/Pangnirtung"},
        {"Eastern Standard Time", "CA", "America/Thunder_Bay"},
        {"Eastern Standard Time", "CA", "America/Toronto", REGION_DEFAULT},

        {"Eastern Standard Time", "HT", "America/Port-au-Prince", REGION_DEFAULT},

        {"Eastern Standard Time", "TC", "America/Grand_Turk", REGION_DEFAULT},

        {"Eastern Standard Time", "US", "America/Detroit"},
        {"Eastern Standard Time", "US", "America/Indiana/Petersburg"},
        {"Eastern Standard Time", "US", "America/Indiana/Vincennes"},
        {"Eastern Standard Time", "US", "America/Indiana/Winamac"},
        {"Eastern Standard Time", "US", "America/Kentucky/Monticello"},
        {"Eastern Standard Time", "US", "America/Louisville"},
        {"Eastern Standard Time", "US", "America/New_York", DEFAULT},

        {"Eastern Standard Time", "ZZ", "EST5EDT", REGION_DEFAULT},

        // Starting in 2013, Cuba's DST start/end dates are same with
        // US. Only the difference is time of the transition date
        // (midnight in standard time for Cuba vs 2 o'clock in wall time
        // for US).
        {"Eastern Standard Time", "CU", "America/Havana", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC-05:00) Bogota, Lima, Quito
        {"SA Pacific Standard Time", "CO", "America/Bogota", DEFAULT}, // Bogota

        {"SA Pacific Standard Time", "PE", "America/Lima", REGION_DEFAULT}, // Lima

        {"SA Pacific Standard Time", "EC", "America/Guayaquil", REGION_DEFAULT}, // *Quito

        // This is the only Windows time zone with UTC offset -05:00 without DST
        {"SA Pacific Standard Time", "ZZ", "Etc/GMT+5", REGION_DEFAULT},

        // Following zones are not South American Pacific regions. Because this is only
        // the Windows time zone with UTC-05:00/no DST, they cannot use anything else.
        {"SA Pacific Standard Time", "CA", "America/Coral_Harbour", REGION_DEFAULT},

        {"SA Pacific Standard Time", "JM", "America/Jamaica", REGION_DEFAULT},

        {"SA Pacific Standard Time", "KY", "America/Cayman", REGION_DEFAULT},

        {"SA Pacific Standard Time", "PA", "America/Panama", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC-05:00) Indiana (East)
        {"US Eastern Standard Time", "US", "America/Indianapolis", DEFAULT}, // *Indiana (East)
        {"US Eastern Standard Time", "US", "America/Indiana/Marengo"},
        {"US Eastern Standard Time", "US", "America/Indiana/Vevay"},


        // --------------------------------------------------------------------
        // (UTC-04:30) Caracas
        {"Venezuela Standard Time", "VE", "America/Caracas", DEFAULT}, // Caracas


        // --------------------------------------------------------------------
        // (UTC-04:00) Atlantic Time (Canada)
        {"Atlantic Standard Time", "BM", "Atlantic/Bermuda", REGION_DEFAULT},

        {"Atlantic Standard Time", "CA", "America/Glace_Bay"},
        {"Atlantic Standard Time", "CA", "America/Goose_Bay"},
        {"Atlantic Standard Time", "CA", "America/Halifax", DEFAULT},
        {"Atlantic Standard Time", "CA", "America/Moncton"},

        {"Atlantic Standard Time", "GL", "America/Thule", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC-04:00) Cuiaba
        {"Central Brazilian Standard Time", "BR", "America/Campo_Grande"},
        {"Central Brazilian Standard Time", "BR", "America/Cuiaba", DEFAULT}, // Cuiaba


        // --------------------------------------------------------------------
        // (UTC-04:00) Santiago
        {"Pacific SA Standard Time", "CL", "America/Santiago", DEFAULT}, // Santiago

        // According to the description in antarctica file, Palmer station keeps the same time
        // as Puta Arenas, Chile. The rule ChileAQ seems is out of sync with the rule Chile in
        // southamerica file in 2011n. This is probably a bug in the tz database. For now,
        // we assume Santiago and Palmer station are using the same time zone.
        // [Update] 2012a updated Rule ChileAQ synchronized with Rule Chile.
        {"Pacific SA Standard Time", "AQ", "Antarctica/Palmer", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC-04:00) Asuncion
        {"Paraguay Standard Time", "PY", "America/Asuncion", DEFAULT}, // Asuncion


        // --------------------------------------------------------------------
        // (UTC-04:00) Georgetown, La Paz, Manaus, San Juan
        {"SA Western Standard Time", "BO", "America/La_Paz", DEFAULT}, // La Paz

        {"SA Western Standard Time", "BR", "America/Boa_Vista"},
        {"SA Western Standard Time", "BR", "America/Eirunepe"},
        {"SA Western Standard Time", "BR", "America/Manaus", REGION_DEFAULT}, // Manaus
        {"SA Western Standard Time", "BR", "America/Porto_Velho"},
        {"SA Western Standard Time", "BR", "America/Rio_Branco"},

        {"SA Western Standard Time", "GY", "America/Guyana", REGION_DEFAULT}, // *Georgetown

        {"SA Western Standard Time", "PR", "America/Puerto_Rico", REGION_DEFAULT}, // *San Juan - Note: There are many locations named San Juan. It's most likely San Juan, Puerto Rico.

        // Caribbean territories are using UTC-4 Atlantic Time without daylight saving
        // time. Windows does not have the dedicated zone for Atlantic Standard Time (no DST).
        // I assume Microsoft simply use this zone for all locations using UTC-4 without DST.
        // Followings are locations using AST.
        {"SA Western Standard Time", "AI", "America/Anguilla", REGION_DEFAULT},

        {"SA Western Standard Time", "AG", "America/Antigua", REGION_DEFAULT},

        {"SA Western Standard Time", "AW", "America/Aruba", REGION_DEFAULT},

        {"SA Western Standard Time", "BB", "America/Barbados", REGION_DEFAULT},

        {"SA Western Standard Time", "BL", "America/St_Barthelemy", REGION_DEFAULT},

        {"SA Western Standard Time", "BQ", "America/Kralendijk", REGION_DEFAULT},

        {"SA Western Standard Time", "CA", "America/Blanc-Sablon", REGION_DEFAULT},

        {"SA Western Standard Time", "CW", "America/Curacao", REGION_DEFAULT},

        {"SA Western Standard Time", "DM", "America/Dominica", REGION_DEFAULT},

        {"SA Western Standard Time", "DO", "America/Santo_Domingo", REGION_DEFAULT},

        {"SA Western Standard Time", "GD", "America/Grenada", REGION_DEFAULT},

        {"SA Western Standard Time", "GP", "America/Guadeloupe", REGION_DEFAULT},

        {"SA Western Standard Time", "LC", "America/St_Lucia", REGION_DEFAULT},

        {"SA Western Standard Time", "KN", "America/St_Kitts", REGION_DEFAULT},

        {"SA Western Standard Time", "MF", "America/Marigot", REGION_DEFAULT},

        {"SA Western Standard Time", "MQ", "America/Martinique", REGION_DEFAULT},

        {"SA Western Standard Time", "MS", "America/Montserrat", REGION_DEFAULT},

        {"SA Western Standard Time", "SX", "America/Lower_Princes", REGION_DEFAULT},

        {"SA Western Standard Time", "TT", "America/Port_of_Spain", REGION_DEFAULT},

        {"SA Western Standard Time", "VC", "America/St_Vincent", REGION_DEFAULT},

        {"SA Western Standard Time", "VG", "America/Tortola", REGION_DEFAULT},

        {"SA Western Standard Time", "VI", "America/St_Thomas", REGION_DEFAULT},

        // This is the only Windows time zone with UTC offset -04:00 without DST
        {"SA Western Standard Time", "ZZ", "Etc/GMT+4", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC-03:30) Newfoundland
        {"Newfoundland Standard Time", "CA", "America/St_Johns", DEFAULT}, // *Newfoundland


        // --------------------------------------------------------------------
        // (UTC-03:00) Buenos Aires
        {"Argentina Standard Time", "AR", "America/Argentina/La_Rioja"},
        {"Argentina Standard Time", "AR", "America/Argentina/Rio_Gallegos"},
        {"Argentina Standard Time", "AR", "America/Argentina/Salta"},
        {"Argentina Standard Time", "AR", "America/Argentina/San_Juan"},
        {"Argentina Standard Time", "AR", "America/Argentina/San_Luis"},
        {"Argentina Standard Time", "AR", "America/Argentina/Tucuman"},
        {"Argentina Standard Time", "AR", "America/Argentina/Ushuaia"},
        {"Argentina Standard Time", "AR", "America/Buenos_Aires", DEFAULT}, // Buenos Aires
        {"Argentina Standard Time", "AR", "America/Catamarca"},
        {"Argentina Standard Time", "AR", "America/Cordoba"},
        {"Argentina Standard Time", "AR", "America/Jujuy"},
        {"Argentina Standard Time", "AR", "America/Mendoza"},


        // --------------------------------------------------------------------
        // (UTC-03:00) Salvador
        {"Bahia Standard Time", "BR", "America/Bahia", DEFAULT}, // *Salvador

        // --------------------------------------------------------------------
        // (UTC-03:00) Brasilia
        {"E. South America Standard Time", "BR", "America/Araguaina"},
        {"E. South America Standard Time", "BR", "America/Sao_Paulo", DEFAULT}, // *Brasilia


        // --------------------------------------------------------------------
        // (UTC-03:00) Greenland
        {"Greenland Standard Time", "GL", "America/Godthab", DEFAULT}, // *Greenland


        // --------------------------------------------------------------------
        // (UTC-03:00) Montevideo
        {"Montevideo Standard Time", "UY", "America/Montevideo", DEFAULT}, // Montevideo


        // --------------------------------------------------------------------
        // (UTC-03:00) Cayenne, Fortaleza
        {"SA Eastern Standard Time", "BR", "America/Belem"},
        {"SA Eastern Standard Time", "BR", "America/Fortaleza", REGION_DEFAULT}, // Fortaleza
        {"SA Eastern Standard Time", "BR", "America/Maceio"},
        {"SA Eastern Standard Time", "BR", "America/Recife"},
        {"SA Eastern Standard Time", "BR", "America/Santarem"},

        {"SA Eastern Standard Time", "FK", "Atlantic/Stanley", REGION_DEFAULT},

        {"SA Eastern Standard Time", "GF", "America/Cayenne", DEFAULT}, // Cayenne

        {"SA Eastern Standard Time", "SR", "America/Paramaribo", REGION_DEFAULT},

        // There are two Windows time zones with UTC offset -03:00 without DST for now.
        // Another one is (UTC-03:00) Buenos Aires. Until recently Buenos Aires used to
        // observe DST, and Cayenne/Fortaleze did not observe DST for longer time,
        // we'll use this zone as default UTC-03:00/no DST zone.
        {"SA Eastern Standard Time", "ZZ", "Etc/GMT+3", REGION_DEFAULT},

        // Rothera station is in Antarctica. The zone is listed here just because we use
        // this zone as the default UTC-03:00/no DST zone.
        {"SA Eastern Standard Time", "AQ", "Antarctica/Rothera", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC-02:00) Mid-Atlantic


        // --------------------------------------------------------------------
        // (UTC-02:00) Coordinated Universal Time-02
        {"UTC-02", "BR", "America/Noronha", REGION_DEFAULT},

        {"UTC-02", "GS", "Atlantic/South_Georgia", REGION_DEFAULT},

        {"UTC-02", "ZZ", "Etc/GMT+2", DEFAULT},

        // --------------------------------------------------------------------
        // (UTC-01:00) Azores
        {"Azores Standard Time", "PT", "Atlantic/Azores", DEFAULT}, // Azores

        // Scoresbysund currently uses the exact same rule (UTC-01:00 / EU's DST rule)
        // with Azores.
        {"Azores Standard Time", "GL", "America/Scoresbysund", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC-01:00) Cape Verde Is.
        {"Cape Verde Standard Time", "CV", "Atlantic/Cape_Verde", DEFAULT}, // Cape Verde Is.

        // This is the only Windows time zone with UTC offset -01:00 without DST
        {"Cape Verde Standard Time", "ZZ", "Etc/GMT+1", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC) Dublin, Edinburgh, Lisbon, London
        {"GMT Standard Time", "ES", "Atlantic/Canary", REGION_DEFAULT},

        {"GMT Standard Time", "GB", "Europe/London", DEFAULT}, // London

        {"GMT Standard Time", "GG", "Europe/Guernsey", REGION_DEFAULT},

        {"GMT Standard Time", "FO", "Atlantic/Faeroe", REGION_DEFAULT},

        {"GMT Standard Time", "IE", "Europe/Dublin", REGION_DEFAULT}, // Dublin

        {"GMT Standard Time", "IM", "Europe/Isle_of_Man", REGION_DEFAULT},

        {"GMT Standard Time", "JE", "Europe/Jersey", REGION_DEFAULT},

        {"GMT Standard Time", "PT", "Atlantic/Madeira"},
        {"GMT Standard Time", "PT", "Europe/Lisbon", REGION_DEFAULT}, // Lisbon


        // --------------------------------------------------------------------
        // (UTC) Monrovia, Reykjavik
        {"Greenwich Standard Time", "BF", "Africa/Ouagadougou", REGION_DEFAULT},

        {"Greenwich Standard Time", "CI", "Africa/Abidjan", REGION_DEFAULT},

        {"Greenwich Standard Time", "GH", "Africa/Accra", REGION_DEFAULT},

        {"Greenwich Standard Time", "GM", "Africa/Banjul", REGION_DEFAULT},

        {"Greenwich Standard Time", "GN", "Africa/Conakry", REGION_DEFAULT},

        {"Greenwich Standard Time", "GW", "Africa/Bissau", REGION_DEFAULT},

        {"Greenwich Standard Time", "IS", "Atlantic/Reykjavik", DEFAULT}, // Reykjavik

        {"Greenwich Standard Time", "LR", "Africa/Monrovia", REGION_DEFAULT}, // Monrovia

        {"Greenwich Standard Time", "ML", "Africa/Bamako", REGION_DEFAULT},

        {"Greenwich Standard Time", "MR", "Africa/Nouakchott", REGION_DEFAULT},

        {"Greenwich Standard Time", "SH", "Atlantic/St_Helena", REGION_DEFAULT},

        {"Greenwich Standard Time", "SL", "Africa/Freetown", REGION_DEFAULT},

        {"Greenwich Standard Time", "SN", "Africa/Dakar", REGION_DEFAULT},

        {"Greenwich Standard Time", "ST", "Africa/Sao_Tome", REGION_DEFAULT},

        {"Greenwich Standard Time", "TG", "Africa/Lome", REGION_DEFAULT},

        // Al Aaiun's time zone is actually WET/no DST
        {"Greenwich Standard Time", "EH", "Africa/El_Aaiun", REGION_DEFAULT},

        
        // --------------------------------------------------------------------
        // (UTC) Casablanca
        {"Morocco Standard Time", "MA", "Africa/Casablanca", DEFAULT},


        // --------------------------------------------------------------------
        // (UTC) Coordinated Universal Time
        {"UTC", "ZZ", "Etc/GMT", DEFAULT},

        {"UTC", "GL", "America/Danmarkshavn", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC+01:00) Belgrade, Bratislava, Budapest, Ljubljana, Prague
        {"Central Europe Standard Time", "AL", "Europe/Tirane", REGION_DEFAULT}, 

        {"Central Europe Standard Time", "CZ", "Europe/Prague", REGION_DEFAULT}, // Prague

        {"Central Europe Standard Time", "HU", "Europe/Budapest", DEFAULT}, // Budapest

        {"Central Europe Standard Time", "ME", "Europe/Podgorica", REGION_DEFAULT},

        {"Central Europe Standard Time", "RS", "Europe/Belgrade", REGION_DEFAULT}, // Belgrade

        {"Central Europe Standard Time", "SI", "Europe/Ljubljana", REGION_DEFAULT}, // Ljubljana

        {"Central Europe Standard Time", "SK", "Europe/Bratislava", REGION_DEFAULT}, // Bratislava

        // --------------------------------------------------------------------
        // (UTC+01:00) Sarajevo, Skopje, Warsaw, Zagreb
        {"Central European Standard Time", "BA", "Europe/Sarajevo", REGION_DEFAULT}, // Sarajevo

        {"Central European Standard Time", "HR", "Europe/Zagreb", REGION_DEFAULT}, // Zagreb

        {"Central European Standard Time", "MK", "Europe/Skopje", REGION_DEFAULT}, // Skopje

        {"Central European Standard Time", "PL", "Europe/Warsaw", DEFAULT}, // Warsaw


        // --------------------------------------------------------------------
        // (UTC+01:00) Windhoek
        {"Namibia Standard Time", "NA", "Africa/Windhoek", DEFAULT}, // Windhoek


        // --------------------------------------------------------------------
        //(UTC+01:00) Brussels, Copenhagen, Madrid, Paris
        {"Romance Standard Time", "BE", "Europe/Brussels", REGION_DEFAULT}, // Brussels

        {"Romance Standard Time", "DK", "Europe/Copenhagen", REGION_DEFAULT}, // Copenhagen

        {"Romance Standard Time", "ES", "Africa/Ceuta"},
        {"Romance Standard Time", "ES", "Europe/Madrid", REGION_DEFAULT}, // Madrid

        {"Romance Standard Time", "FR", "Europe/Paris", DEFAULT}, // Paris

        // --------------------------------------------------------------------
        // (UTC+01:00) West Central Africa
        {"W. Central Africa Standard Time", "AO", "Africa/Luanda", REGION_DEFAULT},

        {"W. Central Africa Standard Time", "BJ", "Africa/Porto-Novo", REGION_DEFAULT},

        {"W. Central Africa Standard Time", "CD", "Africa/Kinshasa", REGION_DEFAULT},

        {"W. Central Africa Standard Time", "CF", "Africa/Bangui", REGION_DEFAULT},

        {"W. Central Africa Standard Time", "CG", "Africa/Brazzaville", REGION_DEFAULT},

        {"W. Central Africa Standard Time", "CM", "Africa/Douala", REGION_DEFAULT},

        {"W. Central Africa Standard Time", "DZ", "Africa/Algiers", REGION_DEFAULT},

        {"W. Central Africa Standard Time", "GA", "Africa/Libreville", REGION_DEFAULT},

        {"W. Central Africa Standard Time", "GQ", "Africa/Malabo", REGION_DEFAULT},

        {"W. Central Africa Standard Time", "NE", "Africa/Niamey", REGION_DEFAULT},

        {"W. Central Africa Standard Time", "NG", "Africa/Lagos", DEFAULT},

        {"W. Central Africa Standard Time", "TD", "Africa/Ndjamena", REGION_DEFAULT},

        // This is the only Windows time zone with UTC offset +01:00 without DST
        {"W. Central Africa Standard Time", "ZZ", "Etc/GMT-1", REGION_DEFAULT},

        // Tunis belongs to CET, but no DST in last several years.
        {"W. Central Africa Standard Time", "TN", "Africa/Tunis", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC+01:00) Amsterdam, Berlin, Bern, Rome, Stockholm, Vienna
        {"W. Europe Standard Time", "AD", "Europe/Andorra",REGION_DEFAULT},

        {"W. Europe Standard Time", "AT", "Europe/Vienna", REGION_DEFAULT}, // Vienna

        {"W. Europe Standard Time", "CH", "Europe/Zurich", REGION_DEFAULT}, // *Bern

        {"W. Europe Standard Time", "DE", "Europe/Berlin", DEFAULT}, // Berlin
        {"W. Europe Standard Time", "DE", "Europe/Busingen"},

        {"W. Europe Standard Time", "GI", "Europe/Gibraltar", REGION_DEFAULT},

        {"W. Europe Standard Time", "IT", "Europe/Rome", REGION_DEFAULT}, // Rome

        {"W. Europe Standard Time", "LI", "Europe/Vaduz", REGION_DEFAULT},

        {"W. Europe Standard Time", "LU", "Europe/Luxembourg", REGION_DEFAULT},

        {"W. Europe Standard Time", "MC", "Europe/Monaco", REGION_DEFAULT},

        {"W. Europe Standard Time", "MT", "Europe/Malta", REGION_DEFAULT},

        {"W. Europe Standard Time", "NL", "Europe/Amsterdam", REGION_DEFAULT}, // Amsterdam

        {"W. Europe Standard Time", "NO", "Europe/Oslo", REGION_DEFAULT},

        {"W. Europe Standard Time", "SE", "Europe/Stockholm", REGION_DEFAULT}, // Stockholm

        {"W. Europe Standard Time", "SJ", "Arctic/Longyearbyen", REGION_DEFAULT},

        {"W. Europe Standard Time", "SM", "Europe/San_Marino", REGION_DEFAULT},

        {"W. Europe Standard Time", "VA", "Europe/Vatican", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC+01:00) Tripoli
        {"Libya Standard Time", "LY", "Africa/Tripoli", DEFAULT},


        // --------------------------------------------------------------------
        // (UTC+02:00) E. Europe
        {"E. Europe Standard Time", "CY", "Asia/Nicosia", DEFAULT}, // Nicosia


        // --------------------------------------------------------------------
        // (UTC+02:00) Cairo
        {"Egypt Standard Time", "EG", "Africa/Cairo", DEFAULT},


        // --------------------------------------------------------------------
        // (UTC+02:00) Helsinki, Kyiv, Riga, Sofia, Tallinn, Vilnius
        {"FLE Standard Time", "AX", "Europe/Mariehamn", REGION_DEFAULT},

        {"FLE Standard Time", "BG", "Europe/Sofia", REGION_DEFAULT}, // Sofia

        {"FLE Standard Time", "EE", "Europe/Tallinn", REGION_DEFAULT}, // Tallinn

        {"FLE Standard Time", "FI", "Europe/Helsinki", REGION_DEFAULT}, // Helsinki

        {"FLE Standard Time", "LT", "Europe/Vilnius", REGION_DEFAULT}, // Vilnius

        {"FLE Standard Time", "LV", "Europe/Riga", REGION_DEFAULT}, // Riga

        {"FLE Standard Time", "UA", "Europe/Kiev", DEFAULT}, // Kyiv
        {"FLE Standard Time", "UA", "Europe/Uzhgorod"},
        {"FLE Standard Time", "UA", "Europe/Simferopol"},
        {"FLE Standard Time", "UA", "Europe/Zaporozhye"},


        // --------------------------------------------------------------------
        // (UTC+02:00) Athens, Bucharest
        {"GTB Standard Time", "GR", "Europe/Athens", REGION_DEFAULT}, // Athens

        {"GTB Standard Time", "MD", "Europe/Chisinau", REGION_DEFAULT},

        {"GTB Standard Time", "RO", "Europe/Bucharest", DEFAULT}, // Bucharest


        // --------------------------------------------------------------------
        // (UTC+02:00) Jerusalem
        {"Israel Standard Time", "IL", "Asia/Jerusalem", DEFAULT}, // Jerusalem


        // --------------------------------------------------------------------
        // (UTC+02:00) Amman
        {"Jordan Standard Time", "JO", "Asia/Amman", DEFAULT}, // Amman


        // --------------------------------------------------------------------
        // (UTC+02:00) Beirut
        {"Middle East Standard Time", "LB", "Asia/Beirut", DEFAULT}, // Beirut


        // --------------------------------------------------------------------
        // (UTC+02:00) Harare, Pretoria
        {"South Africa Standard Time", "BI", "Africa/Bujumbura", REGION_DEFAULT},

        {"South Africa Standard Time", "BW", "Africa/Gaborone", REGION_DEFAULT},

        {"South Africa Standard Time", "CD", "Africa/Lubumbashi", REGION_DEFAULT},

        {"South Africa Standard Time", "LS", "Africa/Maseru", REGION_DEFAULT},

        {"South Africa Standard Time", "MW", "Africa/Blantyre", REGION_DEFAULT},

        {"South Africa Standard Time", "MZ", "Africa/Maputo", REGION_DEFAULT},

        {"South Africa Standard Time", "RW", "Africa/Kigali", REGION_DEFAULT},

        {"South Africa Standard Time", "SZ", "Africa/Mbabane", REGION_DEFAULT},

        {"South Africa Standard Time", "ZA", "Africa/Johannesburg", DEFAULT}, // *Pretoria

        {"South Africa Standard Time", "ZM", "Africa/Lusaka", REGION_DEFAULT},

        {"South Africa Standard Time", "ZW", "Africa/Harare", REGION_DEFAULT}, // Harare

        // There are two Windows time zones with UTC offset +02:00 without DST for now.
        // Another one is (UTC+02:00) Cairo, which used to observe DST until recently.
        // So we'll use this zone as default UTC+02:00/no DST zone.
        {"South Africa Standard Time", "ZZ", "Etc/GMT-2", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC+02:00) Damascus
        {"Syria Standard Time", "SY", "Asia/Damascus", DEFAULT}, // Damascus


        // --------------------------------------------------------------------
        // (UTC+02:00) Istanbul
        {"Turkey Standard Time", "TR", "Europe/Istanbul", DEFAULT}, // Istanbul


        // --------------------------------------------------------------------
        // (UTC+03:00) Kuwait, Riyadh
        {"Arab Standard Time", "BH", "Asia/Bahrain", REGION_DEFAULT},

        {"Arab Standard Time", "KW", "Asia/Kuwait", REGION_DEFAULT}, // Kuwait

        {"Arab Standard Time", "QA", "Asia/Qatar", REGION_DEFAULT},

        {"Arab Standard Time", "SA", "Asia/Riyadh", DEFAULT}, // Riyadh

        {"Arab Standard Time", "YE", "Asia/Aden", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC+03:00) Baghdad
        {"Arabic Standard Time", "IQ", "Asia/Baghdad", DEFAULT}, // Baghdad


        // --------------------------------------------------------------------
        // (UTC+03:00) Nairobi
        {"E. Africa Standard Time", "DJ", "Africa/Djibouti", REGION_DEFAULT},

        {"E. Africa Standard Time", "ET", "Africa/Addis_Ababa", REGION_DEFAULT},

        {"E. Africa Standard Time", "ER", "Africa/Asmera", REGION_DEFAULT},

        {"E. Africa Standard Time", "KE", "Africa/Nairobi", DEFAULT}, // Nairobi

        {"E. Africa Standard Time", "KM", "Indian/Comoro", REGION_DEFAULT},

        {"E. Africa Standard Time", "MG", "Indian/Antananarivo", REGION_DEFAULT},

        {"E. Africa Standard Time", "SD", "Africa/Khartoum", REGION_DEFAULT},

        {"E. Africa Standard Time", "SO", "Africa/Mogadishu", REGION_DEFAULT},

        {"E. Africa Standard Time", "SS", "Africa/Juba", REGION_DEFAULT},

        {"E. Africa Standard Time", "TZ", "Africa/Dar_es_Salaam", REGION_DEFAULT},

        {"E. Africa Standard Time", "UG", "Africa/Kampala", REGION_DEFAULT},

        {"E. Africa Standard Time", "YT", "Indian/Mayotte", REGION_DEFAULT},

        // There are four Windows time zones with UTC offset +03:00 without DST for now.
        // We just pick this one for broader coverage and stability of this zone.
        {"E. Africa Standard Time", "ZZ", "Etc/GMT-3", REGION_DEFAULT},

        // Syowa station is in Antarctica. The zone is listed here just because we use
        // this zone as the default UTC+03:00/no DST zone.
        {"E. Africa Standard Time", "AQ", "Antarctica/Syowa", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC+03:00) Kaliningrad, Minsk
        {"Kaliningrad Standard Time", "BY", "Europe/Minsk", REGION_DEFAULT}, // Minsk

        {"Kaliningrad Standard Time", "RU", "Europe/Kaliningrad", DEFAULT}, // Kaliningrad


        // --------------------------------------------------------------------
        // (UTC+03:30) Tehran
        {"Iran Standard Time", "IR", "Asia/Tehran", DEFAULT}, // Tehran


        // --------------------------------------------------------------------
        // (UTC+04:00) Abu Dhabi, Muscat
        {"Arabian Standard Time", "AE", "Asia/Dubai", DEFAULT}, // *Abu Dhabi

        {"Arabian Standard Time", "OM", "Asia/Muscat", REGION_DEFAULT}, // Muscat

        // There are four Windows time zones with UTC offset +04:00 without DST for now.
        // We just pick this one for broader coverage and stability of this zone.
        {"Arabian Standard Time", "ZZ", "Etc/GMT-4", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC+04:00) Baku
        {"Azerbaijan Standard Time", "AZ", "Asia/Baku", DEFAULT}, // Baku


        // --------------------------------------------------------------------
        // (UTC+04:00) Yerevan
        {"Caucasus Standard Time", "AM", "Asia/Yerevan", DEFAULT}, // Yerevan


        // --------------------------------------------------------------------
        // (UTC+04:00) Tbilisi
        {"Georgian Standard Time", "GE", "Asia/Tbilisi", DEFAULT}, // Tbilisi


        // --------------------------------------------------------------------
        // (UTC+04:00) Port Louis
        {"Mauritius Standard Time", "MU", "Indian/Mauritius", DEFAULT}, // *Port Luis

        // Following zones are not Mauritius, but close enough and using the
        // same UTC offset/no DST.
        {"Mauritius Standard Time", "SC", "Indian/Mahe", REGION_DEFAULT},

        {"Mauritius Standard Time", "RE", "Indian/Reunion", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC+04:00) Moscow, St. Petersburg, Volgograd
        {"Russian Standard Time", "RU", "Europe/Moscow", DEFAULT}, // Moscow
        {"Russian Standard Time", "RU", "Europe/Volgograd"}, // Volgograd
        {"Russian Standard Time", "RU", "Europe/Samara"},


        // --------------------------------------------------------------------
        // (UTC+04:30) Kabul
        {"Afghanistan Standard Time", "AF", "Asia/Kabul", DEFAULT}, // Kabul


        // --------------------------------------------------------------------
        // (UTC+05:00) Islamabad, Karachi
        {"Pakistan Standard Time", "PK", "Asia/Karachi", DEFAULT}, // Karachi


        // --------------------------------------------------------------------
        // (UTC+05:00) Tashkent
        {"West Asia Standard Time", "KZ", "Asia/Aqtau"},
        {"West Asia Standard Time", "KZ", "Asia/Aqtobe"},
        {"West Asia Standard Time", "KZ", "Asia/Oral", REGION_DEFAULT},
        
        {"West Asia Standard Time", "TM", "Asia/Ashgabat", REGION_DEFAULT},

        {"West Asia Standard Time", "TJ", "Asia/Dushanbe", REGION_DEFAULT},

        {"West Asia Standard Time", "UZ", "Asia/Samarkand"},
        {"West Asia Standard Time", "UZ", "Asia/Tashkent", DEFAULT}, // Tashkent

        // There are two Windows time zones with UTC offset +05:00 without DST for now.
        // Another one is (UTC+05:00) Islamabad, Karachi, which used to observe DST until
        // recently. So we'll use this zone as default UTC+05:00/no DST zone.
        {"West Asia Standard Time", "ZZ", "Etc/GMT-5", REGION_DEFAULT},

        // Following zones are not in West Asia. Because we use this zone as the
        // default UTC+05:00/no DST zone and these zones do not belong anywhere else, they
        // are just listed here.
        {"West Asia Standard Time", "AQ", "Antarctica/Mawson", REGION_DEFAULT},

        {"West Asia Standard Time", "MV", "Indian/Maldives", REGION_DEFAULT},

        {"West Asia Standard Time", "TF", "Indian/Kerguelen", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC+05:30) Chennai, Kolkata, Mumbai, New Delhi
        {"India Standard Time", "IN", "Asia/Calcutta", DEFAULT}, // Kolkata


        // --------------------------------------------------------------------
        // (UTC+05:30) Sri Jayawardenepura
        {"Sri Lanka Standard Time", "LK", "Asia/Colombo", DEFAULT}, // *Sri Jayawardenepura


        // --------------------------------------------------------------------
        // (UTC+05:45) Kathmandu
        {"Nepal Standard Time", "NP", "Asia/Katmandu", DEFAULT}, // Kathmandu


        // --------------------------------------------------------------------
        // (UTC+06:00) Dhaka
        {"Bangladesh Standard Time", "BD", "Asia/Dhaka", DEFAULT}, // Dhaka

        {"Bangladesh Standard Time", "BT", "Asia/Thimphu", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC+06:00) Astana
        {"Central Asia Standard Time", "KG", "Asia/Bishkek", REGION_DEFAULT},

        {"Central Asia Standard Time", "KZ", "Asia/Almaty", DEFAULT}, // *Astana
        {"Central Asia Standard Time", "KZ", "Asia/Qyzylorda"},

        // There are three Windows time zones with UTC offset +06:00 without DST for now.
        // Other two used to observe DST until recently comparing to this zone. So we'll
        // use this zone as default UTC+06:00/no DST zone.
        {"Central Asia Standard Time", "ZZ", "Etc/GMT-6", REGION_DEFAULT},

        // Following zones are not in Central Asia. Because we use this zone as the
        // default UTC+06:00/no DST zone and these zones do not belong anywhere else, they
        // are just listed here.
        {"Central Asia Standard Time", "AQ", "Antarctica/Vostok", REGION_DEFAULT},

        {"Central Asia Standard Time", "IO", "Indian/Chagos", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC+06:00) Ekaterinburg
        {"Ekaterinburg Standard Time", "RU", "Asia/Yekaterinburg", DEFAULT}, // Ekaterinburg


        // --------------------------------------------------------------------
        // (UTC+06:30) Yangon (Rangoon)

        {"Myanmar Standard Time", "MM", "Asia/Rangoon", DEFAULT}, // Yangon (Rangoon)

        // Cocos Island is not related to Myanmar, but this is the only Windows zone
        // with UTC+06:30/no DST.
        {"Myanmar Standard Time", "CC", "Indian/Cocos", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC+07:00) Novosibirsk
        {"N. Central Asia Standard Time", "RU", "Asia/Novokuznetsk"},
        {"N. Central Asia Standard Time", "RU", "Asia/Novosibirsk", DEFAULT}, // Novosibirsk
        {"N. Central Asia Standard Time", "RU", "Asia/Omsk"},


        // --------------------------------------------------------------------
        // (UTC+07:00) Bangkok, Hanoi, Jakarta
        {"SE Asia Standard Time", "KH", "Asia/Phnom_Penh", REGION_DEFAULT},

        {"SE Asia Standard Time", "ID", "Asia/Jakarta", REGION_DEFAULT}, // Jakarta
        {"SE Asia Standard Time", "ID", "Asia/Pontianak"},

        {"SE Asia Standard Time", "LA", "Asia/Vientiane", REGION_DEFAULT},

        {"SE Asia Standard Time", "TH", "Asia/Bangkok", DEFAULT}, // Bangkok

        {"SE Asia Standard Time", "VN", "Asia/Saigon", REGION_DEFAULT}, // *Hanoi

        // There are two Windows time zones with UTC offset +07:00 without DST for now.
        // Another one is (UTC+07:00) Novosibirsk, which used to observe DST until recently.
        // So we'll use this zone as default UTC+07:00/no DST zone.
        {"SE Asia Standard Time", "ZZ", "Etc/GMT-7", REGION_DEFAULT},

        // Following zones are not South East Asia regions. Because we use this zone as the
        // default UTC+07:00/no DST zone and these zones do not belong anywhere else, they
        // are just listed here.
        {"SE Asia Standard Time", "AQ", "Antarctica/Davis", REGION_DEFAULT},

        {"SE Asia Standard Time", "MN", "Asia/Hovd", REGION_DEFAULT},

        {"SE Asia Standard Time", "CX", "Indian/Christmas", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC+08:00) Beijing, Chongqing, Hong Kong, Urumqi
        {"China Standard Time", "CN", "Asia/Chongqing"}, // Chongquing
        {"China Standard Time", "CN", "Asia/Harbin"},
        {"China Standard Time", "CN", "Asia/Kashgar"},
        {"China Standard Time", "CN", "Asia/Shanghai", DEFAULT}, // *Beijing
        {"China Standard Time", "CN", "Asia/Urumqi"}, // Urumqi

        {"China Standard Time", "HK", "Asia/Hong_Kong", REGION_DEFAULT}, // Hong Kong

        {"China Standard Time", "MO", "Asia/Macau", REGION_DEFAULT},

        // --------------------------------------------------------------------
        // (UTC+08:00) Krasnoyarsk
        {"North Asia Standard Time", "RU", "Asia/Krasnoyarsk", DEFAULT}, // Krasnoyarsk


        // --------------------------------------------------------------------
        // (UTC+08:00) Kuala Lumpur, Singapore
        {"Singapore Standard Time", "BN", "Asia/Brunei", REGION_DEFAULT},

        {"Singapore Standard Time", "ID", "Asia/Makassar", REGION_DEFAULT},

        {"Singapore Standard Time", "MY", "Asia/Kuala_Lumpur", REGION_DEFAULT}, // Kuala Lumpur
        {"Singapore Standard Time", "MY", "Asia/Kuching"},

        {"Singapore Standard Time", "PH", "Asia/Manila", REGION_DEFAULT},

        {"Singapore Standard Time", "SG", "Asia/Singapore", DEFAULT}, // Singapore

        // There are six Windows time zones with UTC offset +08:00 without DST for now.
        // Probably, this zone never observe DST in near future comparing to others,
        // so we'll use this zone as default UTC+08:00/no DST zone.
        {"Singapore Standard Time", "ZZ", "Etc/GMT-8", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC+08:00) Taipei
        {"Taipei Standard Time", "TW", "Asia/Taipei", DEFAULT}, // Taipei


        // --------------------------------------------------------------------
        // (UTC+08:00) Ulaanbaatar
        {"Ulaanbaatar Standard Time", "MN", "Asia/Ulaanbaatar", DEFAULT}, // Ulaanbaatar
        {"Ulaanbaatar Standard Time", "MN", "Asia/Choibalsan"},


        // --------------------------------------------------------------------
        // (UTC+08:00) Perth
        {"W. Australia Standard Time", "AU", "Australia/Perth", DEFAULT}, // Perth

        {"W. Australia Standard Time", "AQ", "Antarctica/Casey", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC+09:00) Seoul
        {"Korea Standard Time", "KP", "Asia/Pyongyang", REGION_DEFAULT},

        {"Korea Standard Time", "KR", "Asia/Seoul", DEFAULT}, // Seoul


        // --------------------------------------------------------------------
        // (UTC+09:00) Irkutsk
        {"North Asia East Standard Time", "RU", "Asia/Irkutsk", DEFAULT}, // Irkutsk


        // --------------------------------------------------------------------
        // (UTC+09:00) Osaka, Sapporo, Tokyo
        {"Tokyo Standard Time", "JP", "Asia/Tokyo", DEFAULT}, // Tokyo

        // There are three Windows time zones with UTC offset +09:00 without DST for now.
        // In fact, there is no strong reason to pick this one as default UTC+09:00/no DST
        // zone over (UTC+09:00) Seoul.
        {"Tokyo Standard Time", "ZZ", "Etc/GMT-9", REGION_DEFAULT},

        // Following zones are nothing related to Japan. Because we use this zone as the
        // default UTC+09:00/no DST zone and these zones do not belong anywhere else, they
        // are just listed here.
        {"Tokyo Standard Time", "ID", "Asia/Jayapura", REGION_DEFAULT},

        {"Tokyo Standard Time", "TL", "Asia/Dili", REGION_DEFAULT},

        {"Tokyo Standard Time", "PW", "Pacific/Palau", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC+09:30) Darwin
        {"AUS Central Standard Time", "AU", "Australia/Darwin", DEFAULT}, // Darwin


        // --------------------------------------------------------------------
        // (UTC+09:30) Adelaide
        {"Cen. Australia Standard Time", "AU", "Australia/Adelaide", DEFAULT}, // Adelaide
        {"Cen. Australia Standard Time", "AU", "Australia/Broken_Hill"},


        // --------------------------------------------------------------------
        // (UTC+10:00) Canberra, Melbourne, Sydney
        {"AUS Eastern Standard Time", "AU", "Australia/Melbourne"}, // Melbourne
        {"AUS Eastern Standard Time", "AU", "Australia/Sydney", DEFAULT}, // Sydney


        // --------------------------------------------------------------------
        // (UTC+10:00) Brisbane
        {"E. Australia Standard Time", "AU", "Australia/Brisbane", DEFAULT}, // Brisbane
        {"E. Australia Standard Time", "AU", "Australia/Lindeman"},


        // --------------------------------------------------------------------
        // (UTC+10:00) Hobart
        {"Tasmania Standard Time", "AU", "Australia/Currie"},
        {"Tasmania Standard Time", "AU", "Australia/Hobart", DEFAULT}, // Hobert


        // --------------------------------------------------------------------
        // (UTC+10:00) Guam, Port Moresby
        {"West Pacific Standard Time", "FM", "Pacific/Truk", REGION_DEFAULT},

        {"West Pacific Standard Time", "GU", "Pacific/Guam", REGION_DEFAULT}, // Guam

        {"West Pacific Standard Time", "MP", "Pacific/Saipan", REGION_DEFAULT},

        {"West Pacific Standard Time", "PG", "Pacific/Port_Moresby", DEFAULT}, // Port Moresby

        // There are three Windows time zones with UTC offset +10:00 without DST for now.
        // Other two are (UTC+10:00) Brisbane and (UTC+10:00) Yakutsk. Among these three,
        // this is the only zone that has never obserbed DST (and probably won't in future).
        // So we'll use this zone as default UTC+10:00/no DST zone.
        {"West Pacific Standard Time", "ZZ", "Etc/GMT-10", REGION_DEFAULT},

        // Dumont D'Urville station does not belong to West Pacific. The zone is listed here
        // just because we use this zone as the default UTC+10:00/no DST zone.
        {"West Pacific Standard Time", "AQ", "Antarctica/DumontDUrville", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC+10:00) Yakutsk
        {"Yakutsk Standard Time", "RU", "Asia/Yakutsk", DEFAULT}, // Yakrtsk
        {"Yakutsk Standard Time", "RU", "Asia/Khandyga"},


        // --------------------------------------------------------------------
        // (UTC+11:00) Solomon Is., New Caledonia
        {"Central Pacific Standard Time", "FM", "Pacific/Kosrae"},
        {"Central Pacific Standard Time", "FM", "Pacific/Ponape", REGION_DEFAULT},

        {"Central Pacific Standard Time", "NC", "Pacific/Noumea", REGION_DEFAULT}, // New Caledonia

        {"Central Pacific Standard Time", "SB", "Pacific/Guadalcanal", DEFAULT}, // *Solomon Is.

        {"Central Pacific Standard Time", "VU", "Pacific/Efate", REGION_DEFAULT},

        // There are two Windows time zones with UTC offset +11:00 without DST for now.
        // Another one is (UTC+11:00) Vladivostok, which used to observe DST until recently.
        // So we'll use this zone as default UTC+11:00/no DST zone.
        {"Central Pacific Standard Time", "ZZ", "Etc/GMT-11", REGION_DEFAULT},

        // Macquarie Island does not belong to Central Pacific. The zone is listed here
        // just because we use this zone as the default UTC+11:00/no DST zone.
        {"Central Pacific Standard Time", "AU", "Antarctica/Macquarie", REGION_DEFAULT},


        // --------------------------------------------------------------------
        // (UTC+11:00) Vladivostok
        {"Vladivostok Standard Time", "RU", "Asia/Vladivostok", DEFAULT}, // Vladivostok
        {"Vladivostok Standard Time", "RU", "Asia/Sakhalin"},
        {"Vladivostok Standard Time", "RU", "Asia/Ust-Nera"},


        // --------------------------------------------------------------------
        // (UTC+12:00) Fiji
        {"Fiji Standard Time", "FJ", "Pacific/Fiji", DEFAULT}, // Fiji


        // --------------------------------------------------------------------
        // (UTC+12:00) Magadan
        {"Magadan Standard Time", "RU", "Asia/Anadyr"},
        {"Magadan Standard Time", "RU", "Asia/Kamchatka"},
        {"Magadan Standard Time", "RU", "Asia/Magadan", DEFAULT}, // Magadan


        // --------------------------------------------------------------------
        // (UTC+12:00) Auckland, Wellington
        {"New Zealand Standard Time", "AQ", "Antarctica/McMurdo"},
        {"New Zealand Standard Time", "AQ", "Antarctica/South_Pole", REGION_DEFAULT},

        {"New Zealand Standard Time", "NZ", "Pacific/Auckland", DEFAULT}, // Auckland


        // --------------------------------------------------------------------
        // (UTC+12:00) Coordinated Universal Time+12
        {"UTC+12", "KI", "Pacific/Tarawa", REGION_DEFAULT},

        {"UTC+12", "MH", "Pacific/Kwajalein"},
        {"UTC+12", "MH", "Pacific/Majuro", REGION_DEFAULT},

        {"UTC+12", "NR", "Pacific/Nauru", REGION_DEFAULT},

        {"UTC+12", "TV", "Pacific/Funafuti", REGION_DEFAULT},

        {"UTC+12", "UM", "Pacific/Wake", REGION_DEFAULT},

        {"UTC+12", "WF", "Pacific/Wallis", REGION_DEFAULT},

        {"UTC+12", "ZZ", "Etc/GMT-12", DEFAULT},


        // --------------------------------------------------------------------
        // (UTC+13:00) Samoa
        {"Samoa Standard Time", "WS", "Pacific/Apia", DEFAULT}, // *Samoa


        // --------------------------------------------------------------------
        // (UTC+13:00) Nuku'alofa
        {"Tonga Standard Time", "TO", "Pacific/Tongatapu", DEFAULT}, // *Nuku'alofa

        // Enderbury/Fakaofo are not in Tonga, but close enough and using the
        // same UTC offset/no DST.
        {"Tonga Standard Time", "KI", "Pacific/Enderbury", REGION_DEFAULT},
        {"Tonga Standard Time", "TK", "Pacific/Fakaofo", REGION_DEFAULT},

        // This is the only Windows time zone with UTC offset +13:00 without DST
        {"Tonga Standard Time", "ZZ", "Etc/GMT-13", REGION_DEFAULT},
    };

    /*
     * 
     */
    static final String[] UNMAPPALBE_WINDOWS_ZONES_ARRAY = {
        // UTC-02:00/DST start: last Sun in March / end: last Sun in September
        // All UTC-02:00 zones in the tz dadtabase do not observe DST
        "Mid-Atlantic Standard Time",
    };

    /*
     * There are some Olson time zones that do not have the same base UTC offset in
     * Windows time zones. These zones are not supported by Windows.
     */
    static final String[] NO_BASE_OFFSET_MATCH_ZONES_ARRAY = {
        "Australia/Eucla",      // +8:45
        "Australia/Lord_Howe",  // +10:30
        "Etc/GMT-14",           // +14:00
        "Pacific/Chatham",      // +12:45
        "Pacific/Kiritimati",   // +14:00
        "Pacific/Marquesas",    // -9:30
        "Pacific/Norfolk",      // +11:30
    };

    /*
     * These Olson time zones are using different DST rules from Windows zones with
     * same base offset.
     */
    static final String[] NO_DST_RULE_MATCH_ZONES_ARRAY = {
        // UTC-10:00/North American DST rule.
        // Closest match - "Hawaiian Standard Time" (no DST)
        "America/Adak",

        // UTC-08:00/no DST.
        // Closest match - "Pacific Standard Time" (observes DST).
        "Etc/GMT+8",
        "America/Metlakatla",
        "Pacific/Pitcairn",

        // UTC-09:00/no DST
        // Closest match - "Alaskan Standard Time" (observes DST).
        "Etc/GMT+9",
        "Pacific/Gambier",

        // UTC-06:00/Southern Hemisphere style DST rule.
        // Closest match - "Central America Standard Time" (observes Northern Hemisphere style DST rule).
        "Pacific/Easter",

        // UTC-03:00 zone with North American DST rule.
        // Closest match - "Greenland Standard Time" (observes EU DST rule).
        "America/Miquelon",

        // UTC+02:00 with DST (Mar - Sep).
        // Closest match - "E. Europe Standard Time", "Israel Standard Time" and some others
        "Asia/Gaza",
        "Asia/Hebron",
    };

    /*
     * ICU's canonical time zone list is based on the tz database and including
     * some extra zones not included in the CLDR's canonical list.
     */
    static final String[] NON_CLDR_ZONES_ARRAY = {
        "Factory",

        "Asia/Riyadh87",
        "Asia/Riyadh88",
        "Asia/Riyadh89",

        "SystemV/AST4",
        "SystemV/AST4ADT",
        "SystemV/CST6",
        "SystemV/CST6CDT",
        "SystemV/EST5",
        "SystemV/EST5EDT",
        "SystemV/HST10",
        "SystemV/MST7",
        "SystemV/MST7MDT",
        "SystemV/PST8",
        "SystemV/PST8PDT",
        "SystemV/YST9",
        "SystemV/YST9YDT",

        "CET",
        "EET",
        "MET",
        "WET",
    };
}
