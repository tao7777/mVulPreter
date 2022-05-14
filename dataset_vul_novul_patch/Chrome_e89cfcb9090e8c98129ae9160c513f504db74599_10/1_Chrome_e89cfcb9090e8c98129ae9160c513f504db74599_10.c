void TabContentsContainerGtk::DetachTab(TabContents* tab) {
  gfx::NativeView widget = tab->web_contents()->GetNativeView();
 
  if (widget) {
    GtkWidget* parent = gtk_widget_get_parent(widget);
    if (parent) {
      DCHECK_EQ(parent, expanded_);
      gtk_container_remove(GTK_CONTAINER(expanded_), widget);
    }
  }
}
