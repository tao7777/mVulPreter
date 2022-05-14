   bool UnSetVarImpl(const char* variable_name) {
 #if defined(OS_POSIX)
    return !unsetenv(variable_name);
 #elif defined(OS_WIN)
    // On success, a nonzero value is returned.
    return !!SetEnvironmentVariable(UTF8ToWide(variable_name).c_str(), NULL);
 #endif
   }
