    bool attach()
    {
        GRefPtr<WebKitWebViewBase> inspectorView = webkit_web_inspector_get_web_view(m_inspector);
        if (m_inspectorWindow) {
            gtk_container_remove(GTK_CONTAINER(m_inspectorWindow), GTK_WIDGET(inspectorView.get()));
            gtk_widget_destroy(m_inspectorWindow);
            m_inspectorWindow = 0;
        }

        GtkWidget* pane;
        if (gtk_bin_get_child(GTK_BIN(m_parentWindow)) == GTK_WIDGET(m_webView)) {
            GRefPtr<WebKitWebView> inspectedView = m_webView;
            gtk_container_remove(GTK_CONTAINER(m_parentWindow), GTK_WIDGET(m_webView));
            pane = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
            gtk_paned_add1(GTK_PANED(pane), GTK_WIDGET(m_webView));
            gtk_container_add(GTK_CONTAINER(m_parentWindow), pane);
             gtk_widget_show_all(pane);
         } else
             pane = gtk_bin_get_child(GTK_BIN(m_parentWindow));
        gtk_paned_set_position(GTK_PANED(pane), webkit_web_inspector_get_attached_height(m_inspector));
         gtk_paned_add2(GTK_PANED(pane), GTK_WIDGET(inspectorView.get()));
 
         return InspectorTest::attach();
    }
