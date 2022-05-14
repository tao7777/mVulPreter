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
 
  LocalFrame* frame = document.GetFrame();
  if (frame && frame->Tree().Parent() && document.IsPluginDocument()) {
    ContentSecurityPolicy* parent_csp = frame->Tree()
                                            .Parent()
                                            ->GetSecurityContext()
                                            ->GetContentSecurityPolicy();
    if (parent_csp && !parent_csp->AllowPluginType(type, type_attribute, url,
                                                   reporting_policy))
      return false;
  }
   return true;
 }
