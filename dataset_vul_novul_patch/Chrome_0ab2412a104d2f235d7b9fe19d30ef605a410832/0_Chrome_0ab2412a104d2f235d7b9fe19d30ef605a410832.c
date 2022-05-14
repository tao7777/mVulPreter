void Document::InitContentSecurityPolicy(ContentSecurityPolicy* csp) {
//// the first parameter specifies a policy to use as the document csp meaning
//// the document will take ownership of the policy
//// the second parameter specifies a policy to inherit meaning the document
//// will attempt to copy over the policy
void Document::InitContentSecurityPolicy(
    ContentSecurityPolicy* csp,
    const ContentSecurityPolicy* policy_to_inherit) {
   SetContentSecurityPolicy(csp ? csp : ContentSecurityPolicy::Create());
 
  if (policy_to_inherit) {
    GetContentSecurityPolicy()->CopyStateFrom(policy_to_inherit);
  } else if (frame_) {
     Frame* inherit_from = frame_->Tree().Parent() ? frame_->Tree().Parent()
                                                   : frame_->Client()->Opener();
     if (inherit_from && frame_ != inherit_from) {
       DCHECK(inherit_from->GetSecurityContext() &&
              inherit_from->GetSecurityContext()->GetContentSecurityPolicy());
      policy_to_inherit =
           inherit_from->GetSecurityContext()->GetContentSecurityPolicy();
       if (url_.IsEmpty() || url_.ProtocolIsAbout() || url_.ProtocolIsData() ||
           url_.ProtocolIs("blob") || url_.ProtocolIs("filesystem")) {
         GetContentSecurityPolicy()->CopyStateFrom(policy_to_inherit);
       }
     }
   }
  // Plugin documents inherit their parent/opener's 'plugin-types' directive
  // regardless of URL.
  if (policy_to_inherit && IsPluginDocument())
    GetContentSecurityPolicy()->CopyPluginTypesFrom(policy_to_inherit);

   GetContentSecurityPolicy()->BindToExecutionContext(this);
 }
