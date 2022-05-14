   bool UnSetVarImpl(const char* variable_name) {
 #if defined(OS_POSIX)
    return unsetenv(variable_name) == 0;
 #elif defined(OS_WIN)
    return ::SetEnvironmentVariable(ASCIIToWide(variable_name).c_str(),
                                    NULL) != 0;
 #endif
   }
