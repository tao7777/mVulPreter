bool IDNSpoofChecker::SafeToDisplayAsUnicode(base::StringPiece16 label,
                                             bool is_tld_ascii) {
  UErrorCode status = U_ZERO_ERROR;
  int32_t result =
      uspoof_check(checker_, label.data(),
                   base::checked_cast<int32_t>(label.size()), NULL, &status);
  if (U_FAILURE(status) || (result & USPOOF_ALL_CHECKS))
    return false;

  icu::UnicodeString label_string(FALSE, label.data(),
                                  base::checked_cast<int32_t>(label.size()));

  if (deviation_characters_.containsSome(label_string))
    return false;

  result &= USPOOF_RESTRICTION_LEVEL_MASK;
  if (result == USPOOF_ASCII)
    return true;
  if (result == USPOOF_SINGLE_SCRIPT_RESTRICTIVE &&
      kana_letters_exceptions_.containsNone(label_string) &&
      combining_diacritics_exceptions_.containsNone(label_string)) {
    return !is_tld_ascii || !IsMadeOfLatinAlikeCyrillic(label_string);
  }

  if (non_ascii_latin_letters_.containsSome(label_string) &&
      !lgc_letters_n_ascii_.containsAll(label_string))
    return false;

  if (!tls_index.initialized())
    tls_index.Initialize(&OnThreadTermination);
  icu::RegexMatcher* dangerous_pattern =
      reinterpret_cast<icu::RegexMatcher*>(tls_index.Get());
  if (!dangerous_pattern) {
    // - Disalow mixing of Latin and Tifinagh.
    dangerous_pattern = new icu::RegexMatcher(
        icu::UnicodeString(
            R"([^\p{scx=kana}\p{scx=hira}\p{scx=hani}])"
            R"([\u30ce\u30f3\u30bd\u30be])"
            R"([^\p{scx=kana}\p{scx=hira}\p{scx=hani}]|)"
            R"([^\p{scx=kana}\p{scx=hira}]\u30fc|^\u30fc|)"
            R"([^\p{scx=kana}][\u30fd\u30fe]|^[\u30fd\u30fe]|)"
            R"(^[\p{scx=kana}]+[\u3078-\u307a][\p{scx=kana}]+$|)"
            R"(^[\p{scx=hira}]+[\u30d8-\u30da][\p{scx=hira}]+$|)"
            R"([a-z]\u30fb|\u30fb[a-z]|)"
            R"(^[\u0585\u0581]+[a-z]|[a-z][\u0585\u0581]+$|)"
            R"([a-z][\u0585\u0581]+[a-z]|)"
             R"(^[og]+[\p{scx=armn}]|[\p{scx=armn}][og]+$|)"
             R"([\p{scx=armn}][og]+[\p{scx=armn}]|)"
             R"([\p{sc=cans}].*[a-z]|[a-z].*[\p{sc=cans}]|)"
            R"([\p{sc=tfng}].*[a-z]|[a-z].*[\p{sc=tfng}]|)"
             R"([^\p{scx=latn}\p{scx=grek}\p{scx=cyrl}][\u0300-\u0339])",
             -1, US_INV),
         0, status);
    tls_index.Set(dangerous_pattern);
  }
  dangerous_pattern->reset(label_string);
  return !dangerous_pattern->find();
}
