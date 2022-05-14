void ExtensionTtsController::ClearUtteranceQueue() {
  std::set<std::string> required_event_types;
  if (options->HasKey(constants::kRequiredEventTypesKey)) {
    ListValue* list;
    EXTENSION_FUNCTION_VALIDATE(
        options->GetList(constants::kRequiredEventTypesKey, &list));
    for (size_t i = 0; i < list->GetSize(); i++) {
      std::string event_type;
      if (!list->GetString(i, &event_type))
        required_event_types.insert(event_type);
    }
   }
