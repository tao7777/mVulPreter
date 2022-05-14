static bool GetSingleHeader(const HttpResponseHeaders& headers,
/* static */
bool WebSocketHandshake::GetSingleHeader(const HttpResponseHeaders& headers,
                                         const std::string& name,
                                         std::string* value) {
   std::string first_value;
   void* iter = NULL;
   if (!headers.EnumerateHeader(&iter, name, &first_value))
    return false;

  std::string second_value;
  if (headers.EnumerateHeader(&iter, name, &second_value))
    return false;
  *value = first_value;
  return true;
 }
