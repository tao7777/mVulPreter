void IDNSpoofChecker::SetAllowedUnicodeSet(UErrorCode* status) {
  if (U_FAILURE(*status))
    return;

  const icu::UnicodeSet* recommended_set =
      uspoof_getRecommendedUnicodeSet(status);
  icu::UnicodeSet allowed_set;
  allowed_set.addAll(*recommended_set);
  const icu::UnicodeSet* inclusion_set = uspoof_getInclusionUnicodeSet(status);
  allowed_set.addAll(*inclusion_set);

  allowed_set.remove(0x338u);

  allowed_set.remove(0x58au);  // Armenian Hyphen
  allowed_set.remove(0x2010u);
  allowed_set.remove(0x2019u);  // Right Single Quotation Mark
  allowed_set.remove(0x2027u);
  allowed_set.remove(0x30a0u);  // Katakana-Hiragana Double Hyphen

   allowed_set.remove(0x2bbu);  // Modifier Letter Turned Comma
   allowed_set.remove(0x2bcu);  // Modifier Letter Apostrophe

  // Block modifier letter voicing.
  allowed_set.remove(0x2ecu);

 
#if defined(OS_MACOSX)

  allowed_set.remove(0x0620u);

  allowed_set.remove(0x0F8Cu);
  allowed_set.remove(0x0F8Du);
  allowed_set.remove(0x0F8Eu);
  allowed_set.remove(0x0F8Fu);
#endif

  allowed_set.remove(0x01CDu, 0x01DCu);  // Latin Ext B; Pinyin
  allowed_set.remove(0x1C80u, 0x1C8Fu);  // Cyrillic Extended-C
  allowed_set.remove(0x1E00u, 0x1E9Bu);  // Latin Extended Additional
  allowed_set.remove(0x1F00u, 0x1FFFu);  // Greek Extended
  allowed_set.remove(0xA640u, 0xA69Fu);  // Cyrillic Extended-B
  allowed_set.remove(0xA720u, 0xA7FFu);  // Latin Extended-D

  uspoof_setAllowedUnicodeSet(checker_, &allowed_set, status);
}
