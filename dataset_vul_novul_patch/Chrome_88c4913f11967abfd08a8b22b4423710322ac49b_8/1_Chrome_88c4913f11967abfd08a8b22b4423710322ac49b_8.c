    virtual void runTest()
     {
         webkit_support::PostDelayedTask(CCLayerTreeHostTest::onBeginTest, static_cast<void*>(this), 0);
        webkit_support::PostDelayedTask(CCLayerTreeHostTest::testTimeout, static_cast<void*>(this), 5000);
         webkit_support::RunMessageLoop();
        m_running = false;
        bool timedOut = m_timedOut; // Save whether we're timed out in case RunAllPendingMessages has the timeout.
         webkit_support::RunAllPendingMessages();
         ASSERT(!m_layerTreeHost.get());
         m_client.clear();
        if (timedOut) {
             FAIL() << "Test timed out";
             return;
         }
         afterTest();
     }
