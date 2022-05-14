GDataEntry* GDataDirectory::FromDocumentEntry(
void GDataDirectory::InitFromDocumentEntry(DocumentEntry* doc) {
  GDataEntry::InitFromDocumentEntry(doc);
 
   const Link* upload_link = doc->GetLinkByType(Link::RESUMABLE_CREATE_MEDIA);
   if (upload_link)
    upload_url_ = upload_link->href();
 }
