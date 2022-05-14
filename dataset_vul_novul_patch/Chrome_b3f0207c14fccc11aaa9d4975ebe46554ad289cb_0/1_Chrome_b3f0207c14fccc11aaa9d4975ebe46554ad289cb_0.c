bool IDNSpoofChecker::SimilarToTopDomains(base::StringPiece16 hostname) {
  size_t hostname_length = hostname.length() - (hostname.back() == '.' ? 1 : 0);
  icu::UnicodeString ustr_host(FALSE, hostname.data(), hostname_length);
   if (lgc_letters_n_ascii_.span(ustr_host, 0, USET_SPAN_CONTAINED) ==
       ustr_host.length())
    transliterator_.get()->transliterate(ustr_host);
 
   UErrorCode status = U_ZERO_ERROR;
   icu::UnicodeString ustr_skeleton;
  uspoof_getSkeletonUnicodeString(checker_, 0, ustr_host, ustr_skeleton,
                                  &status);
   if (U_FAILURE(status))
     return false;
   std::string skeleton;
  ustr_skeleton.toUTF8String(skeleton);
  return LookupMatchInTopDomains(skeleton);
 }
