  InputMethodDescriptors* GetInputMethodDescriptorsForTesting() {
  input_method::InputMethodDescriptors* GetInputMethodDescriptorsForTesting() {
    input_method::InputMethodDescriptors* descriptions =
        new input_method::InputMethodDescriptors;
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:nl::nld", "Netherlands", "nl", "nl", "nld"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:be::nld", "Belgium", "be", "be", "nld"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:fr::fra", "France", "fr", "fr", "fra"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:be::fra", "Belgium", "be", "be", "fra"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:ca::fra", "Canada", "ca", "ca", "fra"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:ch:fr:fra", "Switzerland - French", "ch(fr)", "ch(fr)", "fra"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:de::ger", "Germany", "de", "de", "ger"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:de:neo:ger", "Germany - Neo 2", "de(neo)", "de(neo)", "ger"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:be::ger", "Belgium", "be", "be", "ger"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:ch::ger", "Switzerland", "ch", "ch", "ger"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "mozc", "Mozc (US keyboard layout)", "us", "us", "ja"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "mozc-jp", "Mozc (Japanese keyboard layout)", "jp", "jp", "ja"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "mozc-dv",
         "Mozc (US Dvorak keyboard layout)", "us(dvorak)", "us(dvorak)", "ja"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:jp::jpn", "Japan", "jp", "jp", "jpn"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:ru::rus", "Russia", "ru", "ru", "rus"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:ru:phonetic:rus",
         "Russia - Phonetic", "ru(phonetic)", "ru(phonetic)", "rus"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "m17n:th:kesmanee", "kesmanee (m17n)", "us", "us", "th"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "m17n:th:pattachote", "pattachote (m17n)", "us", "us", "th"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "m17n:th:tis820", "tis820 (m17n)", "us", "us", "th"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "mozc-chewing", "Mozc Chewing (Chewing)", "us", "us", "zh_TW"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "m17n:zh:cangjie", "cangjie (m17n)", "us", "us", "zh"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "m17n:zh:quick", "quick (m17n)", "us", "us", "zh"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "m17n:vi:tcvn", "tcvn (m17n)", "us", "us", "vi"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "m17n:vi:telex", "telex (m17n)", "us", "us", "vi"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "m17n:vi:viqr", "viqr (m17n)", "us", "us", "vi"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "m17n:vi:vni", "vni (m17n)", "us", "us", "vi"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:us::eng", "USA", "us", "us", "eng"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:us:intl:eng",
         "USA - International (with dead keys)", "us(intl)", "us(intl)", "eng"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:us:altgr-intl:eng", "USA - International (AltGr dead keys)",
         "us(altgr-intl)", "us(altgr-intl)", "eng"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:us:dvorak:eng",
         "USA - Dvorak", "us(dvorak)", "us(dvorak)", "eng"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:us:colemak:eng",
         "USA - Colemak", "us(colemak)", "us(colemak)", "eng"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "hangul", "Korean", "kr(kr104)", "kr(kr104)", "ko"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "pinyin", "Pinyin", "us", "us", "zh"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "pinyin-dv", "Pinyin (for US Dvorak keyboard)",
         "us(dvorak)", "us(dvorak)", "zh"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "m17n:ar:kbd", "kbd (m17n)", "us", "us", "ar"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "m17n:hi:itrans", "itrans (m17n)", "us", "us", "hi"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "m17n:fa:isiri", "isiri (m17n)", "us", "us", "fa"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:br::por", "Brazil", "br", "br", "por"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:bg::bul", "Bulgaria", "bg", "bg", "bul"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:bg:phonetic:bul", "Bulgaria - Traditional phonetic",
         "bg(phonetic)", "bg(phonetic)", "bul"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:ca:eng:eng", "Canada - English", "ca(eng)", "ca(eng)", "eng"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:cz::cze", "Czechia", "cz", "cz", "cze"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:ee::est", "Estonia", "ee", "ee", "est"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:es::spa", "Spain", "es", "es", "spa"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:es:cat:cat", "Spain - Catalan variant with middle-dot L",
         "es(cat)", "es(cat)", "cat"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:dk::dan", "Denmark", "dk", "dk", "dan"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:gr::gre", "Greece", "gr", "gr", "gre"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:il::heb", "Israel", "il", "il", "heb"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:kr:kr104:kor", "Korea, Republic of - 101/104 key Compatible",
         "kr(kr104)", "kr(kr104)", "kor"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:latam::spa", "Latin American", "latam", "latam", "spa"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:lt::lit", "Lithuania", "lt", "lt", "lit"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:lv:apostrophe:lav", "Latvia - Apostrophe (') variant",
         "lv(apostrophe)", "lv(apostrophe)", "lav"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:hr::scr", "Croatia", "hr", "hr", "scr"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:gb:extd:eng", "United Kingdom - Extended - Winkeys",
         "gb(extd)", "gb(extd)", "eng"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:gb:dvorak:eng", "United Kingdom - Dvorak",
         "gb(dvorak)", "gb(dvorak)", "eng"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:fi::fin", "Finland", "fi", "fi", "fin"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:hu::hun", "Hungary", "hu", "hu", "hun"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:it::ita", "Italy", "it", "it", "ita"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:no::nob", "Norway", "no", "no", "nob"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:pl::pol", "Poland", "pl", "pl", "pol"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:pt::por", "Portugal", "pt", "pt", "por"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:ro::rum", "Romania", "ro", "ro", "rum"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:se::swe", "Sweden", "se", "se", "swe"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:sk::slo", "Slovakia", "sk", "sk", "slo"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:si::slv", "Slovenia", "si", "si", "slv"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:rs::srp", "Serbia", "rs", "rs", "srp"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:tr::tur", "Turkey", "tr", "tr", "tur"));
    descriptions->push_back(input_method::InputMethodDescriptor(
         "xkb:ua::ukr", "Ukraine", "ua", "ua", "ukr"));
     return descriptions;
   }
