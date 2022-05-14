  void FocusIn(const char* input_context_path) {
    if (!input_context_path) {
      LOG(ERROR) << "NULL context passed";
    } else {
      DLOG(INFO) << "FocusIn: " << input_context_path;
    }
    input_context_path_ = Or(input_context_path, "");
  }
