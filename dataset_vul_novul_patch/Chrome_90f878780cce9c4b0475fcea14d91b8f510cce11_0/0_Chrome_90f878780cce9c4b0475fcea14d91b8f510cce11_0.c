bool LocalFrame::ShouldReuseDefaultView(const KURL& url) const {
bool LocalFrame::ShouldReuseDefaultView(
    const KURL& url,
    const ContentSecurityPolicy* csp) const {
   if (!Loader().StateMachine()->IsDisplayingInitialEmptyDocument())
     return false;
 
  // The Window object should only be re-used if it is same-origin.
  // Since sandboxing turns the origin into an opaque origin it needs to also
  // be considered when deciding whether to reuse it.
  // Spec:
  // https://html.spec.whatwg.org/multipage/browsing-the-web.html#initialise-the-document-object
  if (csp &&
      SecurityContext::IsSandboxed(kSandboxOrigin, csp->GetSandboxMask())) {
    return false;
  }

   return GetDocument()->IsSecureTransitionTo(url);
 }
