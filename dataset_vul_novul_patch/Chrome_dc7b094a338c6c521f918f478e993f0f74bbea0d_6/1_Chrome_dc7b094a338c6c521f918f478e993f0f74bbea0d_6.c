  static void ConnectionChangeHandler(void* object, bool connected) {
     if (!BrowserThread::CurrentlyOn(BrowserThread::UI)) {
       LOG(ERROR) << "Not on UI thread";
       return;
     }
 
    InputMethodLibraryImpl* input_method_library =
        static_cast<InputMethodLibraryImpl*>(object);
    input_method_library->ime_connected_ = connected;
     if (connected) {
      input_method_library->pending_config_requests_.clear();
      input_method_library->pending_config_requests_.insert(
          input_method_library->current_config_values_.begin(),
          input_method_library->current_config_values_.end());
      input_method_library->FlushImeConfig();
      input_method_library->ChangeInputMethod(
          input_method_library->previous_input_method().id);
      input_method_library->ChangeInputMethod(
          input_method_library->current_input_method().id);
     }
   }
