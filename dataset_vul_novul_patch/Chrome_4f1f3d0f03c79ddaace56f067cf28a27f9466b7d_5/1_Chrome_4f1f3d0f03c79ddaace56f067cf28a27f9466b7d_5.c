bool TestRunner::AddRuleSys32(TargetPolicy::Semantics semantics,
                              const wchar_t* pattern) {
  if (!is_init_)
    return false;

  std::wstring win32_path = MakePathToSys32(pattern, false);
   if (win32_path.empty())
     return false;
 
   return AddRule(TargetPolicy::SUBSYS_FILES, semantics, win32_path.c_str());
 }
