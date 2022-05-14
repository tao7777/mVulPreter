void CCThreadProxy::stop()
{
    TRACE_EVENT("CCThreadProxy::stop", this, 0);
    ASSERT(isMainThread());
    ASSERT(m_started);

    CCCompletionEvent completion;
     s_ccThread->postTask(createCCThreadTask(this, &CCThreadProxy::layerTreeHostClosedOnCCThread, AllowCrossThreadAccess(&completion)));
     completion.wait();
 
    m_mainThreadProxy->shutdown(); // Stop running tasks posted to us.

     ASSERT(!m_layerTreeHostImpl); // verify that the impl deleted.
     m_layerTreeHost = 0;
     m_started = false;
}
