   bool HasPermissionsForFile(const FilePath& file, int permissions) {
     FilePath current_path = file.StripTrailingSeparators();
     FilePath last_path;
    int skip = 0;
     while (current_path != last_path) {
      FilePath base_name =  current_path.BaseName();
      if (base_name.value() == FilePath::kParentDirectory) {
        ++skip;
      } else if (skip > 0) {
        if (base_name.value() != FilePath::kCurrentDirectory)
          --skip;
      } else {
        if (file_permissions_.find(current_path) != file_permissions_.end())
          return (file_permissions_[current_path] & permissions) == permissions;
      }
       last_path = current_path;
       current_path = current_path.DirName();
     }

    return false;
  }
