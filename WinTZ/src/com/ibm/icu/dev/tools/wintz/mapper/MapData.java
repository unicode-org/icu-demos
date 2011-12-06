package com.ibm.icu.dev.tools.wintz.mapper;

public class MapData {
    /*
     * Raw Windows ID - region - zones mapping data
     */
    static final String[][] MAP_DATA_ARRAY = {
        // --------------------------------------------------------------------
        // (UTC-12:00) International Date Line West
        {"Dateline Standard Time", "001", "Etc/GMT+12"},


        // --------------------------------------------------------------------
        // (UTC-11:00) Coordinated Universal Time-11
        {"UTC-11", "001", "Etc/GMT+11"},
        {"UTC-11", "AS", "Pacific/Pago_Pago"},
        {"UTC-11", "NU", "Pacific/Niue"},
        {"UTC-11", "UM", "Pacific/Midway"},


        // --------------------------------------------------------------------
        // (UTC-10:00) Hawaii
        {"Hawaiian Standard Time", "US", "Pacific/Honolulu"}, // *Hawaii
        {"Hawaiian Standard Time", "UM", "Pacific/Johnston"},

        // This is the only Windows time zone with UTC offset -10:00 without DST
        {"Hawaiian Standard Time", "001", "Etc/GMT+10"},

        // Following locations don't belong to Hawaii. Because this is only
        // the Windows time zone with UTC-10:00/no DST, they cannot use anything else.
        {"Hawaiian Standard Time", "CK", "Pacific/Rarotonga"},
        {"Hawaiian Standard Time", "PF", "Pacific/Tahiti"},
        {"Hawaiian Standard Time", "TK", "Pacific/Fakaofo"},


        // --------------------------------------------------------------------
        // (UTC-09:00) Alaska
        {"Alaskan Standard Time", "US", "America/Anchorage"}, // *Alaska
        {"Alaskan Standard Time", "US", "America/Juneau"},
        {"Alaskan Standard Time", "US", "America/Nome"},
        {"Alaskan Standard Time", "US", "America/Sitka"},
        {"Alaskan Standard Time", "US", "America/Yakutat"},


        // --------------------------------------------------------------------
        // (UTC-08:00) Pacific Time (US & Canada)
        {"Pacific Standard Time", "US", "America/Los_Angeles"},
        {"Pacific Standard Time", "CA", "America/Vancouver"},
        {"Pacific Standard Time", "CA", "America/Dawson"},
        {"Pacific Standard Time", "CA", "America/Whitehorse"},
        {"Pacific Standard Time", "MX", "America/Tijuana"},
        {"Pacific Standard Time", "001", "PST8PDT"},


        // --------------------------------------------------------------------
        // (UTC-08:00) Baja California
        {"Pacific Standard Time (Mexico)", "MX", "America/Santa_Isabel"},


        // --------------------------------------------------------------------
        // (UTC-07:00) Mountain Time (US & Canada)
        {"Mountain Standard Time", "US", "America/Denver"},
        {"Mountain Standard Time", "US", "America/Boise"},
        {"Mountain Standard Time", "US", "America/Shiprock"},
        {"Mountain Standard Time", "CA", "America/Edmonton"},
        {"Mountain Standard Time", "CA", "America/Cambridge_Bay"},
        {"Mountain Standard Time", "CA", "America/Inuvik"},
        {"Mountain Standard Time", "CA", "America/Yellowknife"},
        {"Mountain Standard Time", "MX", "America/Ojinaga"},
        {"Mountain Standard Time", "001", "MST7MDT"},


        // --------------------------------------------------------------------
        // (UTC-07:00) Chihuahua, La Paz, Mazatlan
        {"Mountain Standard Time (Mexico)", "MX", "America/Chihuahua"}, // Chihuahua
        {"Mountain Standard Time (Mexico)", "MX", "America/Mazatlan"}, // Mazatlan


        // --------------------------------------------------------------------
        // (UTC-07:00) Arizona
        {"US Mountain Standard Time", "US", "America/Phoenix"}, // *Arizona
        {"US Mountain Standard Time", "CA", "America/Dawson_Creek"},
        {"US Mountain Standard Time", "MX", "America/Hermosillo"},
        // This is the only Windows time zone with UTC offset -07:00 without DST
        {"US Mountain Standard Time", "001", "Etc/GMT+7"},


        // --------------------------------------------------------------------
        // (UTC-06:00) Saskatchewan
        {"Canada Central Standard Time", "CA", "America/Regina"}, // *Saskatchewan
        {"Canada Central Standard Time", "CA", "America/Swift_Current"},


        // --------------------------------------------------------------------
        // (UTC-06:00) Central America
        {"Central America Standard Time", "BZ", "America/Belize"},
        {"Central America Standard Time", "CR", "America/Costa_Rica"},
        {"Central America Standard Time", "SV", "America/El_Salvador"},
        {"Central America Standard Time", "GT", "America/Guatemala"},
        {"Central America Standard Time", "NI", "America/Managua"},
        {"Central America Standard Time", "HN", "America/Tegucigalpa"},
        {"Central America Standard Time", "EC", "Pacific/Galapagos"},

        // There are two Windows time zones with UTC offset -06:00 without DST for now.
        // Another one is (UTC-06:00) Saskatchewan. Because the coverage of this zone is
        // broader, we'll use this zone as default UTC-06:00/no DST zone.
        {"Central America Standard Time", "001", "Etc/GMT+6"},


        // --------------------------------------------------------------------
        // (UTC-06:00) Central Time (US & Canada)
        {"Central Standard Time", "US", "America/Chicago"},
        {"Central Standard Time", "US", "America/Indiana/Knox"},
        {"Central Standard Time", "US", "America/Indiana/Tell_City"},
        {"Central Standard Time", "US", "America/Menominee"},
        {"Central Standard Time", "US", "America/North_Dakota/Center"},
        {"Central Standard Time", "US", "America/North_Dakota/New_Salem"},
        {"Central Standard Time", "US", "America/North_Dakota/Beulah"},
        {"Central Standard Time", "CA", "America/Winnipeg"},
        {"Central Standard Time", "CA", "America/Rainy_River"},
        {"Central Standard Time", "CA", "America/Rankin_Inlet"},
        {"Central Standard Time", "CA", "America/Resolute"},
        {"Central Standard Time", "MX", "America/Matamoros"},
        {"Central Standard Time", "001", "CST6CDT"},


        // --------------------------------------------------------------------
        // (UTC-06:00) Guadalajara, Mexico City, Monterrey
        {"Central Standard Time (Mexico)", "MX", "America/Mexico_City"}, // Mexico City
        {"Central Standard Time (Mexico)", "MX", "America/Monterrey"}, // Monterrey
        {"Central Standard Time (Mexico)", "MX", "America/Bahia_Banderas"},
        {"Central Standard Time (Mexico)", "MX", "America/Cancun"},
        {"Central Standard Time (Mexico)", "MX", "America/Merida"},


        // --------------------------------------------------------------------
        // (UTC-05:00) Eastern Time (US & Canada)
        {"Eastern Standard Time", "US", "America/New_York"},
        {"Eastern Standard Time", "US", "America/Detroit"},
        {"Eastern Standard Time", "US", "America/Indiana/Petersburg"},
        {"Eastern Standard Time", "US", "America/Indiana/Vincennes"},
        {"Eastern Standard Time", "US", "America/Indiana/Winamac"},
        {"Eastern Standard Time", "US", "America/Kentucky/Monticello"},
        {"Eastern Standard Time", "US", "America/Louisville"},
        {"Eastern Standard Time", "BS", "America/Nassau"},
        {"Eastern Standard Time", "CA", "America/Toronto"},
        {"Eastern Standard Time", "CA", "America/Iqaluit"},
        {"Eastern Standard Time", "CA", "America/Montreal"},
        {"Eastern Standard Time", "CA", "America/Nipigon"},
        {"Eastern Standard Time", "CA", "America/Pangnirtung"},
        {"Eastern Standard Time", "CA", "America/Thunder_Bay"},
        {"Eastern Standard Time", "TC", "America/Grand_Turk"},
        {"Eastern Standard Time", "001", "EST5EDT"},


        // --------------------------------------------------------------------
        // (UTC-05:00) Bogota, Lima, Quito
        {"SA Pacific Standard Time", "CO", "America/Bogota"}, // Bogota
        {"SA Pacific Standard Time", "PE", "America/Lima"}, // Lima
        {"SA Pacific Standard Time", "EC", "America/Guayaquil"}, // *Quito

        // This is the only Windows time zone with UTC offset -05:00 without DST
        {"SA Pacific Standard Time", "001", "Etc/GMT+5"},

        // Following zones are not South American Pacific regions. Because this is only
        // the Windows time zone with UTC-05:00/no DST, they cannot use anything else.
        {"SA Pacific Standard Time", "CA", "America/Coral_Harbour"},
        {"SA Pacific Standard Time", "CU", "America/Havana"},
        {"SA Pacific Standard Time", "HT", "America/Port-au-Prince"},
        {"SA Pacific Standard Time", "JM", "America/Jamaica"},
        {"SA Pacific Standard Time", "KY", "America/Cayman"},
        {"SA Pacific Standard Time", "PA", "America/Panama"},


        // --------------------------------------------------------------------
        // (UTC-05:00) Indiana (East)
        {"US Eastern Standard Time", "US", "America/Indianapolis"}, // *Indiana (East)
        {"US Eastern Standard Time", "US", "America/Indiana/Marengo"},
        {"US Eastern Standard Time", "US", "America/Indiana/Vevay"},


        // --------------------------------------------------------------------
        // (UTC-04:30) Caracas
        {"Venezuela Standard Time", "VE", "America/Caracas"}, // Caracas


        // --------------------------------------------------------------------
        // (UTC-04:00) Atlantic Time (Canada)
        {"Atlantic Standard Time", "CA", "America/Halifax"},
        {"Atlantic Standard Time", "CA", "America/Glace_Bay"},
        {"Atlantic Standard Time", "CA", "America/Goose_Bay"},
        {"Atlantic Standard Time", "CA", "America/Moncton"},
        {"Atlantic Standard Time", "BM", "Atlantic/Bermuda"},
        {"Atlantic Standard Time", "GL", "America/Thule"},


        // --------------------------------------------------------------------
        // (UTC-04:00) Cuiaba
        {"Central Brazilian Standard Time", "BR", "America/Cuiaba"}, // Cuiaba
        {"Central Brazilian Standard Time", "BR", "America/Campo_Grande"},


        // --------------------------------------------------------------------
        // (UTC-04:00) Santiago
        {"Pacific SA Standard Time", "CL", "America/Santiago"}, // Santiago

        // According to the description in antarctica file, Palmer station keeps the same time
        // as Puta Arenas, Chile. The rule ChileAQ seems is out of sync with the rule Chile in
        // southamerica file in 2011n. This is probably a bug in the tz database. For now,
        // we assume Santiago and Palmer station are using the same time zone.
        {"Pacific SA Standard Time", "AQ", "Antarctica/Palmer"},


        // --------------------------------------------------------------------
        // (UTC-04:00) Asuncion
        {"Paraguay Standard Time", "PY", "America/Asuncion"}, // Asuncion


        // --------------------------------------------------------------------
        // (UTC-04:00) Georgetown, La Paz, Manaus, San Juan
        {"SA Western Standard Time", "GY", "America/Guyana"}, // *Georgetown
        {"SA Western Standard Time", "BO", "America/La_Paz"}, // La Paz
        {"SA Western Standard Time", "BR", "America/Manaus"}, // Manaus
        {"SA Western Standard Time", "BR", "America/Boa_Vista"},
        {"SA Western Standard Time", "BR", "America/Eirunepe"},
        {"SA Western Standard Time", "BR", "America/Porto_Velho"},
        {"SA Western Standard Time", "BR", "America/Rio_Branco"},

        // Note: There are many locations named San Juan. It's most likely San Juan, Puerto Rico,
        // but it does not match the ID description - South America (SA) Western. San Juan and
        // many other Caribbean territories are using UTC-4 Atlantic Time without daylight saving
        // time. Windows does not have the dedicated zone for Atlantic Standard Time (no DST).
        // I assume Microsoft simply use this zone for all locations using UTC-4 without DST.
        // Followings are locations using AST.
        {"SA Western Standard Time", "PR", "America/Puerto_Rico"}, // *San Juan
        {"SA Western Standard Time", "AI", "America/Anguilla"},
        {"SA Western Standard Time", "AG", "America/Antigua"},
        {"SA Western Standard Time", "AW", "America/Aruba"},
        {"SA Western Standard Time", "BB", "America/Barbados"},
        {"SA Western Standard Time", "BL", "America/St_Barthelemy"},
        {"SA Western Standard Time", "CA", "America/Blanc-Sablon"},
        {"SA Western Standard Time", "CW", "America/Curacao"},
        {"SA Western Standard Time", "DM", "America/Dominica"},
        {"SA Western Standard Time", "DO", "America/Santo_Domingo"},
        {"SA Western Standard Time", "GD", "America/Grenada"},
        {"SA Western Standard Time", "GP", "America/Guadeloupe"},
        {"SA Western Standard Time", "LC", "America/St_Lucia"},
        {"SA Western Standard Time", "KN", "America/St_Kitts"},
        {"SA Western Standard Time", "MF", "America/Marigot"},
        {"SA Western Standard Time", "MQ", "America/Martinique"},
        {"SA Western Standard Time", "MS", "America/Montserrat"},
        {"SA Western Standard Time", "TT", "America/Port_of_Spain"},
        {"SA Western Standard Time", "VC", "America/St_Vincent"},
        {"SA Western Standard Time", "VG", "America/Tortola"},
        {"SA Western Standard Time", "VI", "America/St_Thomas"},

        // This is the only Windows time zone with UTC offset -04:00 without DST
        {"SA Western Standard Time", "001", "Etc/GMT+4"},


        // --------------------------------------------------------------------
        // (UTC-03:30) Newfoundland
        {"Newfoundland Standard Time", "CA", "America/St_Johns"}, // *Newfoundland


        // --------------------------------------------------------------------
        // (UTC-03:00) Buenos Aires
        {"Argentina Standard Time", "AR", "America/Buenos_Aires"}, // Buenos Aires
        {"Argentina Standard Time", "AR", "America/Argentina/La_Rioja"},
        {"Argentina Standard Time", "AR", "America/Argentina/Rio_Gallegos"},
        {"Argentina Standard Time", "AR", "America/Argentina/Salta"},
        {"Argentina Standard Time", "AR", "America/Argentina/San_Juan"},
        {"Argentina Standard Time", "AR", "America/Argentina/San_Luis"},
        {"Argentina Standard Time", "AR", "America/Argentina/Tucuman"},
        {"Argentina Standard Time", "AR", "America/Argentina/Ushuaia"},
        {"Argentina Standard Time", "AR", "America/Catamarca"},
        {"Argentina Standard Time", "AR", "America/Cordoba"},
        {"Argentina Standard Time", "AR", "America/Jujuy"},
        {"Argentina Standard Time", "AR", "America/Mendoza"},


        // --------------------------------------------------------------------
        // (UTC-03:00) Salvador
        {"Bahia Standard Time", "BR", "America/Bahia"}, // *Salvador

        // --------------------------------------------------------------------
        // (UTC-03:00) Brasilia
        {"E. South America Standard Time", "BR", "America/Sao_Paulo"}, // *Brasilia


        // --------------------------------------------------------------------
        // (UTC-03:00) Greenland
        {"Greenland Standard Time", "GL", "America/Godthab"}, // *Greenland


        // --------------------------------------------------------------------
        // (UTC-03:00) Montevideo
        {"Montevideo Standard Time", "UY", "America/Montevideo"}, // Montevideo


        // --------------------------------------------------------------------
        // (UTC-03:00) Cayenne, Fortaleza
        {"SA Eastern Standard Time", "GF", "America/Cayenne"}, // Cayenne
        {"SA Eastern Standard Time", "BR", "America/Fortaleza"}, // Fortaleza
        {"SA Eastern Standard Time", "BR", "America/Araguaina"},
        {"SA Eastern Standard Time", "BR", "America/Belem"},
        {"SA Eastern Standard Time", "BR", "America/Maceio"},
        {"SA Eastern Standard Time", "BR", "America/Recife"},
        {"SA Eastern Standard Time", "BR", "America/Santarem"},
        {"SA Eastern Standard Time", "SR", "America/Paramaribo"},

        // There are two Windows time zones with UTC offset -03:00 without DST for now.
        // Another one is (UTC-03:00) Buenos Aires. Until recently Buenos Aires used to
        // observe DST, and Cayenne/Fortaleze did not observe DST for longer time,
        // we'll use this zone as default UTC-03:00/no DST zone.
        {"SA Eastern Standard Time", "001", "Etc/GMT+3"},

        // Rothera station is in Antarctica. The zone is listed here just because we use
        // this zone as the default UTC-03:00/no DST zone.
        {"SA Eastern Standard Time", "AQ", "Antarctica/Rothera"},

        // Falkland did not go back to winter time in April, 2011. So they use UTC-03:00
        // (base offset: -4:00 + saving 1:00) offset for entire 2011. We're not sure they
        // will continue to stay UTC-03:00.
        {"SA Eastern Standard Time", "FK", "Atlantic/Stanley"},


        // --------------------------------------------------------------------
        // (UTC-02:00) Mid-Atlantic


        // --------------------------------------------------------------------
        // (UTC-02:00) Coordinated Universal Time-02
        {"UTC-02", "001", "Etc/GMT+2"},
        {"UTC-02", "BR", "America/Noronha"},
        {"UTC-02", "GS", "Atlantic/South_Georgia"},


        // --------------------------------------------------------------------
        // (UTC-01:00) Azores
        {"Azores Standard Time", "PT", "Atlantic/Azores"}, // Azores

        // Scoresbysund currently uses the exact same rule (UTC-01:00 / EU's DST rule)
        // with Azores.
        {"Azores Standard Time", "GL", "America/Scoresbysund"},


        // --------------------------------------------------------------------
        // (UTC-01:00) Cape Verde Is.
        {"Cape Verde Standard Time", "CV", "Atlantic/Cape_Verde"}, // Cape Verde Is.

        // This is the only Windows time zone with UTC offset -01:00 without DST
        {"Cape Verde Standard Time", "001", "Etc/GMT+1"},


        // --------------------------------------------------------------------
        // (UTC) Dublin, Edinburgh, Lisbon, London
        {"GMT Standard Time", "GB", "Europe/London"}, // London
        {"GMT Standard Time", "ES", "Atlantic/Canary"},
        {"GMT Standard Time", "GG", "Europe/Guernsey"},
        {"GMT Standard Time", "FO", "Atlantic/Faeroe"},
        {"GMT Standard Time", "IE", "Europe/Dublin"}, // Dublin
        {"GMT Standard Time", "IM", "Europe/Isle_of_Man"},
        {"GMT Standard Time", "JE", "Europe/Jersey"},
        {"GMT Standard Time", "PT", "Europe/Lisbon"}, // Lisbon
        {"GMT Standard Time", "PT", "Atlantic/Madeira"},


        // --------------------------------------------------------------------
        // (UTC) Monrovia, Reykjavik
        {"Greenwich Standard Time", "LR", "Africa/Monrovia"}, // Monrovia
        {"Greenwich Standard Time", "IS", "Atlantic/Reykjavik"}, // Reykjavik
        {"Greenwich Standard Time", "CI", "Africa/Abidjan"},
        {"Greenwich Standard Time", "BF", "Africa/Ouagadougou"},
        {"Greenwich Standard Time", "GH", "Africa/Accra"},
        {"Greenwich Standard Time", "GM", "Africa/Banjul"},
        {"Greenwich Standard Time", "GN", "Africa/Conakry"},
        {"Greenwich Standard Time", "GW", "Africa/Bissau"},
        {"Greenwich Standard Time", "ML", "Africa/Bamako"},
        {"Greenwich Standard Time", "MR", "Africa/Nouakchott"},
        {"Greenwich Standard Time", "SH", "Atlantic/St_Helena"},
        {"Greenwich Standard Time", "SL", "Africa/Freetown"},
        {"Greenwich Standard Time", "SN", "Africa/Dakar"},
        {"Greenwich Standard Time", "ST", "Africa/Sao_Tome"},
        {"Greenwich Standard Time", "TG", "Africa/Lome"},

        // Al Aaiun's time zone is actually WET/no DST
        {"Greenwich Standard Time", "EH", "Africa/El_Aaiun"},

        
        // --------------------------------------------------------------------
        // (UTC) Casablanca
        {"Morocco Standard Time", "MA", "Africa/Casablanca"},


        // --------------------------------------------------------------------
        // (UTC) Coordinated Universal Time
        {"UTC", "001", "Etc/GMT"},
        {"UTC", "GL", "America/Danmarkshavn"},


        // --------------------------------------------------------------------
        // (UTC+01:00) Belgrade, Bratislava, Budapest, Ljubljana, Prague
        {"Central Europe Standard Time", "RS", "Europe/Belgrade"}, // Belgrade
        {"Central Europe Standard Time", "ME", "Europe/Podgorica"},
        {"Central Europe Standard Time", "SK", "Europe/Bratislava"}, // Bratislava
        {"Central Europe Standard Time", "HU", "Europe/Budapest"}, // Budapest
        {"Central Europe Standard Time", "SI", "Europe/Ljubljana"}, // Ljubljana
        {"Central Europe Standard Time", "CZ", "Europe/Prague"}, // Prague
        {"Central Europe Standard Time", "AL", "Europe/Tirane"}, 


        // --------------------------------------------------------------------
        // (UTC+01:00) Sarajevo, Skopje, Warsaw, Zagreb
        {"Central European Standard Time", "BA", "Europe/Sarajevo"}, // Sarajevo
        {"Central European Standard Time", "MK", "Europe/Skopje"}, // Skopje
        {"Central European Standard Time", "PL", "Europe/Warsaw"}, // Warsaw
        {"Central European Standard Time", "HR", "Europe/Zagreb"}, // Zagreb


        // --------------------------------------------------------------------
        // (UTC+01:00) Windhoek
        {"Namibia Standard Time", "NA", "Africa/Windhoek"}, // Windhoek


        // --------------------------------------------------------------------
        //(UTC+01:00) Brussels, Copenhagen, Madrid, Paris
        {"Romance Standard Time", "BE", "Europe/Brussels"}, // Brussels
        {"Romance Standard Time", "DK", "Europe/Copenhagen"}, // Copenhagen
        {"Romance Standard Time", "ES", "Europe/Madrid"}, // Madrid
        {"Romance Standard Time", "ES", "Africa/Ceuta"},
        {"Romance Standard Time", "FR", "Europe/Paris"}, // Paris

        // --------------------------------------------------------------------
        // (UTC+01:00) West Central Africa
        {"W. Central Africa Standard Time", "DZ", "Africa/Algiers"},
        {"W. Central Africa Standard Time", "CF", "Africa/Bangui"},
        {"W. Central Africa Standard Time", "CG", "Africa/Brazzaville"},
        {"W. Central Africa Standard Time", "CM", "Africa/Douala"},
        {"W. Central Africa Standard Time", "CD", "Africa/Kinshasa"},
        {"W. Central Africa Standard Time", "NG", "Africa/Lagos"},
        {"W. Central Africa Standard Time", "GA", "Africa/Libreville"},
        {"W. Central Africa Standard Time", "AO", "Africa/Luanda"},
        {"W. Central Africa Standard Time", "GQ", "Africa/Malabo"},
        {"W. Central Africa Standard Time", "TD", "Africa/Ndjamena"},
        {"W. Central Africa Standard Time", "NE", "Africa/Niamey"},
        {"W. Central Africa Standard Time", "BJ", "Africa/Porto-Novo"},

        // This is the only Windows time zone with UTC offset +01:00 without DST
        {"W. Central Africa Standard Time", "001", "Etc/GMT-1"},

        // Tunis belongs to CET, but no DST in last several years.
        {"W. Central Africa Standard Time", "TN", "Africa/Tunis"},


        // --------------------------------------------------------------------
        // (UTC+01:00) Amsterdam, Berlin, Bern, Rome, Stockholm, Vienna
        {"W. Europe Standard Time", "NL", "Europe/Amsterdam"}, // Amsterdam
        {"W. Europe Standard Time", "DE", "Europe/Berlin"}, // Berlin
        {"W. Europe Standard Time", "CH", "Europe/Zurich"}, // *Bern
        {"W. Europe Standard Time", "IT", "Europe/Rome"}, // Rome
        {"W. Europe Standard Time", "SM", "Europe/San_Marino"},
        {"W. Europe Standard Time", "VA", "Europe/Vatican"},
        {"W. Europe Standard Time", "SE", "Europe/Stockholm"}, // Stockholm
        {"W. Europe Standard Time", "AT", "Europe/Vienna"}, // Vienna
        {"W. Europe Standard Time", "LU", "Europe/Luxembourg"},
        {"W. Europe Standard Time", "MC", "Europe/Monaco"},
        {"W. Europe Standard Time", "NO", "Europe/Oslo"},
        {"W. Europe Standard Time", "AD", "Europe/Andorra"},
        {"W. Europe Standard Time", "GI", "Europe/Gibraltar"},
        {"W. Europe Standard Time", "MT", "Europe/Malta"},
        {"W. Europe Standard Time", "LI", "Europe/Vaduz"},
        {"W. Europe Standard Time", "SJ", "Arctic/Longyearbyen"},


        // --------------------------------------------------------------------
        // (UTC+02:00) Nicosia
        {"E. Europe Standard Time", "CY", "Asia/Nicosia"}, // Nicosia


        // --------------------------------------------------------------------
        // (UTC+02:00) Cairo
        {"Egypt Standard Time", "EG", "Africa/Cairo"},

        // Following Palestine regions are in EET. They used to observe DST
        // and the rule was different from Cairo. For now, these regions do
        // not observe DST like Cairo.
        {"Egypt Standard Time", "PS", "Asia/Gaza"},
        {"Egypt Standard Time", "PS", "Asia/Hebron"},


        // --------------------------------------------------------------------
        // (UTC+02:00) Helsinki, Kyiv, Riga, Sofia, Tallinn, Vilnius
        {"FLE Standard Time", "FI", "Europe/Helsinki"}, // Helsinki
        {"FLE Standard Time", "AX", "Europe/Mariehamn"},
        {"FLE Standard Time", "UA", "Europe/Kiev"}, // Kyiv
        {"FLE Standard Time", "UA", "Europe/Uzhgorod"},
        {"FLE Standard Time", "UA", "Europe/Zaporozhye"},
        {"FLE Standard Time", "LV", "Europe/Riga"}, // Riga
        {"FLE Standard Time", "BG", "Europe/Sofia"}, // Sofia
        {"FLE Standard Time", "EE", "Europe/Tallinn"}, // Tallinn
        {"FLE Standard Time", "LT", "Europe/Vilnius"}, // Vilnius


        // --------------------------------------------------------------------
        // (UTC+02:00) Athens, Bucharest
        {"GTB Standard Time", "GR", "Europe/Athens"}, // Athens
        {"GTB Standard Time", "RO", "Europe/Bucharest"}, // Bucharest
        {"GTB Standard Time", "MD", "Europe/Chisinau"},


        // --------------------------------------------------------------------
        // (UTC+02:00) Jerusalem
        {"Israel Standard Time", "IL", "Asia/Jerusalem"}, // Jerusalem


        // --------------------------------------------------------------------
        // (UTC+02:00) Amman
        {"Jordan Standard Time", "JO", "Asia/Amman"}, // Amman


        // --------------------------------------------------------------------
        // (UTC+02:00) Beirut
        {"Middle East Standard Time", "LB", "Asia/Beirut"}, // Beirut


        // --------------------------------------------------------------------
        // (UTC+02:00) Harare, Pretoria
        {"South Africa Standard Time", "ZW", "Africa/Harare"}, // Harare
        {"South Africa Standard Time", "ZA", "Africa/Johannesburg"}, // *Pretoria
        {"South Africa Standard Time", "MW", "Africa/Blantyre"},
        {"South Africa Standard Time", "BI", "Africa/Bujumbura"},
        {"South Africa Standard Time", "BW", "Africa/Gaborone"},
        {"South Africa Standard Time", "RW", "Africa/Kigali"},
        {"South Africa Standard Time", "CD", "Africa/Lubumbashi"},
        {"South Africa Standard Time", "ZM", "Africa/Lusaka"},
        {"South Africa Standard Time", "MZ", "Africa/Maputo"},
        {"South Africa Standard Time", "LS", "Africa/Maseru"},
        {"South Africa Standard Time", "SZ", "Africa/Mbabane"},

        // There are two Windows time zones with UTC offset +02:00 without DST for now.
        // Another one is (UTC+02:00) Cairo, which used to observe DST until recently.
        // So we'll use this zone as default UTC+02:00/no DST zone.
        {"South Africa Standard Time", "001", "Etc/GMT-2"},

        // Following zones belong to EET. So (UTC+02:00) Cairo might be a better match.
        // However, Cairo used to observe DST until very recently and may observe
        // DST in near future. On the other hand, this Windows zone does not use DST
        // at all. So, from time calculation aspect, this zone may fit better to these
        // zones.
        {"South Africa Standard Time", "LY", "Africa/Tripoli"},
        {"South Africa Standard Time", "UA", "Europe/Simferopol"},


        // --------------------------------------------------------------------
        // (UTC+02:00) Damascus
        {"Syria Standard Time", "SY", "Asia/Damascus"}, // Damascus


        // --------------------------------------------------------------------
        // (UTC+02:00) Istanbul
        {"Turkey Standard Time", "TR", "Europe/Istanbul"}, // Istanbul


        // --------------------------------------------------------------------
        // (UTC+03:00) Kuwait, Riyadh
        {"Arab Standard Time", "KW", "Asia/Kuwait"}, // Kuwait
        {"Arab Standard Time", "SA", "Asia/Riyadh"}, // Riyadh
        {"Arab Standard Time", "YE", "Asia/Aden"},
        {"Arab Standard Time", "BH", "Asia/Bahrain"},
        {"Arab Standard Time", "QA", "Asia/Qatar"},


        // --------------------------------------------------------------------
        // (UTC+03:00) Baghdad
        {"Arabic Standard Time", "IQ", "Asia/Baghdad"}, // Baghdad


        // --------------------------------------------------------------------
        // (UTC+03:00) Nairobi
        {"E. Africa Standard Time", "ET", "Africa/Addis_Ababa"},
        {"E. Africa Standard Time", "ER", "Africa/Asmera"},
        {"E. Africa Standard Time", "TZ", "Africa/Dar_es_Salaam"},
        {"E. Africa Standard Time", "DJ", "Africa/Djibouti"},
        {"E. Africa Standard Time", "SS", "Africa/Juba"},
        {"E. Africa Standard Time", "UG", "Africa/Kampala"},
        {"E. Africa Standard Time", "SD", "Africa/Khartoum"},
        {"E. Africa Standard Time", "SO", "Africa/Mogadishu"},
        {"E. Africa Standard Time", "KE", "Africa/Nairobi"}, // Nairobi
        {"E. Africa Standard Time", "MG", "Indian/Antananarivo"},
        {"E. Africa Standard Time", "KM", "Indian/Comoro"},
        {"E. Africa Standard Time", "YT", "Indian/Mayotte"},

        // There are four Windows time zones with UTC offset +03:00 without DST for now.
        // We just pick this one for broader coverage and stability of this zone.
        {"E. Africa Standard Time", "001", "Etc/GMT-3"},

        // Syowa station is in Antarctica. The zone is listed here just because we use
        // this zone as the default UTC+03:00/no DST zone.
        {"E. Africa Standard Time", "AQ", "Antarctica/Syowa"},


        // --------------------------------------------------------------------
        // (UTC+03:00) Kaliningrad, Minsk
        {"Kaliningrad Standard Time", "RU", "Europe/Kaliningrad"}, // Kaliningrad
        {"Kaliningrad Standard Time", "BY", "Europe/Minsk"}, // Minsk


        // --------------------------------------------------------------------
        // (UTC+03:30) Tehran
        {"Iran Standard Time", "IR", "Asia/Tehran"}, // Tehran


        // --------------------------------------------------------------------
        // (UTC+04:00) Abu Dhabi, Muscat
        {"Arabian Standard Time", "AE", "Asia/Dubai"}, // *Abu Dhabi
        {"Arabian Standard Time", "OM", "Asia/Muscat"}, // Muscat

        // There are four Windows time zones with UTC offset +04:00 without DST for now.
        // We just pick this one for broader coverage and stability of this zone.
        {"Arabian Standard Time", "001", "Etc/GMT-4"},


        // --------------------------------------------------------------------
        // (UTC+04:00) Baku
        {"Azerbaijan Standard Time", "AZ", "Asia/Baku"}, // Baku


        // --------------------------------------------------------------------
        // (UTC+04:00) Yerevan
        {"Caucasus Standard Time", "AM", "Asia/Yerevan"}, // Yerevan


        // --------------------------------------------------------------------
        // (UTC+04:00) Tbilisi
        {"Georgian Standard Time", "GE", "Asia/Tbilisi"}, // Tbilisi


        // --------------------------------------------------------------------
        // (UTC+04:00) Port Louis
        {"Mauritius Standard Time", "MU", "Indian/Mauritius"}, // *Port Luis

        // Following zones are not Mauritius, but close enough and using the
        // same UTC offset/no DST.
        {"Mauritius Standard Time", "SC", "Indian/Mahe"},
        {"Mauritius Standard Time", "RE", "Indian/Reunion"},


        // --------------------------------------------------------------------
        // (UTC+04:00) Moscow, St. Petersburg, Volgograd
        {"Russian Standard Time", "RU", "Europe/Moscow"}, // Moscow
        {"Russian Standard Time", "RU", "Europe/Volgograd"}, // Volgograd
        {"Russian Standard Time", "RU", "Europe/Samara"},


        // --------------------------------------------------------------------
        // (UTC+04:30) Kabul
        {"Afghanistan Standard Time", "AF", "Asia/Kabul"}, // Kabul


        // --------------------------------------------------------------------
        // (UTC+05:00) Islamabad, Karachi
        {"Pakistan Standard Time", "PK", "Asia/Karachi"}, // Karachi


        // --------------------------------------------------------------------
        // (UTC+05:00) Tashkent
        {"West Asia Standard Time", "UZ", "Asia/Tashkent"}, // Tashkent
        {"West Asia Standard Time", "UZ", "Asia/Samarkand"},
        {"West Asia Standard Time", "KZ", "Asia/Aqtau"},
        {"West Asia Standard Time", "KZ", "Asia/Aqtobe"},
        {"West Asia Standard Time", "KZ", "Asia/Oral"},
        {"West Asia Standard Time", "TM", "Asia/Ashgabat"},
        {"West Asia Standard Time", "TJ", "Asia/Dushanbe"},

        // There are two Windows time zones with UTC offset +05:00 without DST for now.
        // Another one is (UTC+05:00) Islamabad, Karachi, which used to observe DST until
        // recently. So we'll use this zone as default UTC+05:00/no DST zone.
        {"West Asia Standard Time", "001", "Etc/GMT-5"},

        // Following zones are not in West Asia. Because we use this zone as the
        // default UTC+05:00/no DST zone and these zones do not belong anywhere else, they
        // are just listed here.
        {"West Asia Standard Time", "AQ", "Antarctica/Mawson"},
        {"West Asia Standard Time", "TF", "Indian/Kerguelen"},
        {"West Asia Standard Time", "MV", "Indian/Maldives"},


        // --------------------------------------------------------------------
        // (UTC+05:30) Chennai, Kolkata, Mumbai, New Delhi
        {"India Standard Time", "IN", "Asia/Calcutta"}, // Kolkata


        // --------------------------------------------------------------------
        // (UTC+05:30) Sri Jayawardenepura
        {"Sri Lanka Standard Time", "LK", "Asia/Colombo"}, // *Sri Jayawardenepura


        // --------------------------------------------------------------------
        // (UTC+05:45) Kathmandu
        {"Nepal Standard Time", "NP", "Asia/Katmandu"}, // Kathmandu


        // --------------------------------------------------------------------
        // (UTC+06:00) Dhaka
        {"Bangladesh Standard Time", "BD", "Asia/Dhaka"}, // Dhaka
        {"Bangladesh Standard Time", "BT", "Asia/Thimphu"},


        // --------------------------------------------------------------------
        // (UTC+06:00) Astana
        {"Central Asia Standard Time", "KZ", "Asia/Almaty"}, // *Astana
        {"Central Asia Standard Time", "KZ", "Asia/Qyzylorda"},
        {"Central Asia Standard Time", "KG", "Asia/Bishkek"},

        // There are three Windows time zones with UTC offset +06:00 without DST for now.
        // Other two used to observe DST until recently comparing to this zone. So we'll
        // use this zone as default UTC+06:00/no DST zone.
        {"Central Asia Standard Time", "001", "Etc/GMT-6"},

        // Following zones are not in Central Asia. Because we use this zone as the
        // default UTC+06:00/no DST zone and these zones do not belong anywhere else, they
        // are just listed here.
        {"Central Asia Standard Time", "AQ", "Antarctica/Vostok"},
        {"Central Asia Standard Time", "IO", "Indian/Chagos"},


        // --------------------------------------------------------------------
        // (UTC+06:00) Ekaterinburg
        {"Ekaterinburg Standard Time", "RU", "Asia/Yekaterinburg"}, // Ekaterinburg


        // --------------------------------------------------------------------
        // (UTC+06:30) Yangon (Rangoon)

        {"Myanmar Standard Time", "MM", "Asia/Rangoon"}, // Yangon (Rangoon)
        // Cocos Island is not related to Myanmar, but this is the only Windows zone
        // with UTC+06:30/no DST.
        {"Myanmar Standard Time", "CC", "Indian/Cocos"},


        // --------------------------------------------------------------------
        // (UTC+07:00) Novosibirsk
        {"N. Central Asia Standard Time", "RU", "Asia/Novosibirsk"}, // Novosibirsk
        {"N. Central Asia Standard Time", "RU", "Asia/Novokuznetsk"},
        {"N. Central Asia Standard Time", "RU", "Asia/Omsk"},


        // --------------------------------------------------------------------
        // (UTC+07:00) Bangkok, Hanoi, Jakarta
        {"SE Asia Standard Time", "TH", "Asia/Bangkok"}, // Bangkok
        {"SE Asia Standard Time", "ID", "Asia/Jakarta"}, // Jakarta
        {"SE Asia Standard Time", "ID", "Asia/Pontianak"},
        {"SE Asia Standard Time", "VN", "Asia/Saigon"}, // *Hanoi
        {"SE Asia Standard Time", "KH", "Asia/Phnom_Penh"},
        {"SE Asia Standard Time", "LA", "Asia/Vientiane"},

        // There are two Windows time zones with UTC offset +07:00 without DST for now.
        // Another one is (UTC+07:00) Novosibirsk, which used to observe DST until recently.
        // So we'll use this zone as default UTC+07:00/no DST zone.
        {"SE Asia Standard Time", "001", "Etc/GMT-7"},

        // Following zones are not South East Asia regions. Because we use this zone as the
        // default UTC+07:00/no DST zone and these zones do not belong anywhere else, they
        // are just listed here.
        {"SE Asia Standard Time", "AQ", "Antarctica/Davis"},
        {"SE Asia Standard Time", "MN", "Asia/Hovd"},
        {"SE Asia Standard Time", "CX", "Indian/Christmas"},


        // --------------------------------------------------------------------
        // (UTC+08:00) Beijing, Chongqing, Hong Kong, Urumqi
        {"China Standard Time", "CN", "Asia/Shanghai"}, // *Beijing
        {"China Standard Time", "CN", "Asia/Chongqing"}, // Chongquing
        {"China Standard Time", "HK", "Asia/Hong_Kong"}, // Hong Kong
        {"China Standard Time", "CN", "Asia/Urumqi"}, // Urumqi
        {"China Standard Time", "CN", "Asia/Harbin"},
        {"China Standard Time", "CN", "Asia/Kashgar"},
        {"China Standard Time", "MO", "Asia/Macau"},

        // --------------------------------------------------------------------
        // (UTC+08:00) Krasnoyarsk
        {"North Asia Standard Time", "RU", "Asia/Krasnoyarsk"}, // Krasnoyarsk


        // --------------------------------------------------------------------
        // (UTC+08:00) Kuala Lumpur, Singapore
        {"Singapore Standard Time", "MY", "Asia/Kuala_Lumpur"}, // Kuala Lumpur
        {"Singapore Standard Time", "MY", "Asia/Kuching"},
        {"Singapore Standard Time", "SG", "Asia/Singapore"}, // Singapore
        {"Singapore Standard Time", "ID", "Asia/Makassar"},
        {"Singapore Standard Time", "PH", "Asia/Manila"},
        {"Singapore Standard Time", "BN", "Asia/Brunei"},

        // There are six Windows time zones with UTC offset +08:00 without DST for now.
        // Probably, this zone never observe DST in near future comparing to others,
        // so we'll use this zone as default UTC+08:00/no DST zone.
        {"Singapore Standard Time", "001", "Etc/GMT-8"},


        // --------------------------------------------------------------------
        // (UTC+08:00) Taipei
        {"Taipei Standard Time", "TW", "Asia/Taipei"}, // Taipei


        // --------------------------------------------------------------------
        // (UTC+08:00) Ulaanbaatar
        {"Ulaanbaatar Standard Time", "MN", "Asia/Ulaanbaatar"}, // Ulaanbaatar
        {"Ulaanbaatar Standard Time", "MN", "Asia/Choibalsan"},


        // --------------------------------------------------------------------
        // (UTC+08:00) Perth
        {"W. Australia Standard Time", "AU", "Australia/Perth"}, // Perth
        {"W. Australia Standard Time", "AQ", "Antarctica/Casey"},


        // --------------------------------------------------------------------
        // (UTC+09:00) Seoul
        {"Korea Standard Time", "KP", "Asia/Pyongyang"},
        {"Korea Standard Time", "KR", "Asia/Seoul"}, // Seoul


        // --------------------------------------------------------------------
        // (UTC+09:00) Irkutsk
        {"North Asia East Standard Time", "RU", "Asia/Irkutsk"}, // Irkutsk


        // --------------------------------------------------------------------
        // (UTC+09:00) Osaka, Sapporo, Tokyo
        {"Tokyo Standard Time", "JP", "Asia/Tokyo"}, // Tokyo

        // There are three Windows time zones with UTC offset +09:00 without DST for now.
        // In fact, there is no strong reason to pick this one as default UTC+09:00/no DST
        // zone over (UTC+09:00) Seoul.
        {"Tokyo Standard Time", "001", "Etc/GMT-9"},

        // Following zones are nothing related to Japan. Because we use this zone as the
        // default UTC+09:00/no DST zone and these zones do not belong anywhere else, they
        // are just listed here.
        {"Tokyo Standard Time", "TL", "Asia/Dili"},
        {"Tokyo Standard Time", "ID", "Asia/Jayapura"},
        {"Tokyo Standard Time", "PW", "Pacific/Palau"},


        // --------------------------------------------------------------------
        // (UTC+09:30) Darwin
        {"AUS Central Standard Time", "AU", "Australia/Darwin"}, // Darwin


        // --------------------------------------------------------------------
        // (UTC+09:30) Adelaide
        {"Cen. Australia Standard Time", "AU", "Australia/Adelaide"}, // Adelaide
        {"Cen. Australia Standard Time", "AU", "Australia/Broken_Hill"},


        // --------------------------------------------------------------------
        // (UTC+10:00) Canberra, Melbourne, Sydney
        {"AUS Eastern Standard Time", "AU", "Australia/Melbourne"}, // Melbourne
        {"AUS Eastern Standard Time", "AU", "Australia/Sydney"}, // Sydney


        // --------------------------------------------------------------------
        // (UTC+10:00) Brisbane
        {"E. Australia Standard Time", "AU", "Australia/Brisbane"}, // Brisbane
        {"E. Australia Standard Time", "AU", "Australia/Lindeman"},


        // --------------------------------------------------------------------
        // (UTC+10:00) Hobart
        {"Tasmania Standard Time", "AU", "Australia/Currie"},
        {"Tasmania Standard Time", "AU", "Australia/Hobart"}, // Hobert


        // --------------------------------------------------------------------
        // (UTC+10:00) Guam, Port Moresby
        {"West Pacific Standard Time", "GU", "Pacific/Guam"}, // Guam
        {"West Pacific Standard Time", "PG", "Pacific/Port_Moresby"}, // Port Moresby
        {"West Pacific Standard Time", "MP", "Pacific/Saipan"},
        {"West Pacific Standard Time", "FM", "Pacific/Truk"},

        // There are three Windows time zones with UTC offset +10:00 without DST for now.
        // Other two are (UTC+10:00) Brisbane and (UTC+10:00) Yakutsk. Among these three,
        // this is the only zone that has never obserbed DST (and probably won't in future).
        // So we'll use this zone as default UTC+10:00/no DST zone.
        {"West Pacific Standard Time", "001", "Etc/GMT-10"},

        // Dumont D'Urville station does not belong to West Pacific. The zone is listed here
        // just because we use this zone as the default UTC+10:00/no DST zone.
        {"West Pacific Standard Time", "AQ", "Antarctica/DumontDUrville"},


        // --------------------------------------------------------------------
        // (UTC+10:00) Yakutsk
        {"Yakutsk Standard Time", "RU", "Asia/Yakutsk"}, // Yakrtsk


        // --------------------------------------------------------------------
        // (UTC+11:00) Solomon Is., New Caledonia
        {"Central Pacific Standard Time", "SB", "Pacific/Guadalcanal"}, // *Solomon Is.
        {"Central Pacific Standard Time", "NC", "Pacific/Noumea"}, // New Caledonia
        {"Central Pacific Standard Time", "VU", "Pacific/Efate"},
        {"Central Pacific Standard Time", "FM", "Pacific/Kosrae"},
        {"Central Pacific Standard Time", "FM", "Pacific/Ponape"},

        // There are two Windows time zones with UTC offset +11:00 without DST for now.
        // Another one is (UTC+11:00) Vladivostok, which used to observe DST until recently.
        // So we'll use this zone as default UTC+11:00/no DST zone.
        {"Central Pacific Standard Time", "001", "Etc/GMT-11"},

        // Macquarie Island does not belong to Central Pacific. The zone is listed here
        // just because we use this zone as the default UTC+11:00/no DST zone.
        {"Central Pacific Standard Time", "AQ", "Antarctica/Macquarie"},


        // --------------------------------------------------------------------
        // (UTC+11:00) Vladivostok
        {"Vladivostok Standard Time", "RU", "Asia/Vladivostok"}, // Vladivostok
        {"Vladivostok Standard Time", "RU", "Asia/Sakhalin"},


        // --------------------------------------------------------------------
        // (UTC+12:00) Fiji
        {"Fiji Standard Time", "FJ", "Pacific/Fiji"}, // Fiji


        // --------------------------------------------------------------------
        // (UTC+12:00) Magadan
        {"Magadan Standard Time", "RU", "Asia/Magadan"}, // Magadan
        {"Magadan Standard Time", "RU", "Asia/Anadyr"},
        {"Magadan Standard Time", "RU", "Asia/Kamchatka"},


        // --------------------------------------------------------------------
        // (UTC+12:00) Auckland, Wellington
        {"New Zealand Standard Time", "NZ", "Pacific/Auckland"}, // Auckland
        {"New Zealand Standard Time", "AQ", "Antarctica/McMurdo"},
        {"New Zealand Standard Time", "AQ", "Antarctica/South_Pole"},


        // --------------------------------------------------------------------
        // (UTC+12:00) Coordinated Universal Time+12
        {"UTC+12", "001", "Etc/GMT-12"},
        {"UTC+12", "TV", "Pacific/Funafuti"},
        {"UTC+12", "MH", "Pacific/Kwajalein"},
        {"UTC+12", "MH", "Pacific/Majuro"},
        {"UTC+12", "NR", "Pacific/Nauru"},
        {"UTC+12", "KI", "Pacific/Tarawa"},
        {"UTC+12", "UM", "Pacific/Wake"},
        {"UTC+12", "WF", "Pacific/Wallis"},


        // --------------------------------------------------------------------
        // (UTC+13:00) Samoa
        {"Samoa Standard Time", "WS", "Pacific/Apia"}, // *Samoa


        // --------------------------------------------------------------------
        // (UTC+13:00) Nuku'alofa
        {"Tonga Standard Time", "TO", "Pacific/Tongatapu"}, // *Nuku'alofa

        // Enderbury is not in Tonga, but close enough and using the
        // same UTC offset/no DST.
        {"Tonga Standard Time", "KI", "Pacific/Enderbury"},

        // This is the only Windows time zone with UTC offset +13:00 without DST
        {"Tonga Standard Time", "001", "Etc/GMT-13"},
    };

    /*
     * Raw Windows ID - region - zones mapping data / supplemental mapping data including zones only matches base offset.
     */
    static final String[][] SUPPLEMENTAL_MAP_DATA_ARRAY = {
        // No DST in Hawaii, but Adak observes DST (US rule)
        {"Hawaiian Standard Time", "US", "America/Adak"},

        // No Windows zones with UTC-08:00/no DST
        {"Pacific Standard Time", "001", "Etc/GMT+8"},
        {"Pacific Standard Time", "US", "America/Metlakatla"},
        {"Pacific Standard Time", "PN", "Pacific/Pitcairn"},

        // No Windows zones with UTC-09:00/no DST
        {"Alaskan Standard Time", "001", "Etc/GMT+9"},
        {"Alaskan Standard Time", "PF", "Pacific/Gambier"},

        // No UTC-06:00 zone with Southern Hemisphere style DST rule
        {"Central America Standard Time", "CL", "Pacific/Easter"},

        // No UTC-03:00 zone with North American DST rule - Godthab uses the same offset, but EU DST rule
        {"Greenland Standard Time", "PM", "America/Miquelon"},
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

    /*
     * There are several Olson time zones that do not have the same base UTC offset in
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

}
