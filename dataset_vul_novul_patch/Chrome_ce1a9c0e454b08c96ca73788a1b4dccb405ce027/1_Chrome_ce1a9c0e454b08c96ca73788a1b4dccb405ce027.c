 ImageLoader::ImageLoader(Element* element)
     : m_element(element),
      m_derefElementTimer(this, &ImageLoader::timerFired),
       m_hasPendingLoadEvent(false),
       m_hasPendingErrorEvent(false),
       m_imageComplete(true),
      m_loadingImageDocument(false),
      m_elementIsProtected(false),
      m_suppressErrorEvents(false) {
  RESOURCE_LOADING_DVLOG(1) << "new ImageLoader " << this;
}
