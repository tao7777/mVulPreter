bool SendAutomationJSONRequest(AutomationMessageSender* sender,
                               const DictionaryValue& request_dict,
                               DictionaryValue* reply_dict,
                                std::string* error_msg) {
   std::string request, reply;
   base::JSONWriter::Write(&request_dict, false, &request);
  bool success = false;
  int timeout_ms = TestTimeouts::action_max_timeout_ms();
   base::Time before_sending = base::Time::Now();
  if (!SendAutomationJSONRequest(
          sender, request, timeout_ms, &reply, &success)) {
    int64 elapsed_ms = (base::Time::Now() - before_sending).InMilliseconds();
    std::string command;
    request_dict.GetString("command", &command);
    if (elapsed_ms >= timeout_ms) {
      *error_msg = base::StringPrintf(
          "Chrome did not respond to '%s'. Request may have timed out. "
              "Elapsed time was %" PRId64 " ms. Request timeout was %d ms. "
              "Request details: (%s).",
          command.c_str(),
          elapsed_ms,
          timeout_ms,
          request.c_str());
    } else {
      *error_msg = base::StringPrintf(
          "Chrome did not respond to '%s'. Elapsed time was %" PRId64 " ms. "
              "Request details: (%s).",
          command.c_str(),
          elapsed_ms,
          request.c_str());
    }
     return false;
   }
   scoped_ptr<Value> value(base::JSONReader::Read(reply, true));
   if (!value.get() || !value->IsType(Value::TYPE_DICTIONARY)) {
    std::string command;
    request_dict.GetString("command", &command);
     LOG(ERROR) << "JSON request did not return dict: " << command << "\n";
     return false;
   }
   DictionaryValue* dict = static_cast<DictionaryValue*>(value.get());
   if (!success) {
    std::string command, error;
    request_dict.GetString("command", &command);
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
