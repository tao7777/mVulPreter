IDNSpoofChecker::IDNSpoofChecker() {
  UErrorCode status = U_ZERO_ERROR;
  checker_ = uspoof_open(&status);
  if (U_FAILURE(status)) {
    checker_ = nullptr;
    return;
  }


  uspoof_setRestrictionLevel(checker_, USPOOF_HIGHLY_RESTRICTIVE);

  SetAllowedUnicodeSet(&status);

  int32_t checks = uspoof_getChecks(checker_, &status) | USPOOF_AUX_INFO;
  uspoof_setChecks(checker_, checks, &status);

  deviation_characters_ = icu::UnicodeSet(
      UNICODE_STRING_SIMPLE("[\\u00df\\u03c2\\u200c\\u200d]"), status);
  deviation_characters_.freeze();

  non_ascii_latin_letters_ =
      icu::UnicodeSet(UNICODE_STRING_SIMPLE("[[:Latin:] - [a-zA-Z]]"), status);
  non_ascii_latin_letters_.freeze();

  kana_letters_exceptions_ = icu::UnicodeSet(
      UNICODE_STRING_SIMPLE("[\\u3078-\\u307a\\u30d8-\\u30da\\u30fb-\\u30fe]"),
      status);
  kana_letters_exceptions_.freeze();
  combining_diacritics_exceptions_ =
      icu::UnicodeSet(UNICODE_STRING_SIMPLE("[\\u0300-\\u0339]"), status);
  combining_diacritics_exceptions_.freeze();

  cyrillic_letters_latin_alike_ = icu::UnicodeSet(
      icu::UnicodeString::fromUTF8("[асԁеһіјӏорԛѕԝхуъЬҽпгѵѡ]"), status);
  cyrillic_letters_latin_alike_.freeze();

  cyrillic_letters_ =
      icu::UnicodeSet(UNICODE_STRING_SIMPLE("[[:Cyrl:]]"), status);
  cyrillic_letters_.freeze();

  DCHECK(U_SUCCESS(status));
  lgc_letters_n_ascii_ = icu::UnicodeSet(
      UNICODE_STRING_SIMPLE("[[:Latin:][:Greek:][:Cyrillic:][0-9\\u002e_"
                            "\\u002d][\\u0300-\\u0339]]"),
      status);
  lgc_letters_n_ascii_.freeze();

  UParseError parse_error;
  diacritic_remover_.reset(icu::Transliterator::createFromRules(
      UNICODE_STRING_SIMPLE("DropAcc"),
      icu::UnicodeString::fromUTF8("::NFD; ::[:Nonspacing Mark:] Remove; ::NFC;"
                                   " ł > l; ø > o; đ > d;"),
      UTRANS_FORWARD, parse_error, status));

  //   - U+043F (п) => n
  //   - {U+0454 (є), U+04BD (ҽ), U+04BF (ҿ)} => e
      UNICODE_STRING_SIMPLE("ExtraConf"),
       icu::UnicodeString::fromUTF8("[þϼҏ] > p; [ħнћңҥӈԧԩ] > h;"
                                    "[ĸκкқҝҟҡӄԟ] > k; [ŧтҭ] > t;"
                                    "[ƅьҍв] > b;  [ωшщ] > w; [мӎ] > m;"
                                   "п > n; [єҽҿ] > e; ґ > r; ғ > f; ҫ > c;"
                                   "ұ > y; [χҳӽӿ] > x;"
 #if defined(OS_WIN)
                                    "ӏ > i;"
 #else
                                   "ӏ > l;"
#endif
                                   "ԃ  > d; ԍ > g; ട > s"),
      UTRANS_FORWARD, parse_error, status));
  DCHECK(U_SUCCESS(status))
      << "Spoofchecker initalization failed due to an error: "
      << u_errorName(status);
}
