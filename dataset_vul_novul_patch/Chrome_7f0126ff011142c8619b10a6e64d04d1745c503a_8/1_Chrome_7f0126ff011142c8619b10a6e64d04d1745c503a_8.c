bool WebPageSerializer::serialize(WebFrame* frame,
                                  bool recursive,
                                  WebPageSerializerClient* client,
                                  const WebVector<WebURL>& links,
                                   const WebVector<WebString>& localPaths,
                                   const WebString& localDirectoryName)
 {
    ASSERT(frame);
    ASSERT(client);
    ASSERT(links.size() == localPaths.size());
 
    LinkLocalPathMap m_localLinks;
 
    for (size_t i = 0; i < links.size(); i++) {
        KURL url = links[i];
        ASSERT(!m_localLinks.contains(url.string()));
        m_localLinks.set(url.string(), localPaths[i]);
    }
 
    Vector<SerializedResource> resources;
    PageSerializer serializer(&resources, &m_localLinks, localDirectoryName);
    serializer.serialize(toWebViewImpl(frame->view())->page());
 
    for (Vector<SerializedResource>::const_iterator iter = resources.begin(); iter != resources.end(); ++iter) {
        client->didSerializeDataForFrame(iter->url, WebCString(iter->data->data(), iter->data->size()), WebPageSerializerClient::CurrentFrameIsFinished);
     }
    client->didSerializeDataForFrame(KURL(), WebCString("", 0), WebPageSerializerClient::AllFramesAreFinished);
     return true;
 }
