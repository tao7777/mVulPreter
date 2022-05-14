IDNSpoofChecker::IDNSpoofChecker() {
  UErrorCode status = U_ZERO_ERROR;
  checker_ = uspoof_open(&status);
  if (U_FAILURE(status)) {
    checker_ = nullptr;
    return;
  }


  uspoof_setRestrictionLevel(checker_, USPOOF_MODERATELY_RESTRICTIVE);

  SetAllowedUnicodeSet(&status);

  int32_t checks = uspoof_getChecks(checker_, &status) | USPOOF_AUX_INFO;
  uspoof_setChecks(checker_, checks, &status);

  deviation_characters_ =
      icu::UnicodeSet(UNICODE_STRING_SIMPLE("[\\u00df\\u03c2\\u200c\\u200d]"),
                      status);
  deviation_characters_.freeze();

  non_ascii_latin_letters_ = icu::UnicodeSet(
      UNICODE_STRING_SIMPLE("[[:Latin:] - [a-zA-Z]]"), status);
  non_ascii_latin_letters_.freeze();

  kana_letters_exceptions_ = icu::UnicodeSet(UNICODE_STRING_SIMPLE(
       "[\\u3078-\\u307a\\u30d8-\\u30da\\u30fb\\u30fc]"), status);
   kana_letters_exceptions_.freeze();
 
   DCHECK(U_SUCCESS(status));
 }
