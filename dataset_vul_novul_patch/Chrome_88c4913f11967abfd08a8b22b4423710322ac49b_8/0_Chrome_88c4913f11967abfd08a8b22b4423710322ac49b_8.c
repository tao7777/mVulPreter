    virtual void runTest()
    class TimeoutTask : public webkit_support::TaskAdaptor {
    public:
        explicit TimeoutTask(CCLayerTreeHostTest* test)
            : m_test(test)
        {
        }

        void clearTest()
        {
            m_test = 0;
        }

        virtual ~TimeoutTask()
        {
            if (m_test)
                m_test->clearTimeout();
        }

        virtual void Run()
        {
            if (m_test)
                m_test->timeout();
        }

    private:
        CCLayerTreeHostTest* m_test;
    };

    virtual void runTest(bool threaded)
     {
        m_settings.enableCompositorThread = threaded;
         webkit_support::PostDelayedTask(CCLayerTreeHostTest::onBeginTest, static_cast<void*>(this), 0);
        m_timeoutTask = new TimeoutTask(this);
        webkit_support::PostDelayedTask(m_timeoutTask, 5000); // webkit_support takes ownership of the task
         webkit_support::RunMessageLoop();
         webkit_support::RunAllPendingMessages();

        if (m_timeoutTask)
            m_timeoutTask->clearTest();

         ASSERT(!m_layerTreeHost.get());
         m_client.clear();
        if (m_timedOut) {
             FAIL() << "Test timed out";
             return;
         }
         afterTest();
     }
