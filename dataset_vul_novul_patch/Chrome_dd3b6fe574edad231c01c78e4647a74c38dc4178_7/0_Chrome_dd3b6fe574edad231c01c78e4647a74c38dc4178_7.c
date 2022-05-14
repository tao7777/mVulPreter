bool GDataDirectory::FromProto(const GDataDirectoryProto& proto) {
  DCHECK(proto.gdata_entry().file_info().is_directory());
   DCHECK(!proto.gdata_entry().has_file_specific_info());
 
   for (int i = 0; i < proto.child_files_size(); ++i) {
    scoped_ptr<GDataFile> file(directory_service_->CreateGDataFile());
     if (!file->FromProto(proto.child_files(i))) {
       RemoveChildren();
       return false;
     }
     AddEntry(file.release());
   }
   for (int i = 0; i < proto.child_directories_size(); ++i) {
    scoped_ptr<GDataDirectory> dir(directory_service_->CreateGDataDirectory());
     if (!dir->FromProto(proto.child_directories(i))) {
       RemoveChildren();
       return false;
    }
    AddEntry(dir.release());
  }

  if (!GDataEntry::FromProto(proto.gdata_entry()))
    return false;

  return true;
}
