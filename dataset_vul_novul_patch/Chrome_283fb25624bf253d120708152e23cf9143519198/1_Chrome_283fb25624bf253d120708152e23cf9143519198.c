bool DataRemovalRequested(base::DictionaryValue* dict, std::string key) {
   bool value = false;
   if (!dict->GetBoolean(key, &value))
     return false;
  else
    return value;
}
