bool RendererPermissionsPolicyDelegate::IsRestrictedUrl(
    const GURL& document_url,
    std::string* error) {
  if (dispatcher_->IsExtensionActive(kWebStoreAppId)) {
    if (error)
      *error = errors::kCannotScriptGallery;
     return true;
   }
 
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          ::switches::kInstantProcess)) {
     if (error)
       *error = errors::kCannotScriptNtp;
     return true;
  }

  return false;
}
