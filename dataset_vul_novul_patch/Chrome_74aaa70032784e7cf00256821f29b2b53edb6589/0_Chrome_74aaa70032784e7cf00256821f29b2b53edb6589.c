 void ParamTraits<GURL>::Write(Message* m, const GURL& p) {
   DCHECK(p.possibly_invalid_spec().length() <= content::kMaxURLChars);

  // Beware of print-parse inconsistency which would change an invalid
  // URL into a valid one. Ideally, the message would contain this flag
  // so that the read side could make the check, but performing it here
  // avoids changing the on-the-wire representation of such a fundamental
  // type as GURL. See https://crbug.com/166486 for additional work in
  // this area.
  if (!p.is_valid()) {
    GURL reconstructed_url(p.possibly_invalid_spec());
    if (reconstructed_url.is_valid()) {
      DLOG(WARNING) << "GURL string " << p.possibly_invalid_spec()
                    << " (marked invalid) but parsed as valid.";
      m->WriteString(std::string());
      return;
    }
  }

   m->WriteString(p.possibly_invalid_spec());
 }
