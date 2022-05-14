 PassRefPtr<CSSRuleList> CSSStyleSheet::cssRules(bool omitCharsetRules)
 {
    if (doc() && !doc()->securityOrigin()->canRequest(baseURL()))
         return 0;
     return CSSRuleList::create(this, omitCharsetRules);
 }
