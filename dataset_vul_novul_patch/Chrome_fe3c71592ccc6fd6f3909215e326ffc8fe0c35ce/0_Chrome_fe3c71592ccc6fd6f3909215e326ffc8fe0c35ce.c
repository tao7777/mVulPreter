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
 
  //   - {U+00FE (þ), U+03FC (ϼ), U+048F (ҏ)} => p
  //   - {U+0127 (ħ), U+043D (н), U+045B (ћ), U+04A3 (ң), U+04A5 (ҥ),
  //      U+04C8 (ӈ), U+0527 (ԧ), U+0529 (ԩ)} => h
  //   - {U+0138 (ĸ), U+03BA (κ), U+043A (к), U+049B (қ), U+049D (ҝ),
  //      U+049F (ҟ), U+04A1(ҡ), U+04C4 (ӄ), U+051F (ԟ)} => k
  //   - {U+0167 (ŧ), U+0442 (т), U+04AD (ҭ)} => t
  //   - {U+0185 (ƅ), U+044C (ь), U+048D (ҍ), U+0432 (в)} => b
  //   - {U+03C9 (ω), U+0448 (ш), U+0449 (щ)} => w
  //   - {U+043C (м), U+04CE (ӎ)} => m
  //   - U+0491 (ґ) => r
  //   - U+0493 (ғ) => f
  //   - U+04AB (ҫ) => c
  //   - U+04B1 (ұ) => y
  //   - U+03C7 (χ), U+04B3 (ҳ), U+04FD (ӽ), U+04FF (ӿ) => x
  //   - U+04BD (ҽ), U+04BF (ҿ) => e
  //   - U+04CF (ӏ) => l
  //   - U+0503 (ԃ) => d
  //   - U+050D (ԍ) => g
   extra_confusable_mapper_.reset(icu::Transliterator::createFromRules(
       UNICODE_STRING_SIMPLE("ExtraConf"),
      icu::UnicodeString::fromUTF8("[þϼҏ] > p; [ħнћңҥӈԧԩ] > h;"
                                   "[ĸκкқҝҟҡӄԟ] > k; [ŧтҭ] > t;"
                                   "[ƅьҍв] > b;  [ωшщ] > w; [мӎ] > m;"
                                   "п > n; ћ > h; ґ > r; ғ > f; ҫ > c;"
                                   "ұ > y; [χҳӽӿ] > x; [ҽҿ] > e; ӏ > l;"
                                   "ԃ  > d; ԍ > g; ട > s"),
       UTRANS_FORWARD, parse_error, status));
   DCHECK(U_SUCCESS(status))
       << "Spoofchecker initalization failed due to an error: "
      << u_errorName(status);
}
