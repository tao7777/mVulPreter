  void UpdateProperty(IBusProperty* ibus_prop) {
    FOR_EACH_OBSERVER(Observer, observers_,
                      OnRegisterImeProperties(prop_list));
   }
