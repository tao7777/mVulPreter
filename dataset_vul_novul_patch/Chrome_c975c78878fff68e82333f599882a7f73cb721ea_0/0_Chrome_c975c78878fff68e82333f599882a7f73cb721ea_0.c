 void Shell::PlatformSetContents() {
   WebContentsView* content_view = web_contents_->GetView();
   gtk_container_add(GTK_CONTAINER(vbox_), content_view->GetNativeView());
 }
