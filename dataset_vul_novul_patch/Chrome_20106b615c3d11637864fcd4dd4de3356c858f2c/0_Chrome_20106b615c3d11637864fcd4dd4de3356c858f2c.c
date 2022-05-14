void StatusBubbleGtk::Hide() {
   expand_timer_.Stop();
   expand_animation_.reset();
 
  gtk_widget_hide(container_.get());
 }
