 std::string Backtrace::GetFunctionName(uintptr_t pc, uintptr_t* offset) {
   std::string func_name = GetFunctionNameRaw(pc, offset);
  if (!func_name.empty()) {
#if defined(__APPLE__)
    if (func_name[0] != '_') {
      return func_name;
    }
#endif
    char* name = __cxa_demangle(func_name.c_str(), 0, 0, 0);
    if (name) {
      func_name = name;
      free(name);
    }
  }
   return func_name;
 }
