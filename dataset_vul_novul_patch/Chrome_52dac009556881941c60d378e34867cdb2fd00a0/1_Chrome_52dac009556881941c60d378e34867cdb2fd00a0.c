 FilePath ExtensionPrefs::GetExtensionPath(const std::string& extension_id) {
   const DictionaryValue* dict = GetExtensionPref(extension_id);
   std::string path;
   if (!dict->GetString(kPrefPath, &path))
     return FilePath();

  return install_directory_.Append(FilePath::FromWStringHack(UTF8ToWide(path)));
}
