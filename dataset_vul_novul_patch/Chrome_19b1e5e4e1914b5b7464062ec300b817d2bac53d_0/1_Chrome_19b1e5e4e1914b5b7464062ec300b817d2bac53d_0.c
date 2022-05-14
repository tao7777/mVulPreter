bool DoCanonicalizePathURL(const URLComponentSource<CHAR>& source,
                           const Parsed& parsed,
                           CanonOutput* output,
                           Parsed* new_parsed) {
  bool success = CanonicalizeScheme(source.scheme, parsed.scheme,
                                    output, &new_parsed->scheme);

  new_parsed->username.reset();
  new_parsed->password.reset();
  new_parsed->host.reset();
   new_parsed->port.reset();
  success &= DoCanonicalizePathComponent<CHAR, UCHAR>(
      source.path, parsed.path, '\0', output, &new_parsed->path);
  success &= DoCanonicalizePathComponent<CHAR, UCHAR>(
      source.query, parsed.query, '?', output, &new_parsed->query);
  success &= DoCanonicalizePathComponent<CHAR, UCHAR>(
      source.ref, parsed.ref, '#', output, &new_parsed->ref);
 
   return success;
 }
