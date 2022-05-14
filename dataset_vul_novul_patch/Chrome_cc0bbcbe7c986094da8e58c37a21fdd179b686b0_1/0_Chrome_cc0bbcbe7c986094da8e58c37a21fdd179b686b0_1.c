 bool IDNToUnicodeOneComponent(const base::char16* comp,
                               size_t comp_len,
                              base::StringPiece top_level_domain,
                               bool enable_spoof_checks,
                               base::string16* out,
                               bool* has_idn_component) {
  DCHECK(out);
  DCHECK(has_idn_component);
  *has_idn_component = false;
  if (comp_len == 0)
    return false;

  static const base::char16 kIdnPrefix[] = {'x', 'n', '-', '-'};
  if (comp_len <= base::size(kIdnPrefix) ||
      memcmp(comp, kIdnPrefix, sizeof(kIdnPrefix)) != 0) {
    out->append(comp, comp_len);
    return false;
  }

  UIDNA* uidna = g_uidna.Get().value;
  DCHECK(uidna != nullptr);
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
    *has_idn_component = true;
    out->resize(original_length + output_length);
    if (!enable_spoof_checks) {
      return true;
    }
     if (IsIDNComponentSafe(
             base::StringPiece16(out->data() + original_length,
                                 base::checked_cast<size_t>(output_length)),
            top_level_domain)) {
       return true;
     }
   }

  out->resize(original_length);
  out->append(comp, comp_len);
  return false;
}
