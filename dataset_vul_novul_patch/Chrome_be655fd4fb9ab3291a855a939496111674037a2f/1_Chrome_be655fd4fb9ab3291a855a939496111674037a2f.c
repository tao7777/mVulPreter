void FrameLoader::detachDocumentLoader(Member<DocumentLoader>& loader)
{
     if (!loader)
         return;
 
     loader->detachFromFrame();
     loader = nullptr;
 }
