bool FindAndUpdateProperty(const chromeos::ImeProperty& new_prop,
                           chromeos::ImePropertyList* prop_list) {
   for (size_t i = 0; i < prop_list->size(); ++i) {
    chromeos::ImeProperty& prop = prop_list->at(i);
     if (prop.key == new_prop.key) {
       const int saved_id = prop.selection_item_id;
      prop = new_prop;
      prop.selection_item_id = saved_id;
      return true;
    }
  }
  return false;
}
