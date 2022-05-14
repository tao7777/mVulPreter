GDataFile::GDataFile(GDataDirectory* parent,
                     GDataDirectoryService* directory_service)
    : GDataEntry(parent, directory_service),
       kind_(DocumentEntry::UNKNOWN),
       is_hosted_document_(false) {
   file_info_.is_directory = false;
}
