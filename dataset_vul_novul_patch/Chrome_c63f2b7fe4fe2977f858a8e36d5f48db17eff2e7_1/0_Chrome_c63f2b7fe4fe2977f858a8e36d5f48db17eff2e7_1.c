void ExtensionTtsController::CheckSpeechStatus() {
 
  std::set<std::string> desired_event_types;
  if (options->HasKey(constants::kDesiredEventTypesKey)) {
    ListValue* list;
    EXTENSION_FUNCTION_VALIDATE(
        options->GetList(constants::kDesiredEventTypesKey, &list));
    for (size_t i = 0; i < list->GetSize(); i++) {
      std::string event_type;
      if (!list->GetString(i, &event_type))
        desired_event_types.insert(event_type);
    }
   }
 
  std::string voice_extension_id;
  if (options->HasKey(constants::kExtensionIdKey)) {
    EXTENSION_FUNCTION_VALIDATE(
        options->GetString(constants::kExtensionIdKey, &voice_extension_id));
   }
