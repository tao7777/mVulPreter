void FrameLoaderClient::windowObjectCleared()
{
    g_signal_emit_by_name(m_frame, "cleared");

    Frame* coreFrame = core(m_frame);
    ASSERT(coreFrame);

    Settings* settings = coreFrame->settings();
    if (!settings || !settings->isJavaScriptEnabled())
        return;
 
    JSGlobalContextRef context = toGlobalRef(coreFrame->script()->globalObject(mainThreadNormalWorld())->globalExec());
    JSObjectRef windowObject = toRef(coreFrame->script()->globalObject(mainThreadNormalWorld()));
     ASSERT(windowObject);
 
     WebKitWebView* webView = getViewFromFrame(m_frame);
    g_signal_emit_by_name(webView, "window-object-cleared", m_frame, context, windowObject);

}
