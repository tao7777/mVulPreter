 bool IDNToUnicodeOneComponent(const base::char16* comp,
                               size_t comp_len,
                               base::string16* out) {
   DCHECK(out);
   if (comp_len == 0)
    return false;

  static const base::char16 kIdnPrefix[] = {'x', 'n', '-', '-'};
  if ((comp_len > arraysize(kIdnPrefix)) &&
      !memcmp(comp, kIdnPrefix, sizeof(kIdnPrefix))) {
    UIDNA* uidna = g_uidna.Get().value;
    DCHECK(uidna != NULL);
    size_t original_length = out->length();
    int32_t output_length = 64;
    UIDNAInfo info = UIDNA_INFO_INITIALIZER;
    UErrorCode status;
    do {
      out->resize(original_length + output_length);
      status = U_ZERO_ERROR;
      output_length = uidna_labelToUnicode(
          uidna, comp, static_cast<int32_t>(comp_len), &(*out)[original_length],
          output_length, &info, &status);
    } while ((status == U_BUFFER_OVERFLOW_ERROR && info.errors == 0));

    if (U_SUCCESS(status) && info.errors == 0) {
       out->resize(original_length + output_length);
       if (IsIDNComponentSafe(
          base::StringPiece16(out->data() + original_length,
                              base::checked_cast<size_t>(output_length))))
         return true;
     }
 
    out->resize(original_length);
  }

  out->append(comp, comp_len);
  return false;
}
