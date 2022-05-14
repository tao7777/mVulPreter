bool SendAutomationJSONRequest(AutomationMessageSender* sender,
                               const DictionaryValue& request_dict,
                               DictionaryValue* reply_dict,
                                std::string* error_msg) {
   std::string request, reply;
   base::JSONWriter::Write(&request_dict, false, &request);
  std::string command;
  request_dict.GetString("command", &command);
  LOG(INFO) << "Sending '" << command << "' command.";

   base::Time before_sending = base::Time::Now();
  bool success = false;
  if (!SendAutomationJSONRequestWithDefaultTimeout(
          sender, request, &reply, &success)) {
    *error_msg = base::StringPrintf(
        "Chrome did not respond to '%s'. Elapsed time was %" PRId64 " ms. "
            "Request details: (%s).",
        command.c_str(),
        (base::Time::Now() - before_sending).InMilliseconds(),
        request.c_str());
     return false;
   }
   scoped_ptr<Value> value(base::JSONReader::Read(reply, true));
   if (!value.get() || !value->IsType(Value::TYPE_DICTIONARY)) {
     LOG(ERROR) << "JSON request did not return dict: " << command << "\n";
     return false;
   }
   DictionaryValue* dict = static_cast<DictionaryValue*>(value.get());
   if (!success) {
    std::string error;
     dict->GetString("error", &error);
     *error_msg = base::StringPrintf(
         "Internal Chrome error during '%s': (%s). Request details: (%s).",
        command.c_str(),
        error.c_str(),
        request.c_str());
    LOG(ERROR) << "JSON request failed: " << command << "\n"
               << "    with error: " << error;
    return false;
  }
  reply_dict->MergeDictionary(dict);
  return true;
}
