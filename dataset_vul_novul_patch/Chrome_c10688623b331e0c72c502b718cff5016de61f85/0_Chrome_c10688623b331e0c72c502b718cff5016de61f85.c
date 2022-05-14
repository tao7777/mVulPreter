   bool SetVarImpl(const char* variable_name, const std::string& new_value) {
 #if defined(OS_POSIX)
    return !setenv(variable_name, new_value.c_str(), 1);
 #elif defined(OS_WIN)
    // On success, a nonzero value is returned.
    return !!SetEnvironmentVariable(UTF8ToWide(variable_name).c_str(),
                                    UTF8ToWide(new_value).c_str());
 #endif
   }
