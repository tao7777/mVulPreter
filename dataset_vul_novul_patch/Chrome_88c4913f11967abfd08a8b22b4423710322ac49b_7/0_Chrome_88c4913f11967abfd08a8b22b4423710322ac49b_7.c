    static void onEndTest(void* self)
     {
         ASSERT(isMainThread());
         webkit_support::QuitMessageLoop();
        webkit_support::RunAllPendingMessages();
         CCLayerTreeHostTest* test = static_cast<CCLayerTreeHostTest*>(self);
         ASSERT(test);
         test->m_layerTreeHost.clear();
    }
