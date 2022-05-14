 bool CSSStyleSheet::CanAccessRules() const {
   if (enable_rule_access_for_inspector_)
     return true;

  // Opaque responses should never be accessible, mod DevTools. See comments for
  // IsOpaqueResponseFromServiceWorker().
  if (contents_->IsOpaqueResponseFromServiceWorker())
    return false;

   if (is_inline_stylesheet_)
     return true;
   KURL base_url = contents_->BaseURL();
  if (base_url.IsEmpty())
    return true;
  Document* document = OwnerDocument();
  if (!document)
    return true;
  if (document->GetSecurityOrigin()->CanReadContent(base_url))
    return true;
  if (allow_rule_access_from_origin_ &&
      document->GetSecurityOrigin()->CanAccess(
          allow_rule_access_from_origin_.get())) {
    return true;
  }
  return false;
}
