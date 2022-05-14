  void RegisterProperties(const ImePropertyList& prop_list) {
     current_ime_properties_ = prop_list;
 
    FOR_EACH_OBSERVER(Observer, observers_,
                       PropertyListChanged(this,
                                           current_ime_properties_));
   }
