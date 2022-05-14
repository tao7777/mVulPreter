  bool ChangeInputMethod(const char* name) {
  // IBusController override.
  virtual bool ChangeInputMethod(const std::string& name) {
     if (!IBusConnectionsAreAlive()) {
       LOG(ERROR) << "ChangeInputMethod: IBus connection is not alive";
       return false;
     }
    if (name.empty()) {
       return false;
     }
     if (!InputMethodIdIsWhitelisted(name)) {
      LOG(ERROR) << "Input method '" << name << "' is not supported";
      return false;
    }

    DoRegisterProperties(NULL);
 
     ibus_bus_set_global_engine_async(ibus_,
                                     name.c_str(),
                                      -1,  // use the default ibus timeout
                                      NULL,  // cancellable
                                      NULL,  // callback
                                      NULL);  // user_data
     return true;
   }
