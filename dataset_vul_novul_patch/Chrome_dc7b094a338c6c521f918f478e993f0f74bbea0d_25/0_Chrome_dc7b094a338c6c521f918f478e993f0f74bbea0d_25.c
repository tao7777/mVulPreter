  void RegisterProperties(const ImePropertyList& prop_list) {
  void RegisterProperties(const input_method::ImePropertyList& prop_list) {
     current_ime_properties_ = prop_list;
 
    FOR_EACH_OBSERVER(InputMethodLibrary::Observer, observers_,
                       PropertyListChanged(this,
                                           current_ime_properties_));
   }
