bool SelectFileDialogImpl::RunOpenFileDialog(
    const std::wstring& title,
    const std::wstring& filter,
    HWND owner,
    FilePath* path) {
  OPENFILENAME ofn;
  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = owner;

  wchar_t filename[MAX_PATH];
  filename[0] = 0;
  FilePath dir;
  if (!path->empty()) {
    bool is_dir;
    base::PlatformFileInfo file_info;
    if (file_util::GetFileInfo(*path, &file_info))
      is_dir = file_info.is_directory;
    else
      is_dir = file_util::EndsWithSeparator(*path);
    if (is_dir) {
      ofn.lpstrInitialDir = path->value().c_str();
    } else {
      dir = path->DirName();
      ofn.lpstrInitialDir = dir.value().c_str();
      base::wcslcpy(filename, path->BaseName().value().c_str(),
                    arraysize(filename));
    }
  }

  ofn.lpstrFile = filename;
  ofn.nMaxFile = MAX_PATH;

  ofn.Flags = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
 
   if (!filter.empty())
     ofn.lpstrFilter = filter.c_str();
  bool success = CallGetOpenFileName(&ofn);
   DisableOwner(owner);
   if (success)
     *path = FilePath(filename);
  return success;
}
