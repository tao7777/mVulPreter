 void DisconnectWindowLinux::Hide() {
  DCHECK(disconnect_window_);

  gtk_widget_hide(disconnect_window_);
}

gboolean DisconnectWindowLinux::OnWindowDelete(GtkWidget* widget,
                                               GdkEvent* event) {
  // Don't allow the window to be closed.
  return TRUE;
}

void DisconnectWindowLinux::OnDisconnectClicked(GtkButton* sender) {
  DCHECK(host_);
  host_->Shutdown();
 }
