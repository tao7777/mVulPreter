bool ParseJSONDictionary(const std::string& json, DictionaryValue** dict,
                         std::string* error) {
  int error_code = 0;
  Value* params =
    base::JSONReader::ReadAndReturnError(json, true, &error_code, error);
  if (error_code != 0) {
    VLOG(1) << "Could not parse JSON object, " << *error;
    if (params)
      delete params;
    return false;
  }
  if (!params || params->GetType() != Value::TYPE_DICTIONARY) {
    *error = "Data passed in URL must be of type dictionary.";
    VLOG(1) << "Invalid type to parse";
    if (params)
      delete params;
    return false;
  }
  *dict = static_cast<DictionaryValue*>(params);
  return true;
}
