bool IDNSpoofChecker::Check(base::StringPiece16 label) {
   UErrorCode status = U_ZERO_ERROR;
   int32_t result = uspoof_check(checker_, label.data(),
                                 base::checked_cast<int32_t>(label.size()),
                                NULL, &status);
  if (U_FAILURE(status) || (result & USPOOF_ALL_CHECKS))
    return false;

  icu::UnicodeString label_string(FALSE, label.data(),
                                  base::checked_cast<int32_t>(label.size()));

  if (deviation_characters_.containsSome(label_string))
     return false;
 
   result &= USPOOF_RESTRICTION_LEVEL_MASK;
  if (result == USPOOF_ASCII ||
      (result == USPOOF_SINGLE_SCRIPT_RESTRICTIVE &&
       kana_letters_exceptions_.containsNone(label_string)))
    return true;
 
  if (non_ascii_latin_letters_.containsSome(label_string))
    return false;

  if (!tls_index.initialized())
    tls_index.Initialize(&OnThreadTermination);
  icu::RegexMatcher* dangerous_pattern =
      reinterpret_cast<icu::RegexMatcher*>(tls_index.Get());
  if (!dangerous_pattern) {
    dangerous_pattern = new icu::RegexMatcher(
        icu::UnicodeString(
            "[^\\p{scx=kana}\\p{scx=hira}\\p{scx=hani}]"
            "[\\u30ce\\u30f3\\u30bd\\u30be]"
            "[^\\p{scx=kana}\\p{scx=hira}\\p{scx=hani}]|"
            "[^\\p{scx=kana}\\p{scx=hira}]\\u30fc|"
            "\\u30fc[^\\p{scx=kana}\\p{scx=hira}]|"
            "^[\\p{scx=kana}]+[\\u3078-\\u307a][\\p{scx=kana}]+$|"
            "^[\\p{scx=hira}]+[\\u30d8-\\u30da][\\p{scx=hira}]+$|"
            "[a-z]\\u30fb|\\u30fb[a-z]|"
            "^[\\u0585\\u0581]+[a-z]|[a-z][\\u0585\\u0581]+$|"
            "[a-z][\\u0585\\u0581]+[a-z]|"
            "^[og]+[\\p{scx=armn}]|[\\p{scx=armn}][og]+$|"
            "[\\p{scx=armn}][og]+[\\p{scx=armn}]", -1, US_INV),
        0, status);
    tls_index.Set(dangerous_pattern);
  }
  dangerous_pattern->reset(label_string);
   return !dangerous_pattern->find();
 }
