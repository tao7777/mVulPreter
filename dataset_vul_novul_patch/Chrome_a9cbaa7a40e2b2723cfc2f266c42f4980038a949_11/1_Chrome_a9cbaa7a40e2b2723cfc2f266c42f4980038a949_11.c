 bool HTMLMediaElement::IsMediaDataCORSSameOrigin(
     const SecurityOrigin* origin) const {
  if (GetWebMediaPlayer() &&
      GetWebMediaPlayer()->DidGetOpaqueResponseFromServiceWorker()) {
    return false;
  }
 
  if (!HasSingleSecurityOrigin())
     return false;
 
  return (GetWebMediaPlayer() &&
          GetWebMediaPlayer()->DidPassCORSAccessCheck()) ||
         origin->CanReadContent(currentSrc());
 }
