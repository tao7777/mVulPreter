  static void InputMethodChangedHandler(
  // IBusController override.
  virtual void OnCurrentInputMethodChanged(
      const input_method::InputMethodDescriptor& current_input_method) {
    if (!BrowserThread::CurrentlyOn(BrowserThread::UI)) {
      LOG(ERROR) << "Not on UI thread";
       return;
     }
 
    ChangeCurrentInputMethod(current_input_method);
   }
