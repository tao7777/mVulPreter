void ImageInputType::ensurePrimaryContent()
{
     if (!m_useFallbackContent)
         return;
     m_useFallbackContent = false;
     reattachFallbackContent();
 }
