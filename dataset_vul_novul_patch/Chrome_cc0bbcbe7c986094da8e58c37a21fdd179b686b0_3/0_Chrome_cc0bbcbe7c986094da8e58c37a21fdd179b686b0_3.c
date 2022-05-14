bool IsIDNComponentSafe(base::StringPiece16 label, bool is_tld_ascii) {
bool IsIDNComponentSafe(base::StringPiece16 label,
                        base::StringPiece top_level_domain) {
  return g_idn_spoof_checker.Get().SafeToDisplayAsUnicode(label,
                                                          top_level_domain);
 }
