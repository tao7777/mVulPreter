  static void UpdatePropertyHandler(
  // IBusController override.
  virtual void OnUpdateImeProperty(
      const input_method::ImePropertyList& prop_list) {
     if (!BrowserThread::CurrentlyOn(BrowserThread::UI)) {
       LOG(ERROR) << "Not on UI thread";
       return;
     }
 
    UpdateProperty(prop_list);
   }
