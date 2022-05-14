bool FileSystemPolicy::GenerateRules(const wchar_t* name,
                                     TargetPolicy::Semantics semantics,
                                     LowLevelPolicy* policy) {
  std::wstring mod_name(name);
  if (mod_name.empty()) {
     return false;
   }
 
   if (!PreProcessName(mod_name, &mod_name)) {
    NOTREACHED();
    return false;
  }

  if (0 != mod_name.compare(0, kNTPrefixLen, kNTPrefix)) {
    mod_name.insert(0, L"\\/?/?\\");
    name = mod_name.c_str();
  }

  EvalResult result = ASK_BROKER;

  const unsigned kCallNtCreateFile = 0x1;
  const unsigned kCallNtOpenFile = 0x2;
  const unsigned kCallNtQueryAttributesFile = 0x4;
  const unsigned kCallNtQueryFullAttributesFile = 0x8;
  const unsigned kCallNtSetInfoRename = 0x10;

  DWORD  rule_to_add = kCallNtOpenFile | kCallNtCreateFile |
                       kCallNtQueryAttributesFile |
                       kCallNtQueryFullAttributesFile | kCallNtSetInfoRename;

  PolicyRule create(result);
  PolicyRule open(result);
  PolicyRule query(result);
  PolicyRule query_full(result);
  PolicyRule rename(result);

  switch (semantics) {
    case TargetPolicy::FILES_ALLOW_DIR_ANY: {
      open.AddNumberMatch(IF, OpenFile::OPTIONS, FILE_DIRECTORY_FILE, AND);
      create.AddNumberMatch(IF, OpenFile::OPTIONS, FILE_DIRECTORY_FILE, AND);
      break;
    }
    case TargetPolicy::FILES_ALLOW_READONLY: {
      DWORD allowed_flags = FILE_READ_DATA | FILE_READ_ATTRIBUTES |
                            FILE_READ_EA | SYNCHRONIZE | FILE_EXECUTE |
                            GENERIC_READ | GENERIC_EXECUTE | READ_CONTROL;
      DWORD restricted_flags = ~allowed_flags;
      open.AddNumberMatch(IF_NOT, OpenFile::ACCESS, restricted_flags, AND);
      create.AddNumberMatch(IF_NOT, OpenFile::ACCESS, restricted_flags, AND);

      rule_to_add &= ~kCallNtSetInfoRename;
      break;
    }
    case TargetPolicy::FILES_ALLOW_QUERY: {
      rule_to_add &= ~(kCallNtOpenFile | kCallNtCreateFile |
                       kCallNtSetInfoRename);
      break;
    }
    case TargetPolicy::FILES_ALLOW_ANY: {
      break;
    }
    default: {
      NOTREACHED();
      return false;
    }
  }

  if ((rule_to_add & kCallNtCreateFile) &&
      (!create.AddStringMatch(IF, OpenFile::NAME, name, CASE_INSENSITIVE) ||
       !policy->AddRule(IPC_NTCREATEFILE_TAG, &create))) {
    return false;
  }

  if ((rule_to_add & kCallNtOpenFile) &&
      (!open.AddStringMatch(IF, OpenFile::NAME, name, CASE_INSENSITIVE) ||
       !policy->AddRule(IPC_NTOPENFILE_TAG, &open))) {
    return false;
  }

  if ((rule_to_add & kCallNtQueryAttributesFile) &&
      (!query.AddStringMatch(IF, FileName::NAME, name, CASE_INSENSITIVE) ||
       !policy->AddRule(IPC_NTQUERYATTRIBUTESFILE_TAG, &query))) {
    return false;
  }

  if ((rule_to_add & kCallNtQueryFullAttributesFile) &&
      (!query_full.AddStringMatch(IF, FileName::NAME, name, CASE_INSENSITIVE)
       || !policy->AddRule(IPC_NTQUERYFULLATTRIBUTESFILE_TAG,
                           &query_full))) {
    return false;
  }

  if ((rule_to_add & kCallNtSetInfoRename) &&
      (!rename.AddStringMatch(IF, FileName::NAME, name, CASE_INSENSITIVE) ||
       !policy->AddRule(IPC_NTSETINFO_RENAME_TAG, &rename))) {
    return false;
  }

  return true;
}
