bool MediaElementAudioSourceHandler::PassesCurrentSrcCORSAccessCheck(
    const KURL& current_src) {
  DCHECK(IsMainThread());
  return Context()->GetSecurityOrigin() &&
         Context()->GetSecurityOrigin()->CanRequest(current_src);
 }
