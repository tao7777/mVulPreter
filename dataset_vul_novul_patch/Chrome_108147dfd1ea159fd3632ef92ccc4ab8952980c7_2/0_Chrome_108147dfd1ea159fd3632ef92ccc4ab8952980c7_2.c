bool ContentSecurityPolicy::AllowPluginTypeForDocument(
    const Document& document,
    const String& type,
    const String& type_attribute,
    const KURL& url,
    SecurityViolationReportingPolicy reporting_policy) const {
  if (document.GetContentSecurityPolicy() &&
      !document.GetContentSecurityPolicy()->AllowPluginType(
           type, type_attribute, url, reporting_policy))
     return false;
 
   return true;
 }
