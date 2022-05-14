GDataEntry* GDataFile::FromDocumentEntry(
void GDataFile::InitFromDocumentEntry(DocumentEntry* doc) {
  GDataEntry::InitFromDocumentEntry(doc);
 
   if (doc->is_file()) {
    file_info_.size = doc->file_size();
    file_md5_ = doc->file_md5();
 
     const Link* upload_link = doc->GetLinkByType(Link::RESUMABLE_EDIT_MEDIA);
     if (upload_link)
      upload_url_ = upload_link->href();
   } else {
    document_extension_ = doc->GetHostedDocumentExtension();
    file_info_.size = 0;
   }
  kind_ = doc->kind();
  content_mime_type_ = doc->content_mime_type();
  is_hosted_document_ = doc->is_hosted_document();
  SetBaseNameFromTitle();
 
   const Link* thumbnail_link = doc->GetLinkByType(Link::THUMBNAIL);
   if (thumbnail_link)
    thumbnail_url_ = thumbnail_link->href();
 
   const Link* alternate_link = doc->GetLinkByType(Link::ALTERNATE);
   if (alternate_link)
    alternate_url_ = alternate_link->href();
 }
