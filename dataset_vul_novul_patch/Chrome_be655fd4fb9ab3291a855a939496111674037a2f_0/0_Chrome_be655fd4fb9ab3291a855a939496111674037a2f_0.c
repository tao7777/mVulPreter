bool FrameLoader::prepareForCommit()
{
    PluginScriptForbiddenScope forbidPluginDestructorScripting;
    DocumentLoader* pdl = m_provisionalDocumentLoader;

    if (m_frame->document()) {
        unsigned nodeCount = 0;
        for (Frame* frame = m_frame; frame; frame = frame->tree().traverseNext()) {
            if (frame->isLocalFrame()) {
                LocalFrame* localFrame = toLocalFrame(frame);
                nodeCount += localFrame->document()->nodeCount();
            }
        }
        unsigned totalNodeCount = InstanceCounters::counterValue(InstanceCounters::NodeCounter);
        float ratio = static_cast<float>(nodeCount) / totalNodeCount;
        ThreadState::current()->schedulePageNavigationGCIfNeeded(ratio);
    }

    SubframeLoadingDisabler disabler(m_frame->document());
    if (m_documentLoader) {
        client()->dispatchWillClose();
        dispatchUnloadEvent();
    }
    m_frame->detachChildren();
    if (pdl != m_provisionalDocumentLoader)
        return false;
     if (m_documentLoader) {
         TemporaryChange<bool> inDetachDocumentLoader(m_protectProvisionalLoader, true);
         detachDocumentLoader(m_documentLoader);
     }
    if (!m_frame->client())
        return false;
    ASSERT(m_provisionalDocumentLoader == pdl);
    if (m_frame->document())
        m_frame->document()->detach();
    m_documentLoader = m_provisionalDocumentLoader.release();
    takeObjectSnapshot();

    return true;
}
