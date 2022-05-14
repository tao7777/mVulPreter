  static void InputMethodChangedHandler(
      void* object,
      const chromeos::InputMethodDescriptor& current_input_method) {
    if (!BrowserThread::CurrentlyOn(BrowserThread::UI)) {
      LOG(ERROR) << "Not on UI thread";
       return;
     }
 
    InputMethodLibraryImpl* input_method_library =
        static_cast<InputMethodLibraryImpl*>(object);
    input_method_library->ChangeCurrentInputMethod(current_input_method);
   }
