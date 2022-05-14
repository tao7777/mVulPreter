 void GDataDirectoryService::InitializeRootEntry(const std::string& root_id) {
  root_.reset(CreateGDataDirectory());
   root_->set_title(kGDataRootDirectory);
   root_->SetBaseNameFromTitle();
   root_->set_resource_id(root_id);
  AddEntryToResourceMap(root_.get());
}
