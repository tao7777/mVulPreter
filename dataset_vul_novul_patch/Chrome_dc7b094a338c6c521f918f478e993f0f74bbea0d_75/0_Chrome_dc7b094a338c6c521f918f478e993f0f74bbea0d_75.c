  static void UpdatePropertyCallback(IBusPanelService* panel,
  void UpdateProperty(IBusPanelService* panel, IBusProperty* ibus_prop) {
    VLOG(1) << "UpdateProperty";
    DCHECK(ibus_prop);

    // You can call
    //   LOG(INFO) << "\n" << PrintProp(ibus_prop, 0);
    // here to dump |ibus_prop|.

    ImePropertyList prop_list;  // our representation.
    if (!FlattenProperty(ibus_prop, &prop_list)) {
      // Don't update the UI on errors.
      LOG(ERROR) << "Malformed properties are detected";
      return;
    }
    // Notify the change.
    if (!prop_list.empty()) {
      FOR_EACH_OBSERVER(Observer, observers_,
                        OnUpdateImeProperty(prop_list));
    }
   }
