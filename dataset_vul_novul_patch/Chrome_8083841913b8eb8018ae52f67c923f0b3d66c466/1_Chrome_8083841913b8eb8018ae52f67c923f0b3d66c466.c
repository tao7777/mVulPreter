   bool HasPermissionsForFile(const FilePath& file, int permissions) {
     FilePath current_path = file.StripTrailingSeparators();
     FilePath last_path;
     while (current_path != last_path) {
      if (file_permissions_.find(current_path) != file_permissions_.end())
        return (file_permissions_[current_path] & permissions) == permissions;
       last_path = current_path;
       current_path = current_path.DirName();
     }

    return false;
  }
