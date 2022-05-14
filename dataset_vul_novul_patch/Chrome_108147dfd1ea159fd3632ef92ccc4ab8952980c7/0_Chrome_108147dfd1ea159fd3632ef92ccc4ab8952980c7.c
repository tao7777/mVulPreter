void Document::InitContentSecurityPolicy(
void Document::InitContentSecurityPolicy(ContentSecurityPolicy* csp,
                                         const Document* origin_document) {
   SetContentSecurityPolicy(csp ? csp : ContentSecurityPolicy::Create());
 
   GetContentSecurityPolicy()->BindToExecutionContext(this);
 
  ContentSecurityPolicy* policy_to_inherit = nullptr;
  if (origin_document)
    policy_to_inherit = origin_document->GetContentSecurityPolicy();

  // We should inherit the navigation initiator CSP if the document is loaded
  // using a local-scheme url.
  if (policy_to_inherit &&
      (url_.IsEmpty() || url_.ProtocolIsAbout() || url_.ProtocolIsData() ||
       url_.ProtocolIs("blob") || url_.ProtocolIs("filesystem"))) {
     GetContentSecurityPolicy()->CopyStateFrom(policy_to_inherit);
  }

  if (IsPluginDocument()) {
    // TODO(andypaicu): This should inherit the origin document's plugin types
    // but because this could be a OOPIF document it might not have access.
    // In this situation we fallback on using the parent/opener.
    if (origin_document) {
      GetContentSecurityPolicy()->CopyPluginTypesFrom(
          origin_document->GetContentSecurityPolicy());
    } else if (frame_) {
       Frame* inherit_from = frame_->Tree().Parent()
                                 ? frame_->Tree().Parent()
                                 : frame_->Client()->Opener();
       if (inherit_from && frame_ != inherit_from) {
         DCHECK(inherit_from->GetSecurityContext() &&
                inherit_from->GetSecurityContext()->GetContentSecurityPolicy());
        GetContentSecurityPolicy()->CopyPluginTypesFrom(
            inherit_from->GetSecurityContext()->GetContentSecurityPolicy());
       }
     }
  }
 }
