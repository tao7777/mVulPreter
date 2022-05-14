bool LocalFrame::ShouldReuseDefaultView(const KURL& url) const {
   if (!Loader().StateMachine()->IsDisplayingInitialEmptyDocument())
     return false;
 
   return GetDocument()->IsSecureTransitionTo(url);
 }
