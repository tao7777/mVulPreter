bool WebPageSerializer::serialize(WebFrame* frame,
                                  bool recursive,
                                  WebPageSerializerClient* client,
                                  const WebVector<WebURL>& links,
                                   const WebVector<WebString>& localPaths,
                                   const WebString& localDirectoryName)
 {
    WebPageSerializerImpl serializerImpl(
        frame, recursive, client, links, localPaths, localDirectoryName);
    return serializerImpl.serialize();
}
 
bool WebPageSerializer::retrieveAllResources(WebView* view,
                                             const WebVector<WebCString>& supportedSchemes,
                                             WebVector<WebURL>* resourceURLs,
                                             WebVector<WebURL>* frameURLs) {
    WebFrameImpl* mainFrame = toWebFrameImpl(view->mainFrame());
    if (!mainFrame)
        return false;
 
    Vector<Frame*> framesToVisit;
    Vector<Frame*> visitedFrames;
    Vector<KURL> frameKURLs;
    Vector<KURL> resourceKURLs;
 
    // Let's retrieve the resources from every frame in this page.
    framesToVisit.append(mainFrame->frame());
    while (!framesToVisit.isEmpty()) {
        Frame* frame = framesToVisit[0];
        framesToVisit.remove(0);
        retrieveResourcesForFrame(frame, supportedSchemes,
                                  &visitedFrames, &framesToVisit,
                                  &frameKURLs, &resourceKURLs);
    }
 
    // Converts the results to WebURLs.
    WebVector<WebURL> resultResourceURLs(resourceKURLs.size());
    for (size_t i = 0; i < resourceKURLs.size(); ++i) {
        resultResourceURLs[i] = resourceKURLs[i];
        // A frame's src can point to the same URL as another resource, keep the
        // resource URL only in such cases.
        size_t index = frameKURLs.find(resourceKURLs[i]);
        if (index != kNotFound)
            frameKURLs.remove(index);
     }
    *resourceURLs = resultResourceURLs;
    WebVector<WebURL> resultFrameURLs(frameKURLs.size());
    for (size_t i = 0; i < frameKURLs.size(); ++i)
        resultFrameURLs[i] = frameKURLs[i];
    *frameURLs = resultFrameURLs;

     return true;
 }
