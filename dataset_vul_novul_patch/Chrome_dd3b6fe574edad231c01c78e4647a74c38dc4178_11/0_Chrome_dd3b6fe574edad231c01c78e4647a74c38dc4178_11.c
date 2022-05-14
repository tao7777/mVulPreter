GDataEntry::GDataEntry(GDataDirectory* parent,
GDataEntry::GDataEntry(GDataDirectoryService* directory_service)
    : parent_(NULL),
      directory_service_(directory_service),
       deleted_(false) {
 }
