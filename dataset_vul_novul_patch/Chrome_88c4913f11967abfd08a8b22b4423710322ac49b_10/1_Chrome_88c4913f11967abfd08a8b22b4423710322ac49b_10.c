    static void testTimeout(void* self)
    {
        CCLayerTreeHostTest* test = static_cast<CCLayerTreeHostTest*>(self);
        if (!test->m_running)
            return;
        test->m_timedOut = true;
        test->endTest();
    }
