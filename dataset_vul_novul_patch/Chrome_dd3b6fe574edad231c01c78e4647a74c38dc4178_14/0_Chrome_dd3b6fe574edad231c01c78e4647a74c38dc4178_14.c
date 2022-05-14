 GDataDirectory* AddDirectory(GDataDirectory* parent,
                              GDataDirectoryService* directory_service,
                              int sequence_id) {
  GDataDirectory* dir = directory_service->CreateGDataDirectory();
   const std::string dir_name = "dir" + base::IntToString(sequence_id);
   const std::string resource_id = std::string("dir_resource_id:") +
                                   dir_name;
  dir->set_title(dir_name);
  dir->set_resource_id(resource_id);
  GDataFileError error = GDATA_FILE_ERROR_FAILED;
  FilePath moved_file_path;
  directory_service->MoveEntryToDirectory(
      parent->GetFilePath(),
      dir,
      base::Bind(&test_util::CopyResultsFromFileMoveCallback,
                 &error,
                 &moved_file_path));
  test_util::RunBlockingPoolTask();
  EXPECT_EQ(GDATA_FILE_OK, error);
  EXPECT_EQ(parent->GetFilePath().AppendASCII(dir_name), moved_file_path);
  return dir;
}
