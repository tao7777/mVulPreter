   virtual void ChangeInputMethod(const std::string& input_method_id) {
    tentative_current_input_method_id_ = input_method_id;
     if (ibus_daemon_process_handle_ == base::kNullProcessHandle &&
        chromeos::input_method::IsKeyboardLayout(input_method_id)) {
       ChangeCurrentInputMethodFromId(input_method_id);
    } else {
      StartInputMethodDaemon();
      if (!ChangeInputMethodViaIBus(input_method_id)) {
        VLOG(1) << "Failed to change the input method to " << input_method_id
                << " (deferring)";
      }
    }
  }
