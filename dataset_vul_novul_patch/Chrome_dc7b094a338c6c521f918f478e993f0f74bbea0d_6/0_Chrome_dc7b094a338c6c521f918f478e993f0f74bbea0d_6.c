  static void ConnectionChangeHandler(void* object, bool connected) {
  // IBusController override.
  virtual void OnConnectionChange(bool connected) {
     if (!BrowserThread::CurrentlyOn(BrowserThread::UI)) {
       LOG(ERROR) << "Not on UI thread";
       return;
     }
 
    ime_connected_ = connected;
     if (connected) {
      pending_config_requests_.clear();
      pending_config_requests_.insert(
          current_config_values_.begin(),
          current_config_values_.end());
      FlushImeConfig();

      ChangeInputMethod(previous_input_method().id);
      ChangeInputMethod(current_input_method().id);
     }
   }
