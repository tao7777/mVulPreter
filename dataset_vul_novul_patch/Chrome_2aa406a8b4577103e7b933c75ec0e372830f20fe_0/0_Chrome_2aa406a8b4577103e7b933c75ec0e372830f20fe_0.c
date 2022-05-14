bool WebviewHandler::Parse(Extension* extension, base::string16* error) {
  scoped_ptr<WebviewInfo> info(new WebviewInfo());

  const base::DictionaryValue* dict_value = NULL;
  if (!extension->manifest()->GetDictionary(keys::kWebview,
                                            &dict_value)) {
    *error = base::ASCIIToUTF16(errors::kInvalidWebview);
     return false;
   }
 
  const base::ListValue* partition_list = NULL;
  if (!dict_value->GetList(keys::kWebviewPartitions, &partition_list)) {
    *error = base::ASCIIToUTF16(errors::kInvalidWebviewPartitionsList);
     return false;
   }
 
  // The partition list must have at least one entry.
  if (partition_list->GetSize() == 0) {
    *error = base::ASCIIToUTF16(errors::kInvalidWebviewPartitionsList);
    return false;
  }

  for (size_t i = 0; i < partition_list->GetSize(); ++i) {
    const base::DictionaryValue* partition = NULL;
    if (!partition_list->GetDictionary(i, &partition)) {
       *error = ErrorUtils::FormatErrorMessageUTF16(
          errors::kInvalidWebviewPartition, base::IntToString(i));
       return false;
     }

    std::string partition_pattern;
    if (!partition->GetString(keys::kWebviewName, &partition_pattern)) {
       *error = ErrorUtils::FormatErrorMessageUTF16(
          errors::kInvalidWebviewPartitionName, base::IntToString(i));
       return false;
     }
 
    const base::ListValue* url_list = NULL;
    if (!partition->GetList(keys::kWebviewAccessibleResources,
                            &url_list)) {
      *error = base::ASCIIToUTF16(
          errors::kInvalidWebviewAccessibleResourcesList);
      return false;
    }

    // The URL list should have at least one entry.
    if (url_list->GetSize() == 0) {
      *error = base::ASCIIToUTF16(
          errors::kInvalidWebviewAccessibleResourcesList);
       return false;
     }

    scoped_ptr<PartitionItem> partition_item(
        new PartitionItem(partition_pattern));

    for (size_t i = 0; i < url_list->GetSize(); ++i) {
      std::string relative_path;
      if (!url_list->GetString(i, &relative_path)) {
        *error = ErrorUtils::FormatErrorMessageUTF16(
            errors::kInvalidWebviewAccessibleResource, base::IntToString(i));
        return false;
      }
      URLPattern pattern(URLPattern::SCHEME_EXTENSION,
                         Extension::GetResourceURL(extension->url(),
                                                   relative_path).spec());
      partition_item->AddPattern(pattern);
    }
    info->AddPartitionItem(partition_item.Pass());
   }

   extension->SetManifestData(keys::kWebviewAccessibleResources, info.release());
   return true;
 }
