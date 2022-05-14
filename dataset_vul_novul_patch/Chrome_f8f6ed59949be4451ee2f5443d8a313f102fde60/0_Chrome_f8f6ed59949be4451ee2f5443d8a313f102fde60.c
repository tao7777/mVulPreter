void DoCanonicalizeRef(const CHAR* spec,
                       const Component& ref,
                       CanonOutput* output,
                       Component* out_ref) {
  if (ref.len < 0) {
    *out_ref = Component();
    return;
  }

  output->push_back('#');
  out_ref->begin = output->length();

  int end = ref.end();
  for (int i = ref.begin; i < end; i++) {
    if (spec[i] == 0) {
      continue;
    } else if (static_cast<UCHAR>(spec[i]) < 0x20) {
      AppendEscapedChar(static_cast<unsigned char>(spec[i]), output);
    } else if (static_cast<UCHAR>(spec[i]) < 0x80) {
       output->push_back(static_cast<char>(spec[i]));
     } else {
      AppendUTF8EscapedChar(spec, &i, end, output);
     }
   }
 
  out_ref->len = output->length() - out_ref->begin;
}
