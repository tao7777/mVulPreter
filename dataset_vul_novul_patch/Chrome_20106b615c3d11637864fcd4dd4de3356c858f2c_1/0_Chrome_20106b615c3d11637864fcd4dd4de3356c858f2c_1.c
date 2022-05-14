void StatusBubbleGtk::Show() {
   hide_timer_.Stop();
 
  gtk_widget_show(container_.get());
   if (container_->window)
     gdk_window_raise(container_->window);
 }
