void FrameLoader::detachDocumentLoader(Member<DocumentLoader>& loader)
{
     if (!loader)
         return;
 
    FrameNavigationDisabler navigationDisabler(*m_frame);
     loader->detachFromFrame();
     loader = nullptr;
 }
