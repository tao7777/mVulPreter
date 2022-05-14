bool WebviewHandler::Parse(Extension* extension, base::string16* error) {
  scoped_ptr<WebviewInfo> info(new WebviewInfo());

  const base::DictionaryValue* dict_value = NULL;
  if (!extension->manifest()->GetDictionary(keys::kWebview,
                                            &dict_value)) {
    *error = base::ASCIIToUTF16(errors::kInvalidWebview);
     return false;
   }
 
  const base::ListValue* url_list = NULL;
  if (!dict_value->GetList(keys::kWebviewAccessibleResources,
                           &url_list)) {
    *error = base::ASCIIToUTF16(errors::kInvalidWebviewAccessibleResourcesList);
     return false;
   }
 
  for (size_t i = 0; i < url_list->GetSize(); ++i) {
    std::string relative_path;
    if (!url_list->GetString(i, &relative_path)) {
       *error = ErrorUtils::FormatErrorMessageUTF16(
          errors::kInvalidWebviewAccessibleResource, base::IntToString(i));
       return false;
     }
    URLPattern pattern(URLPattern::SCHEME_EXTENSION);
    if (pattern.Parse(extension->url().spec()) != URLPattern::PARSE_SUCCESS) {
       *error = ErrorUtils::FormatErrorMessageUTF16(
          errors::kInvalidURLPatternError, extension->url().spec());
       return false;
     }
    while (relative_path[0] == '/')
      relative_path = relative_path.substr(1, relative_path.length() - 1);
    pattern.SetPath(pattern.path() + relative_path);
    info->webview_accessible_resources_.AddPattern(pattern);
  }
 
  const base::ListValue* partition_list = NULL;
  if (!dict_value->GetList(keys::kWebviewPrivilegedPartitions,
                           &partition_list)) {
    *error = base::ASCIIToUTF16(errors::kInvalidWebviewPrivilegedPartitionList);
    return false;
  }
  for (size_t i = 0; i < partition_list->GetSize(); ++i) {
    std::string partition_wildcard;
    if (!partition_list->GetString(i, &partition_wildcard)) {
      *error = ErrorUtils::FormatErrorMessageUTF16(
          errors::kInvalidWebviewPrivilegedPartition, base::IntToString(i));
       return false;
     }
    info->webview_privileged_partitions_.push_back(partition_wildcard);
   }
   extension->SetManifestData(keys::kWebviewAccessibleResources, info.release());
   return true;
 }
