  void UpdateProperty(const ImePropertyList& prop_list) {
  void UpdateProperty(const input_method::ImePropertyList& prop_list) {
     for (size_t i = 0; i < prop_list.size(); ++i) {
       FindAndUpdateProperty(prop_list[i], &current_ime_properties_);
     }
 
    FOR_EACH_OBSERVER(InputMethodLibrary::Observer, observers_,
                       PropertyListChanged(this,
                                           current_ime_properties_));
   }
