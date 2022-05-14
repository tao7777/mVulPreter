 GDataFile* AddFile(GDataDirectory* parent,
                    GDataDirectoryService* directory_service,
                    int sequence_id) {
  GDataFile* file = new GDataFile(NULL, directory_service);
   const std::string title = "file" + base::IntToString(sequence_id);
   const std::string resource_id = std::string("file_resource_id:") +
                                   title;
  file->set_title(title);
  file->set_resource_id(resource_id);
  file->set_file_md5(std::string("file_md5:") + title);
  GDataFileError error = GDATA_FILE_ERROR_FAILED;
  FilePath moved_file_path;
  directory_service->MoveEntryToDirectory(
      parent->GetFilePath(),
      file,
      base::Bind(&test_util::CopyResultsFromFileMoveCallback,
                 &error,
                 &moved_file_path));
  test_util::RunBlockingPoolTask();
  EXPECT_EQ(GDATA_FILE_OK, error);
  EXPECT_EQ(parent->GetFilePath().AppendASCII(title), moved_file_path);
  return file;
}
