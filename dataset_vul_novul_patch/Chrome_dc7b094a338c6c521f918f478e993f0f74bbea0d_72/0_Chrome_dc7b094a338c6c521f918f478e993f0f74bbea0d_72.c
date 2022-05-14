  void SetImePropertyActivated(const char* key, bool activated) {
  // IBusController override.
  virtual void SetImePropertyActivated(const std::string& key,
                                       bool activated) {
     if (!IBusConnectionsAreAlive()) {
       LOG(ERROR) << "SetImePropertyActivated: IBus connection is not alive";
       return;
     }
    if (key.empty()) {
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
        context, key.c_str(),
        (activated ? PROP_STATE_CHECKED : PROP_STATE_UNCHECKED));
 
     g_object_unref(context);
   }
