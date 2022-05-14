  bool ChangeInputMethod(const char* name) {
     if (!IBusConnectionsAreAlive()) {
       LOG(ERROR) << "ChangeInputMethod: IBus connection is not alive";
       return false;
     }
    if (!name) {
       return false;
     }
     if (!InputMethodIdIsWhitelisted(name)) {
      LOG(ERROR) << "Input method '" << name << "' is not supported";
      return false;
    }

    RegisterProperties(NULL);
 
     ibus_bus_set_global_engine_async(ibus_,
                                     name,
                                      -1,  // use the default ibus timeout
                                      NULL,  // cancellable
                                      NULL,  // callback
                                      NULL);  // user_data
     return true;
   }
