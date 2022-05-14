  void SetImePropertyActivated(const char* key, bool activated) {
     if (!IBusConnectionsAreAlive()) {
       LOG(ERROR) << "SetImePropertyActivated: IBus connection is not alive";
       return;
     }
    if (!key || (key[0] == '\0')) {
       return;
     }
     if (input_context_path_.empty()) {
      LOG(ERROR) << "Input context is unknown";
      return;
    }

    IBusInputContext* context = GetInputContext(input_context_path_, ibus_);
    if (!context) {
      return;
     }
     ibus_input_context_property_activate(
        context, key, (activated ? PROP_STATE_CHECKED : PROP_STATE_UNCHECKED));
 
     g_object_unref(context);
   }
