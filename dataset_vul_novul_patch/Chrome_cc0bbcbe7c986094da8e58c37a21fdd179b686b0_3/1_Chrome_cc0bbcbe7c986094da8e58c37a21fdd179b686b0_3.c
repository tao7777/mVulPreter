bool IsIDNComponentSafe(base::StringPiece16 label, bool is_tld_ascii) {
  return g_idn_spoof_checker.Get().SafeToDisplayAsUnicode(label, is_tld_ascii);
 }
