GDataEntry* GDataEntry::FromDocumentEntry(
    GDataDirectory* parent,
    DocumentEntry* doc,
    GDataDirectoryService* directory_service) {
  DCHECK(doc);
  if (doc->is_folder())
    return GDataDirectory::FromDocumentEntry(parent, doc, directory_service);
  else if (doc->is_hosted_document() || doc->is_file())
    return GDataFile::FromDocumentEntry(parent, doc, directory_service);
  return NULL;
}
