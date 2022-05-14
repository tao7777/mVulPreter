 std::string GetUploadData(const std::string& brand) {
  std::string app_id(kDefaultAppID);
#if defined(OS_WIN)
  app_id = install_static::UTF16ToUTF8(install_static::GetAppGuid());
#endif  // defined(OS_WIN)
   DCHECK(!brand.empty());
  return base::StringPrintf(kPostXml, app_id.c_str(), brand.c_str());
 }
