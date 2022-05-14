 void DisconnectWindowLinux::Show(remoting::ChromotingHost* host,
                                  const std::string& username) {
  host_ = host;
  CreateWindow();
  gtk_label_set_text(GTK_LABEL(user_label_), username.c_str());
  gtk_window_present(GTK_WINDOW(disconnect_window_));
 }
