static void testInspectorDefault(InspectorTest* test, gconstpointer)
{
    test->showInWindowAndWaitUntilMapped(GTK_WINDOW_TOPLEVEL);
    test->resizeView(200, 200);
    test->loadHtml("<html><body><p>WebKitGTK+ Inspector test</p></body></html>", 0);
    test->waitUntilLoadFinished();

    test->showAndWaitUntilFinished(false);
    GRefPtr<WebKitWebViewBase> inspectorView = webkit_web_inspector_get_web_view(test->m_inspector);
    g_assert(inspectorView.get());
    test->assertObjectIsDeletedWhenTestFinishes(G_OBJECT(inspectorView.get()));
    g_assert(!webkit_web_inspector_is_attached(test->m_inspector));
    g_assert_cmpuint(webkit_web_inspector_get_attached_height(test->m_inspector), ==, 0);
    Vector<InspectorTest::InspectorEvents>& events = test->m_events;
    g_assert_cmpint(events.size(), ==, 2);
    g_assert_cmpint(events[0], ==, InspectorTest::BringToFront);
    g_assert_cmpint(events[1], ==, InspectorTest::OpenWindow);
    test->m_events.clear();

    test->showAndWaitUntilFinished(true);
    events = test->m_events;
    g_assert_cmpint(events.size(), ==, 1);
    g_assert_cmpint(events[0], ==, InspectorTest::BringToFront);
    test->m_events.clear();
 
     test->resizeViewAndAttach();
     g_assert(webkit_web_inspector_is_attached(test->m_inspector));
    g_assert_cmpuint(webkit_web_inspector_get_attached_height(test->m_inspector), >=, InspectorTest::gMinimumAttachedInspectorHeight);
     events = test->m_events;
     g_assert_cmpint(events.size(), ==, 1);
     g_assert_cmpint(events[0], ==, InspectorTest::Attach);
    test->m_events.clear();

    test->detachAndWaitUntilWindowOpened();
    g_assert(!webkit_web_inspector_is_attached(test->m_inspector));
    events = test->m_events;
    g_assert_cmpint(events.size(), ==, 2);
    g_assert_cmpint(events[0], ==, InspectorTest::Detach);
    g_assert_cmpint(events[1], ==, InspectorTest::OpenWindow);
    test->m_events.clear();

    test->closeAndWaitUntilClosed();
    events = test->m_events;
    g_assert_cmpint(events.size(), ==, 1);
    g_assert_cmpint(events[0], ==, InspectorTest::Closed);
    test->m_events.clear();
}
