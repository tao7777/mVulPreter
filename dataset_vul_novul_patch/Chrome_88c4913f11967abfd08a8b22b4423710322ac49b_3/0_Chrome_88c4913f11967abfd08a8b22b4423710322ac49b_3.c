     CCLayerTreeHostTest()
         : m_beginning(false)
         , m_endWhenBeginReturns(false)
         , m_timedOut(false)
     {
         m_webThread = adoptPtr(webKitPlatformSupport()->createThread("CCLayerTreeHostTest"));
         WebCompositor::setThread(m_webThread.get());
#ifndef NDEBUG
        CCProxy::setMainThread(currentThread());
 #endif
        ASSERT(CCProxy::isMainThread());
        m_mainThreadProxy = CCScopedMainThreadProxy::create();
     }
