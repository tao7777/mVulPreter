 PassRefPtr<CSSRuleList> CSSStyleSheet::cssRules(bool omitCharsetRules)
 {
    KURL url = finalURL();
    if (!url.isEmpty() && doc() && !doc()->securityOrigin()->canRequest(url))
         return 0;
     return CSSRuleList::create(this, omitCharsetRules);
 }
