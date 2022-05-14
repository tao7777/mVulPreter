  static void RegisterPropertiesHandler(
  // IBusController override.
  virtual void OnRegisterImeProperties(
      const input_method::ImePropertyList& prop_list) {
     if (!BrowserThread::CurrentlyOn(BrowserThread::UI)) {
       LOG(ERROR) << "Not on UI thread";
       return;
     }
 
    RegisterProperties(prop_list);
   }
