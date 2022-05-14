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

  //   - {U+03C9 (ω), U+0448 (ш), U+0449 (щ), U+0E1E (พ),
  //      U+0E1F (ฟ), U+0E9E (ພ), U+0E9F (ຟ)} => w
  //   - {U+0D1F (ട), U+0E23 (ร), U+0EA3 (ຣ), U+0EAE (ຮ)} => s
  //   - {U+0E1A (บ), U+0E9A (ບ)} => u
   extra_confusable_mapper_.reset(icu::Transliterator::createFromRules(
       UNICODE_STRING_SIMPLE("ExtraConf"),
       icu::UnicodeString::fromUTF8(
           "[æӕ] > ae; [þϼҏ] > p; [ħнћңҥӈӊԋԧԩ] > h;"
           "[ĸκкқҝҟҡӄԟ] > k; [ŋпԥ] > n; œ > ce;"
          "[ŧтҭԏ] > t; [ƅьҍв] > b;  [ωшщพฟພຟ] > w;"
           "[мӎ] > m; [єҽҿၔ] > e; ґ > r; [ғӻ] > f;"
           "[ҫင] > c; ұ > y; [χҳӽӿ] > x;"
          "ԃ  > d; [ԍဌ] > g; [ടรຣຮ] > s; ၂ > j;"
          "[зҙӡ] > 3; [บບ] > u"),
       UTRANS_FORWARD, parse_error, status));
   DCHECK(U_SUCCESS(status))
       << "Spoofchecker initalization failed due to an error: "
      << u_errorName(status);
}
