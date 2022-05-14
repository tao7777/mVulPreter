   bool SetVarImpl(const char* variable_name, const std::string& new_value) {
 #if defined(OS_POSIX)
    return setenv(variable_name, new_value.c_str(), 1) == 0;
 #elif defined(OS_WIN)
    return ::SetEnvironmentVariable(ASCIIToWide(variable_name).c_str(),
                                    ASCIIToWide(new_value).c_str()) != 0;
 #endif
   }
