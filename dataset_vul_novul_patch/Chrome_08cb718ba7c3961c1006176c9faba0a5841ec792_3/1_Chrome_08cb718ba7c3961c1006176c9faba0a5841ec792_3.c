bool IsIDNComponentSafe(base::StringPiece16 label) {
  return g_idn_spoof_checker.Get().Check(label);
 }
