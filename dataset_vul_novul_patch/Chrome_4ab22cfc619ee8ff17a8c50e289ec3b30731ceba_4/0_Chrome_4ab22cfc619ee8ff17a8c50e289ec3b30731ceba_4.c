bool ParseRequestInfo(const struct mg_request_info* const request_info,
                      std::string* method,
                      std::vector<std::string>* path_segments,
                      DictionaryValue** parameters,
                      Response* const response) {
  *method = request_info->request_method;
  if (*method == "HEAD")
    *method = "GET";
  else if (*method == "PUT")
    *method = "POST";

  std::string uri(request_info->uri);
  SessionManager* manager = SessionManager::GetInstance();
  uri = uri.substr(manager->url_base().length());

   base::SplitString(uri, '/', path_segments);
 
   if (*method == "POST" && request_info->post_data_len > 0) {
     std::string json(request_info->post_data, request_info->post_data_len);
    std::string error_msg;
    scoped_ptr<Value> params(base::JSONReader::ReadAndReturnError(
        json, true, NULL, &error_msg));
    if (!params.get()) {
       response->SetError(new Error(
           kBadRequest,
          "Failed to parse command data: " + error_msg + "\n  Data: " + json));
       return false;
     }
    if (!params->IsType(Value::TYPE_DICTIONARY)) {
      response->SetError(new Error(
          kBadRequest,
          "Data passed in URL must be a dictionary. Data: " + json));
      return false;
    }
    *parameters = static_cast<DictionaryValue*>(params.release());
   }
   return true;
 }
