 void webkitWebViewBaseSetInspectorViewHeight(WebKitWebViewBase* webkitWebViewBase, unsigned height)
 {
    if (!webkitWebViewBase->priv->inspectorView)
        return;
     if (webkitWebViewBase->priv->inspectorViewHeight == height)
         return;
     webkitWebViewBase->priv->inspectorViewHeight = height;
    gtk_widget_queue_resize_no_redraw(GTK_WIDGET(webkitWebViewBase));
 }
