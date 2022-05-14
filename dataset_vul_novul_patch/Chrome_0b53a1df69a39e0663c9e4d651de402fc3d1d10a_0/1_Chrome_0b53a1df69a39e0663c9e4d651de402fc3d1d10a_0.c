   static std::vector<syncer::ModelType> DefaultDatatypes() {
     std::vector<syncer::ModelType> datatypes;
     datatypes.push_back(syncer::APPS);
     datatypes.push_back(syncer::APP_SETTINGS);
    datatypes.push_back(syncer::AUTOFILL);
    datatypes.push_back(syncer::AUTOFILL_PROFILE);
    datatypes.push_back(syncer::BOOKMARKS);
#if defined(OS_LINUX) || defined(OS_WIN) || defined(OS_CHROMEOS)
    datatypes.push_back(syncer::DICTIONARY);
#endif
    datatypes.push_back(syncer::EXTENSIONS);
    datatypes.push_back(syncer::EXTENSION_SETTINGS);
    datatypes.push_back(syncer::PASSWORDS);
     datatypes.push_back(syncer::PREFERENCES);
     datatypes.push_back(syncer::SEARCH_ENGINES);
     datatypes.push_back(syncer::SESSIONS);
     datatypes.push_back(syncer::PROXY_TABS);
     datatypes.push_back(syncer::THEMES);
     datatypes.push_back(syncer::TYPED_URLS);
    return datatypes;
  }
