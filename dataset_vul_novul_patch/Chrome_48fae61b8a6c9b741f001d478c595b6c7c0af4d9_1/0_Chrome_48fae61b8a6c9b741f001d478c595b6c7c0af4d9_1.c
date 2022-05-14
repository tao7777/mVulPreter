const wchar_t* GetIntegrityLevelString(IntegrityLevel integrity_level) {
  switch (integrity_level) {
    case INTEGRITY_LEVEL_SYSTEM:
      return L"S-1-16-16384";
    case INTEGRITY_LEVEL_HIGH:
      return L"S-1-16-12288";
    case INTEGRITY_LEVEL_MEDIUM:
      return L"S-1-16-8192";
    case INTEGRITY_LEVEL_MEDIUM_LOW:
      return L"S-1-16-6144";
    case INTEGRITY_LEVEL_LOW:
       return L"S-1-16-4096";
     case INTEGRITY_LEVEL_BELOW_LOW:
       return L"S-1-16-2048";
    case INTEGRITY_LEVEL_UNTRUSTED:
      return L"S-1-16-0";
     case INTEGRITY_LEVEL_LAST:
       return NULL;
   }

  NOTREACHED();
  return NULL;
}
