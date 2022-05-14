 std::string Backtrace::GetFunctionName(uintptr_t pc, uintptr_t* offset) {
   std::string func_name = GetFunctionNameRaw(pc, offset);
   return func_name;
 }
