 MountError PerformFakeMount(const std::string& source_path,
                            const base::FilePath& mounted_path,
                            MountType type) {
   if (mounted_path.empty())
     return MOUNT_ERROR_INVALID_ARGUMENT;
 
  if (!base::CreateDirectory(mounted_path)) {
    DLOG(ERROR) << "Failed to create directory at " << mounted_path.value();
     return MOUNT_ERROR_DIRECTORY_CREATION_FAILED;
   }
 
  // Fake network mounts are responsible for populating their mount paths so
  // don't need a dummy file.
  if (type == MOUNT_TYPE_NETWORK_STORAGE)
    return MOUNT_ERROR_NONE;

   const base::FilePath dummy_file_path =
       mounted_path.Append("SUCCESSFULLY_PERFORMED_FAKE_MOUNT.txt");
  const std::string dummy_file_content = "This is a dummy file.";
  const int write_result = base::WriteFile(
      dummy_file_path, dummy_file_content.data(), dummy_file_content.size());
  if (write_result != static_cast<int>(dummy_file_content.size())) {
    DLOG(ERROR) << "Failed to put a dummy file at "
                << dummy_file_path.value();
    return MOUNT_ERROR_MOUNT_PROGRAM_FAILED;
  }

  return MOUNT_ERROR_NONE;
}
