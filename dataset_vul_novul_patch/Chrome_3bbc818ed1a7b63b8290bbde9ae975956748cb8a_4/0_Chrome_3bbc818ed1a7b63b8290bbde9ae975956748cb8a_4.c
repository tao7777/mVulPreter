void WebInspectorProxy::platformAttach()
{
    GRefPtr<GtkWidget> inspectorView = m_inspectorView;
    if (m_inspectorWindow) {
        gtk_container_remove(GTK_CONTAINER(m_inspectorWindow), m_inspectorView);
        gtk_widget_destroy(m_inspectorWindow);
         m_inspectorWindow = 0;
     }
 
    // Set a default attached height based on InspectorFrontendClientLocal.
    static const unsigned defaultAttachedHeight = 300;
    unsigned maximumAttachedHeight = platformInspectedWindowHeight() * 3 / 4;
    platformSetAttachedWindowHeight(std::max(minimumAttachedHeight, std::min(defaultAttachedHeight, maximumAttachedHeight)));

     if (m_client.attach(this))
         return;
 
    gtk_container_add(GTK_CONTAINER(m_page->viewWidget()), m_inspectorView);
    gtk_widget_show(m_inspectorView);
}
