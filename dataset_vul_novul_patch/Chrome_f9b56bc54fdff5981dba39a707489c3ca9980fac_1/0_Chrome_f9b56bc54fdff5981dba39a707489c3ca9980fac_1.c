 bool IDNSpoofChecker::SimilarToTopDomains(base::StringPiece16 hostname) {
   size_t hostname_length = hostname.length() - (hostname.back() == '.' ? 1 : 0);
  icu::UnicodeString host(FALSE, hostname.data(), hostname_length);
  if (lgc_letters_n_ascii_.span(host, 0, USET_SPAN_CONTAINED) == host.length())
    diacritic_remover_.get()->transliterate(host);
  extra_confusable_mapper_.get()->transliterate(host);
 
   UErrorCode status = U_ZERO_ERROR;
  icu::UnicodeString skeleton;

  // Map U+04CF (ӏ) to lowercase L in addition to what uspoof_getSkeleton does
  // (mapping it to lowercase I).
  int32_t u04cf_pos;
  if ((u04cf_pos = host.indexOf(0x4CF)) != -1) {
    icu::UnicodeString host_alt(host);
    size_t length = host_alt.length();
    char16_t* buffer = host_alt.getBuffer(-1);
    for (char16_t* uc = buffer + u04cf_pos ; uc < buffer + length; ++uc) {
      if (*uc == 0x4CF)
        *uc = 0x6C;  // Lowercase L
    }
    host_alt.releaseBuffer(length);
    uspoof_getSkeletonUnicodeString(checker_, 0, host_alt, skeleton, &status);
    if (U_SUCCESS(status) && LookupMatchInTopDomains(skeleton))
      return true;
  }

  uspoof_getSkeletonUnicodeString(checker_, 0, host, skeleton, &status);
  return U_SUCCESS(status) && LookupMatchInTopDomains(skeleton);
 }
