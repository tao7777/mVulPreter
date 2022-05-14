void LogErrorEventDescription(Display* dpy,
                              const XErrorEvent& error_event) {
  char error_str[256];
  char request_str[256];

  XGetErrorText(dpy, error_event.error_code, error_str, sizeof(error_str));

  strncpy(request_str, "Unknown", sizeof(request_str));
  if (error_event.request_code < 128) {
    std::string num = base::UintToString(error_event.request_code);
    XGetErrorDatabaseText(
        dpy, "XRequest", num.c_str(), "Unknown", request_str,
        sizeof(request_str));
  } else {
    int num_ext;
    char** ext_list = XListExtensions(dpy, &num_ext);

    for (int i = 0; i < num_ext; i++) {
      int ext_code, first_event, first_error;
      XQueryExtension(dpy, ext_list[i], &ext_code, &first_event, &first_error);
      if (error_event.request_code == ext_code) {
        std::string msg = StringPrintf(
            "%s.%d", ext_list[i], error_event.minor_code);
        XGetErrorDatabaseText(
            dpy, "XRequest", msg.c_str(), "Unknown", request_str,
            sizeof(request_str));
        break;
      }
    }
     XFreeExtensionList(ext_list);
   }
 
  LOG(ERROR) 
       << "X Error detected: "
       << "serial " << error_event.serial << ", "
       << "error_code " << static_cast<int>(error_event.error_code)
      << " (" << error_str << "), "
      << "request_code " << static_cast<int>(error_event.request_code) << ", "
      << "minor_code " << static_cast<int>(error_event.minor_code)
      << " (" << request_str << ")";
}
