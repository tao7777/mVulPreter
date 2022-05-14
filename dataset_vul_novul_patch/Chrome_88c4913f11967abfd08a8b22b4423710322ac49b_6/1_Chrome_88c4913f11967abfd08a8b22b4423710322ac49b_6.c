void CCLayerTreeHostTest::endTest()
 {
     if (!isMainThread())
        CCMainThread::postTask(createMainThreadTask(this, &CCLayerTreeHostTest::endTest));
     else {
        if (m_beginning)
            m_endWhenBeginReturns = true;
        else
            onEndTest(static_cast<void*>(this));
    }
}
