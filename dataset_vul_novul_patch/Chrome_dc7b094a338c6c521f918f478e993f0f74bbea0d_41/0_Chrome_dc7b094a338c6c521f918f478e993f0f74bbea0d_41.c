void AddInputMethodNames(const GList* engines, InputMethodDescriptors* out) {
  DCHECK(out);
  for (; engines; engines = g_list_next(engines)) {
    IBusEngineDesc* engine_desc = IBUS_ENGINE_DESC(engines->data);
    const gchar* name = ibus_engine_desc_get_name(engine_desc);
    const gchar* longname = ibus_engine_desc_get_longname(engine_desc);
    const gchar* layout = ibus_engine_desc_get_layout(engine_desc);
    const gchar* language = ibus_engine_desc_get_language(engine_desc);
    if (InputMethodIdIsWhitelisted(name)) {
      out->push_back(CreateInputMethodDescriptor(name,
                                                  longname,
                                                  layout,
                                                  language));
      VLOG(1) << name << " (preloaded)";
     }
   }
 }
