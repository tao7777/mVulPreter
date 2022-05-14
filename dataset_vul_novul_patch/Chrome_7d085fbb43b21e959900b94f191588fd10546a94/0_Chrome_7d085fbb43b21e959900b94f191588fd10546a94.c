void ImageLoader::updateFromElement()
{
    Document* document = m_element->document();
    if (!document->renderer())
        return;

    AtomicString attr = m_element->imageSourceURL();

    if (attr == m_failedLoadURL)
        return;

    CachedResourceHandle<CachedImage> newImage = 0;
    if (!attr.isNull() && !stripLeadingAndTrailingHTMLSpaces(attr).isEmpty()) {
        CachedResourceRequest request(ResourceRequest(document->completeURL(sourceURI(attr))));
        request.setInitiator(element());

        String crossOriginMode = m_element->fastGetAttribute(HTMLNames::crossoriginAttr);
        if (!crossOriginMode.isNull()) {
            StoredCredentials allowCredentials = equalIgnoringCase(crossOriginMode, "use-credentials") ? AllowStoredCredentials : DoNotAllowStoredCredentials;
            updateRequestForAccessControl(request.mutableResourceRequest(), document->securityOrigin(), allowCredentials);
        }

        if (m_loadManually) {
            bool autoLoadOtherImages = document->cachedResourceLoader()->autoLoadImages();
            document->cachedResourceLoader()->setAutoLoadImages(false);
            newImage = new CachedImage(request.resourceRequest());
            newImage->setLoading(true);
            newImage->setOwningCachedResourceLoader(document->cachedResourceLoader());
            document->cachedResourceLoader()->m_documentResources.set(newImage->url(), newImage.get());
            document->cachedResourceLoader()->setAutoLoadImages(autoLoadOtherImages);
        } else
            newImage = document->cachedResourceLoader()->requestImage(request);

        if (!newImage && !pageIsBeingDismissed(document)) {
            m_failedLoadURL = attr;
            m_hasPendingErrorEvent = true;
            errorEventSender().dispatchEventSoon(this);
        } else
             clearFailedLoadURL();
     } else if (!attr.isNull()) {
        m_hasPendingErrorEvent = true;
        errorEventSender().dispatchEventSoon(this);
     }
     
     CachedImage* oldImage = m_image.get();
    if (newImage != oldImage) {
        if (m_hasPendingBeforeLoadEvent) {
            beforeLoadEventSender().cancelEvent(this);
            m_hasPendingBeforeLoadEvent = false;
        }
        if (m_hasPendingLoadEvent) {
            loadEventSender().cancelEvent(this);
            m_hasPendingLoadEvent = false;
        }

        if (m_hasPendingErrorEvent && newImage) {
            errorEventSender().cancelEvent(this);
            m_hasPendingErrorEvent = false;
        }

        m_image = newImage;
        m_hasPendingBeforeLoadEvent = !m_element->document()->isImageDocument() && newImage;
        m_hasPendingLoadEvent = newImage;
        m_imageComplete = !newImage;

        if (newImage) {
            if (!m_element->document()->isImageDocument()) {
                if (!m_element->document()->hasListenerType(Document::BEFORELOAD_LISTENER))
                    dispatchPendingBeforeLoadEvent();
                else
                    beforeLoadEventSender().dispatchEventSoon(this);
            } else
                updateRenderer();

            newImage->addClient(this);
        }
        if (oldImage)
            oldImage->removeClient(this);
    }

    if (RenderImageResource* imageResource = renderImageResource())
        imageResource->resetAnimation();

    updatedHasPendingEvent();
}
