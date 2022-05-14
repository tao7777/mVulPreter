  void RegisterProperties(IBusPropList* ibus_prop_list) {
  void DoRegisterProperties(IBusPropList* ibus_prop_list) {
    VLOG(1) << "RegisterProperties" << (ibus_prop_list ? "" : " (clear)");
 
     ImePropertyList prop_list;  // our representation.
     if (ibus_prop_list) {
       if (!FlattenPropertyList(ibus_prop_list, &prop_list)) {
        DoRegisterProperties(NULL);
         return;
       }
     }
    VLOG(1) << "RegisterProperties" << (ibus_prop_list ? "" : " (clear)");
