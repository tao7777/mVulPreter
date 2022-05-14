  static void FocusInCallback(IBusPanelService* panel,
  void FocusIn(IBusPanelService* panel, const gchar* input_context_path) {
    if (!input_context_path) {
      LOG(ERROR) << "NULL context passed";
    } else {
      VLOG(1) << "FocusIn: " << input_context_path;
    }
    // Remember the current ic path.
    input_context_path_ = Or(input_context_path, "");
   }
