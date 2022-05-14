void TabContentsContainerGtk::DetachTab(TabContents* tab) {
void TabContentsContainerGtk::DetachTab(WebContents* tab) {
  gfx::NativeView widget = tab->GetNativeView();
 
  if (widget) {
    GtkWidget* parent = gtk_widget_get_parent(widget);
    if (parent) {
      DCHECK_EQ(parent, expanded_);
      gtk_container_remove(GTK_CONTAINER(expanded_), widget);
    }
  }
}
