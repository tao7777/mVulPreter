GDataEntry* GDataFile::FromDocumentEntry(
    GDataDirectory* parent,
    DocumentEntry* doc,
    GDataDirectoryService* directory_service) {
  DCHECK(doc->is_hosted_document() || doc->is_file());
  GDataFile* file = new GDataFile(parent, directory_service);
  file->title_ = UTF16ToUTF8(doc->title());
 
   if (doc->is_file()) {
    file->file_info_.size = doc->file_size();
    file->file_md5_ = doc->file_md5();
 
     const Link* upload_link = doc->GetLinkByType(Link::RESUMABLE_EDIT_MEDIA);
     if (upload_link)
      file->upload_url_ = upload_link->href();
   } else {
    file->document_extension_ = doc->GetHostedDocumentExtension();
    file->file_info_.size = 0;
   }
  file->kind_ = doc->kind();
  const Link* edit_link = doc->GetLinkByType(Link::EDIT);
  if (edit_link)
    file->edit_url_ = edit_link->href();
  file->content_url_ = doc->content_url();
  file->content_mime_type_ = doc->content_mime_type();
  file->resource_id_ = doc->resource_id();
  file->is_hosted_document_ = doc->is_hosted_document();
  file->file_info_.last_modified = doc->updated_time();
  file->file_info_.last_accessed = doc->updated_time();
  file->file_info_.creation_time = doc->published_time();
  file->deleted_ = doc->deleted();
  const Link* parent_link = doc->GetLinkByType(Link::PARENT);
  if (parent_link)
    file->parent_resource_id_ = ExtractResourceId(parent_link->href());
  file->SetBaseNameFromTitle();
 
   const Link* thumbnail_link = doc->GetLinkByType(Link::THUMBNAIL);
   if (thumbnail_link)
    file->thumbnail_url_ = thumbnail_link->href();
 
   const Link* alternate_link = doc->GetLinkByType(Link::ALTERNATE);
   if (alternate_link)
    file->alternate_url_ = alternate_link->href();
  return file;
 }
