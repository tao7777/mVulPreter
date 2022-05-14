   bool Init() {
     DCHECK(!initialized_successfully_) << "Already initialized";
 
    if (!CrosLibrary::Get()->EnsureLoaded())
      return false;
    input_method_status_connection_ = chromeos::MonitorInputMethodStatus(
        this,
        &InputMethodChangedHandler,
        &RegisterPropertiesHandler,
        &UpdatePropertyHandler,
        &ConnectionChangeHandler);
    if (!input_method_status_connection_)
      return false;
 
     initialized_successfully_ = true;
     return true;
   }
