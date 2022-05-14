GDataFile::GDataFile(GDataDirectory* parent,
GDataFile::GDataFile(GDataDirectoryService* directory_service)
    : GDataEntry(directory_service),
       kind_(DocumentEntry::UNKNOWN),
       is_hosted_document_(false) {
   file_info_.is_directory = false;
}
