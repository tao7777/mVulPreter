void FrameLoader::receivedFirstData()
{
    begin(m_workingURL, false);

    dispatchDidCommitLoad();
    dispatchWindowObjectAvailable();
    
    if (m_documentLoader) {
        String ptitle = m_documentLoader->title();
        if (!ptitle.isNull())
            m_client->dispatchDidReceiveTitle(ptitle);
    }

    m_workingURL = KURL();

    double delay;
     String url;
     if (!m_documentLoader)
         return;
    if (m_frame->inViewSourceMode())
        return;
     if (!parseHTTPRefresh(m_documentLoader->response().httpHeaderField("Refresh"), false, delay, url))
         return;
 
    if (url.isEmpty())
        url = m_URL.string();
    else
        url = m_frame->document()->completeURL(url).string();

    m_frame->redirectScheduler()->scheduleRedirect(delay, url);
}
