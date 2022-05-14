void ImageInputType::ensurePrimaryContent()
{
     if (!m_useFallbackContent)
         return;
     m_useFallbackContent = false;
    if (ShadowRoot* root = element().userAgentShadowRoot())
        root->removeChildren();
    createShadowSubtree();
     reattachFallbackContent();
 }
