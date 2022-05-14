pp::Var Error(nacl::string call_name, const char* caller,
pp::Var Error(const nacl::string& call_name, const char* caller,
               const char* error, pp::Var* exception) {
   nacl::stringstream error_stream;
   error_stream << call_name << ": " << error;
  if (!exception->is_undefined()) {
    error_stream << " - " + exception->AsString();
  }
  std::string str = error_stream.str();
  const char* e = str.c_str();
  PLUGIN_PRINTF(("ScriptableHandle::%s (%s)\n", caller, e));
  *exception = pp::Var(e);
  return pp::Var();
}
