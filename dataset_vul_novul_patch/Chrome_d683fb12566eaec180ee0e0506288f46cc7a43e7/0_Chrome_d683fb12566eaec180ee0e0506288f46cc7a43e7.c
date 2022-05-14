 void Document::InitContentSecurityPolicy(
     ContentSecurityPolicy* csp,
    const ContentSecurityPolicy* policy_to_inherit,
    const ContentSecurityPolicy* previous_document_csp) {
   SetContentSecurityPolicy(csp ? csp : ContentSecurityPolicy::Create());
 
   GetContentSecurityPolicy()->BindToExecutionContext(this);

   if (policy_to_inherit) {
     GetContentSecurityPolicy()->CopyStateFrom(policy_to_inherit);
  } else {
    if (frame_) {
      Frame* inherit_from = frame_->Tree().Parent()
                                ? frame_->Tree().Parent()
                                : frame_->Client()->Opener();
      if (inherit_from && frame_ != inherit_from) {
        DCHECK(inherit_from->GetSecurityContext() &&
               inherit_from->GetSecurityContext()->GetContentSecurityPolicy());
        policy_to_inherit =
            inherit_from->GetSecurityContext()->GetContentSecurityPolicy();
       }
     }

    // If we don't have an opener or parent, inherit from the previous
    // document CSP.
    if (!policy_to_inherit)
      policy_to_inherit = previous_document_csp;

    // We should inherit the relevant CSP if the document is loaded using
    // a local-scheme url.
    if (policy_to_inherit &&
        (url_.IsEmpty() || url_.ProtocolIsAbout() || url_.ProtocolIsData() ||
         url_.ProtocolIs("blob") || url_.ProtocolIs("filesystem")))
      GetContentSecurityPolicy()->CopyStateFrom(policy_to_inherit);
   }
  if (policy_to_inherit && IsPluginDocument())
    GetContentSecurityPolicy()->CopyPluginTypesFrom(policy_to_inherit);
}
