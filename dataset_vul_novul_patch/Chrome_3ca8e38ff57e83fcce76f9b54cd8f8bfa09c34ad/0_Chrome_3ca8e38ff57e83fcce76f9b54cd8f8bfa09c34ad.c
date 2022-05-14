bool DoResolveRelativeHost(const char* base_url,
                           const url_parse::Parsed& base_parsed,
                           const CHAR* relative_url,
                           const url_parse::Component& relative_component,
                           CharsetConverter* query_converter,
                           CanonOutput* output,
                           url_parse::Parsed* out_parsed) {
   url_parse::Parsed relative_parsed;  // Everything but the scheme is valid.
  url_parse::ParseAfterScheme(relative_url, relative_component.end(),
                              relative_component.begin, &relative_parsed);
 
  Replacements<CHAR> replacements;
  replacements.SetUsername(relative_url, relative_parsed.username);
  replacements.SetPassword(relative_url, relative_parsed.password);
  replacements.SetHost(relative_url, relative_parsed.host);
  replacements.SetPort(relative_url, relative_parsed.port);
  replacements.SetPath(relative_url, relative_parsed.path);
  replacements.SetQuery(relative_url, relative_parsed.query);
  replacements.SetRef(relative_url, relative_parsed.ref);

  return ReplaceStandardURL(base_url, base_parsed, replacements,
                            query_converter, output, out_parsed);
}
