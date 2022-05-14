GDataDirectory::GDataDirectory(GDataDirectory* parent,
GDataDirectory::GDataDirectory(GDataDirectoryService* directory_service)
    : GDataEntry(directory_service) {
   file_info_.is_directory = true;
 }
