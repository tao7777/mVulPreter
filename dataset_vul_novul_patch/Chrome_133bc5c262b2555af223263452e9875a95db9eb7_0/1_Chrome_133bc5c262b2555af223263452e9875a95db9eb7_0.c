static bool TokenExitsSVG(const CompactHTMLToken& token) {
  return DeprecatedEqualIgnoringCase(token.Data(),
                                     SVGNames::foreignObjectTag.LocalName());
}
