bool HTMLMediaElement::isAutoplayAllowedPerSettings() const {
  LocalFrame* frame = document().frame();
   if (!frame)
     return false;
   FrameLoaderClient* frameLoaderClient = frame->loader().client();
  return frameLoaderClient && frameLoaderClient->allowAutoplay(false);
 }
