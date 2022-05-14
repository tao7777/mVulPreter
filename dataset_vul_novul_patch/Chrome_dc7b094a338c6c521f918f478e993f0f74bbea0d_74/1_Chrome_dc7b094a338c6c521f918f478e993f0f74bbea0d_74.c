  void UpdateProperty(IBusProperty* ibus_prop) {
    DLOG(INFO) << "UpdateProperty";
    DCHECK(ibus_prop);
    ImePropertyList prop_list;  // our representation.
    if (!FlattenProperty(ibus_prop, &prop_list)) {
      LOG(ERROR) << "Malformed properties are detected";
      return;
    }
    if (!prop_list.empty()) {
      update_ime_property_(language_library_, prop_list);
    }
   }
