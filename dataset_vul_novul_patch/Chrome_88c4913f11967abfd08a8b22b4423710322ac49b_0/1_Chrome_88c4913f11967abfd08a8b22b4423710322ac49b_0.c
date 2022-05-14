CCThreadProxy::CCThreadProxy(CCLayerTreeHost* layerTreeHost)
    : m_commitRequested(false)
    , m_layerTreeHost(layerTreeHost)
    , m_compositorIdentifier(-1)
     , m_started(false)
     , m_lastExecutedBeginFrameAndCommitSequenceNumber(-1)
     , m_numBeginFrameAndCommitsIssuedOnCCThread(0)
 {
     TRACE_EVENT("CCThreadProxy::CCThreadProxy", this, 0);
     ASSERT(isMainThread());
}
