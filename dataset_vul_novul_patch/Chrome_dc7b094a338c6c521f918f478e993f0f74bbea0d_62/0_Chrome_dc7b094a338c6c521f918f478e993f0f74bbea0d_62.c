  static void IBusBusGlobalEngineChangedCallback(
  void IBusBusGlobalEngineChanged(IBusBus* bus, const gchar* engine_name) {
     DCHECK(engine_name);
    VLOG(1) << "Global engine is changed to " << engine_name;
    UpdateUI(engine_name);
   }
